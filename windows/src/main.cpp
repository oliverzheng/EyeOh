#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>

#include "win_config.h"
#include "resource.h"

#include "WindowsKeyboard.h"
#include "keyboardhook.h"
#include "UserConfig.h"
#include "StateMap.h"

#include "KeyboardOutput.h"

using namespace eyeoh;
using namespace std;

class EyeOh
{
public:
	static bool MakeWindow(::HINSTANCE hInstance, StateMap & statemap);
	static void Cleanup();
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static const int HOOK_MSG = (WM_USER + 0);

	static ::HINSTANCE hInstance;
	static ::HWND hWnd;
	static StateMap * statemap;

	EyeOh() {};
};

::HINSTANCE EyeOh::hInstance = NULL;
::HWND EyeOh::hWnd = NULL;
StateMap * EyeOh::statemap = NULL;

bool EyeOh::MakeWindow(::HINSTANCE hInstance, StateMap & statemap)
{
	EyeOh::hInstance = hInstance;
	EyeOh::statemap = &statemap;

	WNDCLASS wndclass ;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = EyeOh::WndProc;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = EyeOh::hInstance;
	wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = TEXT(WNDCLS);

	if (!RegisterClass(&wndclass))
		return false;

	EyeOh::hWnd = ::CreateWindow(TEXT(APPNAME), TEXT(APPNAME), 0,
									CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
									HWND_MESSAGE, NULL, hInstance, NULL);

	if (EyeOh::hWnd == NULL)
		return false;

	if (!InstallKeyboardHook(EyeOh::hWnd, EyeOh::HOOK_MSG))
		return false;

	return true;
}

void EyeOh::Cleanup()
{
}

LRESULT CALLBACK EyeOh::WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Key & key = *(Key *)wParam;
	unsigned char & processed = *(unsigned char *)lParam;
	pair<StateMap::ChangeResult, StateMap::OutputResult> result;

	switch (message)
	{
	case HOOK_MSG:
		result = EyeOh::statemap->OnPress(key);
		if (result.first == StateMap::RESET && result.second == StateMap::OUTPUT)
			processed = 1;
		else
			processed = 0;
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

void LoadFileInResource(int name, int type, DWORD * size, const char ** data)
{
	HMODULE handle = ::GetModuleHandle(NULL);
	HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(type));
	HGLOBAL rcData = ::LoadResource(handle, rc);
	*size = ::SizeofResource(handle, rc);
	*data = static_cast<const char*>(::LockResource(rcData));
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    DWORD size = 0;
    const char * data = NULL;
    LoadFileInResource(IDR_WINDOWSKEYS, TEXTFILE, &size, &data);
    char * buffer = new char[size+1];
    ::memcpy(buffer, data, size);
    buffer[size] = 0; // NULL terminator

	WindowsKeyboard keyboard(buffer);

    delete[] buffer;

	UserConfig userconfig(keyboard);

	// Plugins
	userconfig.AddOutput(KeyboardOutput::Create);

	::TCHAR szPath[MAX_PATH];
	::PTSTR ppszPath = NULL;
	if (::SHGetKnownFolderPath(::FOLDERID_RoamingAppData, 0, NULL, &ppszPath) != S_OK)
		return -1;
	if (::PathCombine(szPath, ppszPath, TEXT(APPNAME)) == NULL)
		return -1;
	::CoTaskMemFree(ppszPath);

	if (::CreateDirectory(szPath, NULL) == 0)
		if (::GetLastError() != ERROR_ALREADY_EXISTS)
			return -1;
	if (::PathAppend(szPath, TEXT("\\")) == NULL ||
		::PathAppend(szPath, TEXT(CONFIG_FILENAME)) == NULL)
		return -1;

	wstring filepath(szPath);
	if (!userconfig.ParseFile(string(filepath.begin(), filepath.end())))
		return -1;

	if (!EyeOh::MakeWindow(hInstance, userconfig.GetStateMap()))
		return -1;

	MSG msg;
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	EyeOh::Cleanup();

	return msg.wParam ;
}
