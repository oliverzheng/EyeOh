#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

#ifdef _EXPORTING
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include <Windows.h>
#include "Keyboard.h"

#define INJECT_KEY_IDENTIFIER	1

DECLDIR BOOL InjectKey(eyeoh::Key key);
DECLDIR BOOL InstallKeyboardHook (HWND hWnd, UINT uMyMsg);

#endif