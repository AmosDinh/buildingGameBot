#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <boost/thread.hpp>
using namespace std;
//g++ bot.cpp -lgdi32
//X:\WinPython-32bit-3.4.3.5\python-3.4.3\share\mingwpy\bin\g++.exe bot.cpp -lgdi32

struct
{
    int top, right, bottom, left;
    int width, height;
    int deskWidth, deskHeight;
    int blockSize;
} window;

struct
{
    int *p1, *p2, *p3, *p4;

    int pastelBlue[3] = {96, 213, 240};
    int doorGreyBorder[3] = {162, 162, 162};

} colors;
struct
{
    int center[2];
} player;
struct
{
    int punch1 = 90, punch2 = 220, punch3 = 423,
        punch4 = 625, punch5 = 830, punch6 = 1032, punch7 = 1232, punch8 = 2000, punch9 = 2000, punch10 = 1000;

    /*   int left = 163, right = 163; //no boots */
    int left = 162, right = 162;
    int jump1 = 50, jump2 = 153, jump3 = 320;

} delayTimes;
struct turnHandle
{
    int turns = 1;
    string action1 = "none";
    string action2 = "none";
    string action3 = "none";
};

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

        RECT desktop;
        // Get a handle to the desktop window
        // Get the size of screen to the variable desktop
        GetWindowRect(GetDesktopWindow(), &desktop);
        window.deskHeight = desktop.bottom;
        window.deskWidth = desktop.right;

        cout << "deskWidth,Height: " << window.deskWidth << " " << window.deskHeight << endl;
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
    // cout << "rgb (" << arr[0] << " " << arr[1] << " " << arr[2] << ")" << endl;
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
void mouseMove(int x, int y);

int *getPlayer(int divider = 3)
{
    screenshot();

    //scan for playerposition
    int bl = window.blockSize;
    int *p1 = colors.p1,
        *p2 = colors.p2,
        *p3 = colors.p3,
        *p4 = colors.p4;
    int sum1 = getColorSum(p1),
        sum2 = getColorSum(p2),
        sum3 = getColorSum(p3),
        sum4 = getColorSum(p4);

    int pX, pY;
SearchCoordinates:
    int step = bl / divider;

    for (int y = step; y < window.height - step - 1; y += step)
    {
        for (int x = step; x < window.width - step - 1; x += step)
        {

            int *px = getPx(x, y);
            int sum = getColorSum(px);
            if (sum == sum2 || sum == sum1 || sum == sum3 || sum == sum4)
            {
                if ((px[0] == p1[0] && px[1] == p1[1]) ||
                    (px[0] == p2[0] && px[1] == p2[1]) ||
                    (px[0] == p3[0] && px[1] == p3[1]) ||
                    (px[0] == p3[0] && px[1] == p3[1]))
                {
                    pX = x;
                    pY = y;
                    delete[] px;
                    goto Skip;
                }
            }
            delete[] px;
        }
    }
    divider = divider + 1;
    goto SearchCoordinates;

Skip:
    int topY = pY, topX = pX;
    int pYStart = pY - bl;
    if (pYStart < 1)
        pYStart = 1;
    for (int y = pYStart; y < pY; y += 2)
    {
        int *px = getPx(pX, y);

        int sum = getColorSum(px);
        delete[] px;
        if (sum == sum2 || sum == sum1 || sum == sum3 || sum == sum4)
        {
            topY = y;
            break;
        }
    }
    int pXStart = pX - bl;
    if (pXStart < 1)
        pXStart = 1;
    for (int x = pXStart; x <= pX; x += 2)
    {
        int *px = getPx(x, topY);
        int sum = getColorSum(px);
        delete[] px;
        if (sum == sum2 || sum == sum1 || sum == sum3 || sum == sum4)
        {
            topX = x;
            break;
        }
    }

    int *coords = new int[2];

    coords[0] = (int)topX + (bl * 0.31);
    coords[1] = (int)topY + (bl * 0.5);
    cout << "Player center: " << coords[0] << " " << coords[1] << endl;
    //mouseMove(coords[0], coords[1]);
    return coords;
}
//sets blockWidth and gets player colors
void setBlockSize()

