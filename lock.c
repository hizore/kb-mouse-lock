#include <stdio.h>
#include <windows.h>

HHOOK keyboardHook;
HHOOK mouseHook;
BOOL blockKeyboardInput = TRUE;
BOOL blockMouseInput = TRUE;
BOOL IsUserAnAdmin();

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_KEYDOWN)
    {
        KBDLLHOOKSTRUCT *kb = (KBDLLHOOKSTRUCT *)lParam;
        if (kb->vkCode == VK_MEDIA_PLAY_PAUSE)
        {
            blockKeyboardInput = !blockKeyboardInput;
            blockMouseInput = blockKeyboardInput;
            return 1;
        }
    }
    if (blockKeyboardInput)
    {
        return 1;
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        MSLLHOOKSTRUCT *ms = (MSLLHOOKSTRUCT *)lParam;
        if ((wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN || wParam == WM_MOUSEWHEEL) && blockMouseInput)
        {
            return 1;
        }
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

int main()
{
    if (!IsUserAnAdmin())
    {
        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = "runas";
        sei.lpFile = "lock.exe";
        sei.hwnd = NULL;
        sei.nShow = SW_HIDE;
        if (!ShellExecuteEx(&sei))
        {
            MessageBox(NULL, "Failed to request administrator rights", "Error", MB_OK | MB_ICONERROR);
            return 1;
        }
        return 0;
    }

    ShowWindow(GetConsoleWindow(), SW_HIDE);

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
    UnhookWindowsHookEx(mouseHook);

    return 0;
}
