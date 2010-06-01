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
		key.key = pkbHook->vkCode;
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

#define SENDKEY(input, key, up) \
	do { \
		input->type = INPUT_KEYBOARD; \
		input->ki.wVk = key; \
		input->ki.time = 0; \
		input->ki.dwExtraInfo = INJECT_KEY_IDENTIFIER; \
		if (up) \
			input->ki.dwFlags = KEYEVENTF_KEYUP; \
		input++; \
	} while (0)

#define SENDKEYDOWN(input, key) SENDKEY(input, key, 0)
#define SENDKEYUP(input, key) SENDKEY(input, key, 1)

enum PressState {
	NOCHANGE,
	DOWN,
	UP,
};

#define SEND(input, modifiers, mask, vk, send) \
	PressState send##_sent = NOCHANGE; \
	do { \
		/* Change modifier state if current state is not the desired */ \
		if ((modifiers & mask) ^ (GetKeyState(vk) < 0)) \
		{ \
			if (modifiers & mask) \
			{ \
				send##_sent = DOWN; \
				SENDKEYDOWN(input, send); \
			} \
			else \
			{ \
				send##_sent = UP; \
				SENDKEYUP(input, send); \
			} \
		} \
	} while (0)

#define RESTORE(input, send) \
	do { \
		if (send##_sent == DOWN) \
			SENDKEYUP(input, send); \
		else if (send##_sent == UP) \
			SENDKEYDOWN(input, send); \
	} while (0)

#define MAX_KEYPRESSES	8

DECLDIR BOOL InjectKey(eyeoh::Key key)
{
	::INPUT inputs[MAX_KEYPRESSES] = {0};
	::INPUT * input = inputs;

	if (key.key == 0)
		return TRUE;

	SEND(input, key.modifiers, eyeoh::Key::SHIFT, VK_SHIFT, VK_LSHIFT);
	SEND(input, key.modifiers, eyeoh::Key::ALT, VK_MENU, VK_LMENU);
	SEND(input, key.modifiers, eyeoh::Key::CTRL, VK_CONTROL, VK_LCONTROL);

	SENDKEYDOWN(input, key.key);
	SENDKEYUP(input, key.key);

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