{
    screenshot();
    int yCoord = 0;
    int pastelBlueSum = getColorSum(colors.pastelBlue);
    int doorSum = getColorSum(colors.doorGreyBorder);

    // get Y coord by checking for not pastelBlue anymore
    for (int y = 0; y <= window.height; y++)
    {
        int *px = getPx(0, y);
        int sum = getColorSum(px);
        delete[] px;
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
    //get blockSize by checking horizontally on y coord  when it is pastelBlue again
    //get coordinates of upper left corner of player by checking not doorcolor anymore
    //if we didnt find player coordinates we y++
Loop1:
    for (int x = 0; x <= window.width / 3; x++)
    {
        int *px = getPx(x, y);
        int sum = getColorSum(px);
        delete[] px;
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

    //get other colors of player
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
                delete[] px;
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
        else
            delete[] px;
    }

    for (int y = pCY; y < pCY + blockSize; y++)
    {
        int *px = getPx(pCX, y);
        int sum = getColorSum(px);

        if (sum != checkColor)
        {
            colors.p4 = px;
            break;
        }
        delete[] px;
    }
    int *a = colors.p1;
    cout << "p1 " << a[0] << " " << a[1] << " " << a[2] << endl;
    a = colors.p2;
    cout << "p2 " << a[0] << " " << a[1] << " " << a[2] << endl;
    a = colors.p3;
    cout << "p3 " << a[0] << " " << a[1] << " " << a[2] << endl;
    a = colors.p4;
    cout << "p4 " << a[0] << " " << a[1] << " " << a[2] << endl;

    cout << "blockSize: " << window.blockSize << endl;
}
void mouseMove(int x, int y)
{
    SetCursorPos(x + window.left, y + window.top);
}
void mouseClick(int x, int y)
{
    INPUT Inputs[3] = {0};

    Inputs[0].type = INPUT_MOUSE;
    Inputs[0].mi.dx = x * (65535 / window.deskWidth);  // desired X coordinate
    Inputs[0].mi.dy = y * (65535 / window.deskHeight); // desired Y coordinate
    Inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

    Inputs[1].type = INPUT_MOUSE;
    Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(3, Inputs, sizeof(INPUT));

    Inputs[2].type = INPUT_MOUSE;
    Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(3, Inputs, sizeof(INPUT));
}
void mousePress(int delay, int yOffset)
{
    int *coords = getPlayer();
    int x = (window.left + coords[0]) * (65535 / window.deskWidth);
    int y = (window.top + coords[1] + (int)(yOffset * window.blockSize)) * (65535 / window.deskHeight);

    cout << "mousepress: x" << x << " y" << y << endl;

    delete[] coords;
    INPUT Inputs[3] = {0};

    Inputs[0].type = INPUT_MOUSE;
    Inputs[0].mi.dx = x; // desired X coordinate
    Inputs[0].mi.dy = y; // desired Y coordinate
    Inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

    Inputs[1].type = INPUT_MOUSE;
    Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(3, Inputs, sizeof(INPUT));
    Sleep(delay);

    Inputs[2].type = INPUT_MOUSE;
    Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(3, Inputs, sizeof(INPUT));
}
void keyDown(WORD word, int delay)
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
    // KEYEVENTF_KEYUP for key release
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}
void doMove(string action, int delay)
{
    cout << action << endl;
    WORD up = 0x26;
    WORD left = 0x25;
    WORD right = 0x27;
    WORD space = 0x20;
    if (action == "right")

        keyDown(right, delayTimes.right);

    else if (action == "left")

        keyDown(left, delayTimes.left);

    else if (action.find("rel") != string::npos)
    {

        int yOffset = 0;
        if (action.find("top") != string::npos)
            yOffset = -1 * ((int)action[action.length() - 1] - 48);
        else if (action.find("bottom") != string::npos)
            yOffset = (int)action[action.length() - 1] - 48;
        char check1 = action[8];
        char check2 = action[9];
        int delayTime;
        if (check1 == '1')
        {
            if (check2 == '0')
                delayTime = delayTimes.punch10;

            else
                delayTime = delayTimes.punch1;
        }
        else if (check1 == '2')
            delayTime = delayTimes.punch2;
        else if (check1 == '3')
            delayTime = delayTimes.punch3;
        else if (check1 == '4')
            delayTime = delayTimes.punch4;
        else if (check1 == '5')
            delayTime = delayTimes.punch5;
        else if (check1 == '6')
            delayTime = delayTimes.punch6;
        else if (check1 == '7')
            delayTime = delayTimes.punch7;
        else if (check1 == '8')
            delayTime = delayTimes.punch8;
        else if (check1 == '9')
            delayTime = delayTimes.punch9;

        mousePress(delayTime, yOffset);
    }
    else if (action.find("punch") != string::npos)
    {
        char check1 = action[5];
        char check2 = action[6];
        int delayTime;
        if (check1 == '1')
        {
            if (check2 == '0')
                delayTime = delayTimes.punch10;
            else
                delayTime = delayTimes.punch1;
        }
        else if (check1 == '2')
            delayTime = delayTimes.punch2;
        else if (check1 == '3')
            delayTime = delayTimes.punch3;
        else if (check1 == '4')
            delayTime = delayTimes.punch4;
        else if (check1 == '5')
            delayTime = delayTimes.punch5;
        else if (check1 == '6')
            delayTime = delayTimes.punch6;
        else if (check1 == '7')
            delayTime = delayTimes.punch7;
        else if (check1 == '8')
            delayTime = delayTimes.punch8;
        else if (check1 == '9')
            delayTime = delayTimes.punch9;

        cout << "keyDown " << delayTime << endl;
        keyDown(space, delayTime);
    }
    else if (action.find("jump") != string::npos)
    {
        if (action == "jump1")
            keyDown(up, delayTimes.jump1);

        else if (action == "jump2")
            keyDown(up, delayTimes.jump2);

        else if (action == "jump3")
            keyDown(up, delayTimes.jump3);
    }

    /*  if (action == "right")

        keyDown(right, delayTimes.right);

    else if (action == "left")

        keyDown(left, delayTimes.left);

    else if (action.find("rel") != string::npos)
    {

        int yOffset = 0;
        if (action.find("top") != string::npos)
            yOffset = -1 * ((int)action[action.length() - 1] - 48);
        else if (action.find("bottom") != string::npos)
            yOffset = (int)action[action.length() - 1] - 48;
        char check1 = action[8];
        char check2 = action[9];
        int delayTime;
        if (check1 == '1')
        {
            if (check2 == '0')
                delayTime = delayTimes.punch10;

            else
                delayTime = delayTimes.punch1;
        }
        else if (check1 == '2')
            delayTime = delayTimes.punch2;
        else if (check1 == '3')
            delayTime = delayTimes.punch3;
        else if (check1 == '4')
            delayTime = delayTimes.punch4;
        else if (check1 == '5')
            delayTime = delayTimes.punch5;
        else if (check1 == '6')
            delayTime = delayTimes.punch6;
        else if (check1 == '7')
            delayTime = delayTimes.punch7;
        else if (check1 == '8')
            delayTime = delayTimes.punch8;
        else if (check1 == '9')
            delayTime = delayTimes.punch9;

        mousePress(delayTime, yOffset);
    }
    else if (action.find("punch") != string::npos)
    {
        char check1 = action[5];
        char check2 = action[6];
        int delayTime;
        if (check1 == '1')
        {
            if (check2 == '0')
                delayTime = delayTimes.punch10;
            else
                delayTime = delayTimes.punch1;
        }
        else if (check1 == '2')
            delayTime = delayTimes.punch2;
        else if (check1 == '3')
            delayTime = delayTimes.punch3;
        else if (check1 == '4')
            delayTime = delayTimes.punch4;
        else if (check1 == '5')
            delayTime = delayTimes.punch5;
        else if (check1 == '6')
            delayTime = delayTimes.punch6;
        else if (check1 == '7')
            delayTime = delayTimes.punch7;
        else if (check1 == '8')
            delayTime = delayTimes.punch8;
        else if (check1 == '9')
            delayTime = delayTimes.punch9;

        cout << "keyDown " << delayTime << endl;
        keyDown(space, delayTime);
    }
    else if (action.find("jump") != string::npos)
    {
        if (action == "jump1")
            keyDown(up, delayTimes.jump1);

        else if (action == "jump2")
            keyDown(up, delayTimes.jump2);

        else if (action == "jump3")
            keyDown(up, delayTimes.jump3);
    } */
}
void getDelay(string action, int &delayTime)
{

    if (action == "right")
        delayTime = delayTimes.right;

    else if (action == "left")

        delayTime = delayTimes.left;

    else if (action.find("rel") != string::npos)
    {

        char check1 = action[8];
        char check2 = action[9];

        if (check1 == '1')
        {
            if (check2 == '0')
                delayTime = delayTimes.punch10;

            else
                delayTime = delayTimes.punch1;
        }
        else if (check1 == '2')
            delayTime = delayTimes.punch2;
        else if (check1 == '3')
            delayTime = delayTimes.punch3;
        else if (check1 == '4')
            delayTime = delayTimes.punch4;
        else if (check1 == '5')
            delayTime = delayTimes.punch5;
        else if (check1 == '6')
            delayTime = delayTimes.punch6;
        else if (check1 == '7')
            delayTime = delayTimes.punch7;
        else if (check1 == '8')
            delayTime = delayTimes.punch8;
        else if (check1 == '9')
            delayTime = delayTimes.punch9;
    }
    else if (action.find("punch") != string::npos)
    {
        char check1 = action[5];
        char check2 = action[6];
        if (check1 == '1')
        {
            if (check2 == '0')
                delayTime = delayTimes.punch10;
            else
                delayTime = delayTimes.punch1;
        }
        else if (check1 == '2')
            delayTime = delayTimes.punch2;
        else if (check1 == '3')
            delayTime = delayTimes.punch3;
        else if (check1 == '4')
            delayTime = delayTimes.punch4;
        else if (check1 == '5')
            delayTime = delayTimes.punch5;
        else if (check1 == '6')
            delayTime = delayTimes.punch6;
        else if (check1 == '7')
            delayTime = delayTimes.punch7;
        else if (check1 == '8')
            delayTime = delayTimes.punch8;
        else if (check1 == '9')
            delayTime = delayTimes.punch9;
    }
    else if (action.find("jump") != string::npos)
    {

        delayTime = delayTimes.jump1;

        delayTime = delayTimes.jump2;

        delayTime = delayTimes.jump3;
    }
}
void turnHandler(vector<turnHandle> *arr)
{
    for (turnHandle &t : *arr)
    {
        vector<string> moves;

        if (t.action1 != "none")
            moves.push_back(t.action1);
        if (t.action2 != "none")
            moves.push_back(t.action2);
        if (t.action3 != "none")
            moves.push_back(t.action3);

        for (int i = 0; i < t.turns; i++)
        {
            for (string &action : moves)
                doMove(action);
        }
    }
}
int main()
{
    EnumWindows(EnumWindowsProc, NULL);

    screenshot();
    setBlockSize();
    mouseClick(window.left, window.top);
    vector<turnHandle> turnArr;
    turnArr.push_back({2, "right"});
    //turnArr.push_back({97, "right", "relpunch1center"});
    turnHandler(&turnArr);

    /* int *a = getPx(0, 0);
    cout << a[0] << a[1] << a[2] << endl; */

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
