#include <Windows.h>
#include <stdio.h>

#include "keyboardhook.h"

static UINT uMsg = 0;
static HWND hWndMain = NULL;
static HHOOK hKeyHook = NULL;
static HINSTANCE hInstance = 0;
static HOOKPROC lpfnHookProc = 0;
static BOOL enabled = TRUE;
static unsigned short enable_key = 0;

LRESULT __stdcall KeyboardFunc (int nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain (HANDLE hModule, DWORD dwReason, LPVOID lpReserved) 
{
	UNREFERENCED_PARAMETER(lpReserved);

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		hInstance  = (HINSTANCE)hModule; // the DLLs instance handle.
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

DECLDIR BOOL InstallKeyboardHook(HWND hWnd, UINT uMyMsg)
{
	uMsg = uMyMsg;
	lpfnHookProc = (HOOKPROC)KeyboardFunc;
	hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, lpfnHookProc, hInstance, NULL);
	hWndMain = hWnd;
	enabled = TRUE;
 
	if (hKeyHook)
		return TRUE;
	else
		return FALSE;
}

LRESULT CALLBACK KeyboardFunc(int nCode, WPARAM wParam, LPARAM lParam)
{ 
	BOOL bPassThrough = TRUE;

	if (nCode < 0)
		return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

	PKBDLLHOOKSTRUCT pkbHook = (PKBDLLHOOKSTRUCT)lParam;
	eyeoh::Key key;
	unsigned char processed = 0;

	switch (wParam)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		key.key = (unsigned short)pkbHook->vkCode;
		if (!enabled && key.key != enable_key)
			break;

		if (GetKeyState(VK_SHIFT) < 0)
			key.modifiers |= eyeoh::Key::SHIFT;

		if (GetKeyState(VK_CONTROL) < 0)
			key.modifiers |= eyeoh::Key::CTRL;

		if (GetKeyState(VK_MENU) < 0)
			key.modifiers |= eyeoh::Key::ALT;

		if (pkbHook->dwExtraInfo == INJECT_KEY_IDENTIFIER)
			bPassThrough = TRUE;
		else
		{
			SendMessage(hWndMain, uMsg, (WPARAM)&key, (LPARAM)&processed);
			if (processed)
				bPassThrough = FALSE;
			else
				bPassThrough = TRUE;
		}
		break;

	default : 
		bPassThrough = TRUE;
		break;
	}

	if (bPassThrough)
		return (CallNextHookEx(hKeyHook, nCode, wParam, lParam));
	else
		return TRUE;
}

#define DO_WHILE_0_BEGIN do {
#define DO_WHILE_0_END \
	__pragma(warning(push)) \
	__pragma(warning(disable:4127)) \
	} while(0) \
	__pragma(warning(pop))

#define SENDKEY(input, key, extended, up) \
	DO_WHILE_0_BEGIN \
		input->type = INPUT_KEYBOARD; \
		input->ki.wVk = key; \
		input->ki.time = 0; \
		input->ki.dwExtraInfo = INJECT_KEY_IDENTIFIER; \
		__pragma(warning(push)) \
		__pragma(warning(disable:4127)) \
		if (extended) \
			input->ki.dwFlags |= KEYEVENTF_EXTENDEDKEY; \
		if (up) \
			input->ki.dwFlags |= KEYEVENTF_KEYUP; \
		__pragma(warning(pop)) \
		input++; \
	DO_WHILE_0_END

#define SENDKEYDOWN(input, key, extended) SENDKEY(input, key, extended, 0)
#define SENDKEYUP(input, key, extended) SENDKEY(input, key, extended, 1)

enum PressState {
	NOCHANGE,
	DOWN,
	UP,
};

#define SEND(input, modifiers, mask, vk, send) \
	PressState send##_sent = NOCHANGE; \
	DO_WHILE_0_BEGIN \
		/* Change modifier state if current state is not the desired */ \
		if ((modifiers & mask) ^ (GetKeyState(vk) < 0)) \
		{ \
			if (modifiers & mask) \
			{ \
				send##_sent = DOWN; \
				SENDKEYDOWN(input, send, 0); \
			} \
			else \
			{ \
				send##_sent = UP; \
				SENDKEYUP(input, send, 0); \
			} \
		} \
	DO_WHILE_0_END

#define RESTORE(input, send) \
	DO_WHILE_0_BEGIN \
		if (send##_sent == DOWN) \
			SENDKEYUP(input, send, 0); \
		else if (send##_sent == UP) \
			SENDKEYDOWN(input, send, 0); \
	DO_WHILE_0_END

#define MAX_KEYPRESSES	8

// This is not documented anywhere.
// See http://www.experts-exchange.com/Programming/Languages/Pascal/Delphi/Q_21737163.html
static const unsigned short EXTENDED_KEYS[] = {
	VK_UP,
	VK_DOWN,
	VK_LEFT,
	VK_RIGHT,
	VK_HOME,
	VK_END,
	VK_PRIOR,
	VK_NEXT,
	VK_INSERT,
	VK_DELETE
};

DECLDIR BOOL InjectKey(eyeoh::Key key)
{
	if (key.key == 0)
		return TRUE;

	::INPUT inputs[MAX_KEYPRESSES] = {0};
	::INPUT * input = inputs;

	BOOL extended = FALSE;

	for (int i = 0; i < sizeof(EXTENDED_KEYS) / sizeof(EXTENDED_KEYS[0]); i++)
	{
		if (key.key == EXTENDED_KEYS[i])
		{
			extended = TRUE;
			break;
		}
	}

	SEND(input, key.modifiers, eyeoh::Key::SHIFT, VK_SHIFT, VK_LSHIFT);
	SEND(input, key.modifiers, eyeoh::Key::ALT, VK_MENU, VK_LMENU);
	SEND(input, key.modifiers, eyeoh::Key::CTRL, VK_CONTROL, VK_LCONTROL);

	SENDKEYDOWN(input, key.key, extended);
	SENDKEYUP(input, key.key, extended);

	RESTORE(input, VK_LCONTROL);
	RESTORE(input, VK_LMENU);
	RESTORE(input, VK_LSHIFT);

	::SendInput(input - inputs, inputs, sizeof(INPUT));

	return TRUE;
};

DECLDIR VOID Disable(unsigned short enable_keycode)
{
	enable_key = enable_keycode;
	enabled = FALSE;
}

DECLDIR VOID Enable()
{
	enabled = TRUE;
}