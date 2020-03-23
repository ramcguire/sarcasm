#include <stdio.h>
#include <Windows.h>
#define _WIN32_WINNT 0x050


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // maintain static bool to keep track of character case of last key
    static BOOL uppercase = TRUE;
    BOOL fEatKeystroke = FALSE;

    if (nCode == HC_ACTION) {
        switch (wParam) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
            // if vkCode is A-Z
            if (fEatKeystroke = (p->vkCode >= 0x41 && p->vkCode <= 0x5A)) {
                // get the char code
                UINT charCode = MapVirtualKeyA(p->vkCode, MAPVK_VK_TO_CHAR);
                INPUT uppercaseHelper;
                uppercaseHelper.type = INPUT_KEYBOARD;
                uppercaseHelper.ki.wVk = VK_SHIFT;
                INPUT input;
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = p->vkCode;
                input.ki.time = 0; // let system provide timestamp
                // keydown event
                if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) {
                    input.ki.dwFlags = 0;
                    if (uppercase) {
                        // press shift key
                        uppercaseHelper.ki.dwFlags = 0;
                        SendInput(1, &uppercaseHelper, sizeof(INPUT));
                        // send captured character
                        SendInput(1, &input, sizeof(INPUT));
                        // release shift key
                        uppercaseHelper.ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(1, &uppercaseHelper, sizeof(INPUT));
                        uppercase = FALSE;
                    } else {
                        SendInput(1, &input, sizeof(INPUT));
                        uppercase = TRUE;
                    }
                    
                }
                // keyup event
                else if ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)) {
                    input.ki.dwFlags = KEYEVENTF_KEYUP;
                    if (uppercase) {
                        SendInput(1, &input, sizeof(INPUT));                        
                    } else {
                        SendInput(1, &input, sizeof(INPUT));
                    }
                }
            }
        }
    }
    return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}

int main(){
    printf("Installing hook...\n");
    printf("Close this window to remove\n");
    // install keyboard hook
    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    printf("ReAdY! hAvE fUn :)\n");

    // keep app running and available to accept messages
    MSG msg;
    while (!GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hhkLowLevelKybd);

    return(0);
}