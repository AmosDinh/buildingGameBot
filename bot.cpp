#include <iostream>
#include <string>
#include <windows.h>
using namespace std;
//g++ bot.cpp -lgdi32

struct
{
    int top, right, bottom, left;
    int width, height;
    int blockSize;
} window;
struct
{
    int *p1, *p2, *p3, *p4;

    int *pastelBlue;
    int *doorGreyBorder;

} colors;
struct
{
    int center[2];
} player;
struct
{
    float punch1 = 0.0f, punch2 = 0.0f, punch3 = 0.0f,
          punch4 = 0.0f, punch5 = 0.0f, punch6 = 0.0f, punch7;

    float left = 0.0f, right = 0.0f;

    float jump1 = 0.0f, jump2 = 0.0f, jump3 = 0.0f;

} delayTimes;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    char title[80];
    GetWindowTextA(hwnd, title, sizeof(title));
    if (strcmp("Growtopia", title) == 0)
    {
        RECT rect;
        GetWindowRect(hwnd, &rect);
        window.left = (int)rect.left + 8;
        window.top = (int)rect.top + 31;
        window.right = (int)rect.right - 8;
        window.bottom = (int)rect.bottom - 8;
        window.width = window.right - window.left;
        window.height = window.bottom - window.top;

        cout << "window : x1 " << window.left << ", y1 " << window.top << ", x2 " << window.right << ", y2 " << window.bottom << endl;
    }

    return TRUE;
}
BYTE *ScreenData = 0;
void screenshot()
{
    int x1 = window.left;
    int y1 = window.top;
    int x2 = window.right;
    int y2 = window.bottom;

    int w = window.width;
    int h = window.height;

    // get the device context of the screen
    HDC hScreen = GetDC(NULL);
    /*   int w = GetDeviceCaps(hScreen, HORZRES);
    int h = GetDeviceCaps(hScreen, VERTRES); */
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    /* BOOL bRet =  */ //BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);
    BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

    SelectObject(hDC, old_obj);
    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();

    BITMAPINFOHEADER bmi = {0};
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biWidth = w;
    bmi.biHeight = -h;
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = 0;

    if (ScreenData)
        free(ScreenData);
    ScreenData = (BYTE *)malloc(4 * w * h);

    GetDIBits(hDC, hBitmap, 0, h, ScreenData, (BITMAPINFO *)&bmi, DIB_RGB_COLORS);

    ReleaseDC(GetDesktopWindow(), hScreen);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
}
int *getPx(int x, int y)
{
    int w = window.width;

    int pos = 4 * ((y * w) + x);
    int *arr = new int[3];

    arr[0] = (int)ScreenData[pos + 2];
    arr[1] = (int)ScreenData[pos + 1];
    arr[2] = (int)ScreenData[pos];
    cout << "rgb (" << arr[0] << " " << arr[1] << " " << arr[2] << ")" << endl;
    return arr;

    // now your image is held in hBitmap. You can save it or do whatever with it
}
/* int (&getPx(int x,int y))[3]{
  int w = window.width;

    int pos = 4 * ((y * w) + x);
    int arr[3];

    arr[0] = (int)ScreenData[pos + 2];
    arr[1] = (int)ScreenData[pos + 1];
    arr[2] = (int)ScreenData[pos];
    cout << "rgb (" << arr[0] << " " << arr[1] << " " << arr[2] << ")" << endl;
    return arr;
} */

int getColorSum(int arr[3])
{
    int sum = arr[0] + arr[1] + arr[2];
    return sum;
}
bool ButtonPress(int Key)
{
    bool button_pressed = false;

    while (GetAsyncKeyState(Key))
        button_pressed = true;

    return button_pressed;
}
void setPlayerwindow()
{
}
//sets blockWidth and gets player colors
void setBlockSize()

