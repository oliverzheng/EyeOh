#include <Windows.h>
#include <stdio.h>

#include "keyboardhook.h"

static UINT uMsg = 0;
static HWND hWndMain = NULL;
static HHOOK hKeyHook = NULL;
static HINSTANCE hInstance = 0;
static HOOKPROC lpfnHookProc = 0;

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
	{
		return TRUE;
	}
}

DECLDIR BOOL InjectKey(eyeoh::Key key)
{
	::INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = key.key;
	input.ki.time = 0;
	input.ki.dwExtraInfo = INJECT_KEY_IDENTIFIER;
	::SendInput(1, &input, sizeof(INPUT));

	return TRUE;
};