{
    screenshot();
    int yCoord = 0;
    int pastelBlueSum = getColorSum(colors.pastelBlue);
    int doorSum = getColorSum(colors.doorGreyBorder);

    for (int y = 0; y <= window.height; y++)
    {
        int sum = getColorSum(getPx(0, y));
        if (sum != pastelBlueSum)
        {
            yCoord = y;
            break;
        }
    }
    bool playerHasColor = false;
    int pCX = 0;
    int pCY = 0;
    int blockSize = 0;

    int y = yCoord;
Loop1:
    for (int x = 0; x <= window.width / 3; x++)
    {
        int *px = getPx(x, y);
        int sum = getColorSum(px);
        delete px;
        if (sum == pastelBlueSum)
        {
            blockSize = x - 1;
            break;
        }
        if (!playerHasColor && sum != doorSum)
        {
            pCX = x;
            pCY = y;
            playerHasColor = true;
        }
    }
    if (!playerHasColor)
    {
        y += 1;
        goto Loop1;
    }
    window.blockSize = blockSize;
    int *pC1 = getPx(pCX, pCY);
    colors.p1 = pC1;
    int checkColor = getColorSum(pC1);
    int counter = 1;
    for (int x = pCX; x < blockSize; x++)
    {
        int *px = getPx(x, pCY);
        int sum = getColorSum(px);
        if (sum != checkColor)
        {
            counter += 1;
            if (counter == 4)
            {
                pCX = x - 1;
                break;
            }
            if (counter == 2)
            {
                colors.p2 = px;
            }
            else if (counter == 3)
            {
                colors.p3 = px;
            }
            checkColor = sum;
        }
    }
    for (int y = pCY; y < pCY + blockSize; y++)
    {
        int *px = getPx(pCX, y);
        int sum = getColorSum(px);
        if (sum != checkColor)
        {
            colors.p3 = px;
            break;
        }
    }
    cout << "player colors" << *colors.p1 << *colors.p2 << *colors.p3 << *colors.p4 << endl;
    cout << "blockSize: " << window.blockSize << endl;
}
void keyDown(WORD word, float delay)
{
    // This structure will be used to create the keyboard
    // input event.
    INPUT ip;

    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Press the "A" key
    ip.ki.wVk = word;  // virtual-key code for the "a" key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
    Sleep(delay);
    // Release the "A" key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}
void doMove(string action)
{
    WORD up = 0x26;
    WORD left = 0x25;
    WORD right = 0x27;
    WORD space = 0x20;

    if (action == "right")
    {
        keyDown(right, delayTimes.right);
    }
    else if (action == "left")
    {
        keyDown(right, delayTimes.left);
    }
    else if (action == "punch1")
    {
        keyDown(space, delayTimes.punch1);
    }
    else if (action == "punch2")
    {
        keyDown(space, delayTimes.punch2);
    }
    else if (action == "punch3")
    {
        keyDown(space, delayTimes.punch3);
    }
    else if (action == "punch4")
    {
        keyDown(space, delayTimes.punch4);
    }
    else if (action == "punch5")
    {
        keyDown(space, delayTimes.punch5);
    }
    else if (action == "punch6")
    {
        keyDown(space, delayTimes.punch6);
    }
    else if (action == "punch7")
    {
        keyDown(space, delayTimes.punch7);
    }
    else if (action == "jump1")
    {
        keyDown(up, delayTimes.jump1);
    }
    else if (action == "jump2")
    {
        keyDown(up, delayTimes.jump2);
    }
    else if (action == "jump3")
    {
        keyDown(up, delayTimes.jump3);
    }
}
int main()
{
    EnumWindows(EnumWindowsProc, NULL);

    /*    while (true)
    {
        if (ButtonPress(VK_SPACE))
        {  

            // Print out current cursor position
            POINT p;
            GetCursorPos(&p);
            cout << "x:" << (int)p.x << "y:" << (int)p.y << endl;
            // Print out RGB value at that position

            int *color = getPx((int)p.x, (int)p.y);
            cout << "rgb(" << color[0] << " " << color[1] << " " << color[2] << ")" << endl;
            delete color;
        }
        else if (ButtonPress(VK_ESCAPE))
        {
            cout << "quit" << endl;
            break;
        }
    } */

    //screenshot(window.left, window.top, window.right, window.bottom); */
    return 0;
}

/* void screenshot(int x1, int y1, int x2, int y2)
{

    int w = x2 - x1;
    int h = y2 - y1;

    // get screen dimensions
 

    // copy screen to bitmap
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

    // save bitmap to clipboard
    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();
    //ShellExecute(0, 0, L"c:\\outfile.txt", 0, 0 , SW_SHOW );
    //graphics.DrawImage(&hBitmap, 10, 10);

    // clean up
    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
} */
