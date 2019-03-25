#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <thread>
//for time testing
#include <conio.h>
#include <math.h>

using namespace std;
//g++ bot.cpp -lgdi32
//X:\WinPython-32bit-3.4.3.5\python-3.4.3\share\mingwpy\bin\g++.exe bot.cpp -lgdi32
struct
{
    int sumPepperSeed = 100;
    int sumPepperBlock = 100;
    int platformBlock = 100;

} inventoryColorSums;
struct
{
    int top, right, bottom, left;
    int width, height;
    int deskWidth, deskHeight;
    int blockSize;
} window;

struct
{
    int *p1, *p2, *p3, *p4, *p5;

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
    string moveAction = "none";
    string punchAction = "none";
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

    /*  SelectObject(hDC, old_obj);
    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard(); */

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
    int pos = 4 * ((y * window.width) + x);
    int *arr = new int[3];

    arr[0] = ScreenData[pos + 2];
    arr[1] = ScreenData[pos + 1];
    arr[2] = ScreenData[pos];
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
    return arr[0] + arr[1] + arr[2];
}
void mouseMove(int x, int y);
int deltaE(int *a, int *b)
{
    return sqrt(a[0] - b[0]) + sqrt(a[1] - b[1]) + sqrt(a[2] - b[2]);
}

int *getPlayer(int divider = 3)
{
    screenshot();

SearchCoordinates:
    //scan for playerposition
    int bl = window.blockSize;
    int *p1 = colors.p1,
        *p2 = colors.p2,
        *p3 = colors.p3,
        *p4 = colors.p4,
        *p5 = colors.p5;
    int sum1 = getColorSum(p1),
        sum2 = getColorSum(p2),
        sum3 = getColorSum(p3),
        sum4 = getColorSum(p4),
        sum5 = getColorSum(p5);

    int pX, pY;

    int step = bl / divider;
    for (int y = step; y < window.height - step - 1; y += step)
    {
        for (int x = step; x < window.width - step - 1; x += step)
        {

            int *px = getPx(x, y);

            int sum = getColorSum(px);
            if (abs(sum - sum3) < 3 || abs(sum - sum5) < 3 || abs(sum - sum2) < 3 || abs(sum - sum1) < 3 || abs(sum - sum4) < 3)

            {

                /*  cout << "passed1" << endl;
                cout << abs(px[0] - p1[0]) << "&" << abs(px[1] - p1[1]) << "\n"
                     << abs(px[0] - p2[0]) << "&" << abs(px[1] - p2[1]) << "\n"
                     << abs(px[0] - p3[0]) << "&" << abs(px[1] - p3[1]) << "\n"
                     << abs(px[0] - p4[0]) << "&" << abs(px[1] - p4[1]) << "\n"
                     << endl; */
                if ((abs(px[0] - p1[0]) < 3 && abs(px[1] - p1[1]) < 3) ||
                    (abs(px[0] - p5[0]) < 3 && abs(px[1] - p5[1]) < 3) ||
                    (abs(px[0] - p2[0]) < 3 && abs(px[1] - p2[1]) < 3) ||
                    (abs(px[0] - p3[0]) < 3 && abs(px[1] - p3[1]) < 3) ||
                    (abs(px[0] - p4[0]) < 3 && abs(px[1] - p4[1]) < 3))
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
    int pYStart = pY - (int)(bl * 0.8);
    if (pYStart < 1)
        pYStart = 1;
    int pXStart = pX - (int)(bl * 0.7);
    if (pXStart < 1)
        pXStart = 1;
    int newX, newY;

    for (int x = pXStart; x < pX; x += 2)
    {
        for (int y = pYStart; y < pY; y += 3)
        {
            int *px = getPx(x, y);

            int sum = getColorSum(px);
            /*    cout << "first" << endl; */

            if (abs(sum - sum3) < 3 || abs(sum - sum5) < 3 || abs(sum - sum2) < 3 || abs(sum - sum1) < 3 || abs(sum - sum4) < 3)
            {
                /*  if ((px[0] == p1[0] && px[1] == p1[1]) ||
                    (px[0] == p2[0] && px[1] == p2[1]) ||
                    (px[0] == p3[0] && px[1] == p3[1]) ||
                    (px[0] == p3[0] && px[1] == p3[1]))
                { */
                if ((abs(px[0] - p1[0]) < 3 && abs(px[1] - p1[1]) < 3) ||
                    (abs(px[0] - p5[0]) < 3 && abs(px[1] - p5[1]) < 3) ||
                    (abs(px[0] - p2[0]) < 3 && abs(px[1] - p2[1]) < 3) ||
                    (abs(px[0] - p3[0]) < 3 && abs(px[1] - p3[1]) < 3) ||
                    (abs(px[0] - p4[0]) < 3 && abs(px[1] - p4[1]) < 3))
                {
                    newX = x;
                    newY = y;

                    delete[] px;
                    goto Skip2;
                }
                delete[] px;
                /*  }
                else
                    delete[] px; */
            }
            else
                delete[] px;
        }
    }
Skip2:
    int centerX, centerY = pY + (bl / 3);
    pXStart = pX + bl;
    if (pXStart > window.right)
        pXStart = window.right;
    /* int limitX = pX-10;
    if(pX<) */
    for (int x = pXStart; x > pX; x -= 2)
    {
        for (int y = pYStart; y < pY; y += 3)
        {
            int *px = getPx(x, y);

            int sum = getColorSum(px);
            /*  cout << "second" << endl; */

            if (abs(sum - sum3) < 3 || abs(sum - sum5) < 3 || abs(sum - sum2) < 3 || abs(sum - sum1) < 3 || abs(sum - sum4) < 3)
            {
                /*  if ((px[0] == p1[0] && px[1] == p1[1]) ||
                    (px[0] == p2[0] && px[1] == p2[1]) ||
                    (px[0] == p3[0] && px[1] == p3[1]) ||
                    (px[0] == p3[0] && px[1] == p3[1]))
                { */
                if ((abs(px[0] - p1[0]) < 3 && abs(px[1] - p1[1]) < 3) ||
                    (abs(px[0] - p5[0]) < 3 && abs(px[1] - p5[1]) < 3) ||
                    (abs(px[0] - p2[0]) < 3 && abs(px[1] - p2[1]) < 3) ||
                    (abs(px[0] - p3[0]) < 3 && abs(px[1] - p3[1]) < 3) ||
                    (abs(px[0] - p4[0]) < 3 && abs(px[1] - p4[1]) < 3))
                {

                    delete[] px;

                    centerX = newX + (x - newX) / 2;

                    if (y > newY)
                        pY = y;
                    // centerY = newY + bl * 0.5;
                    goto Skip3;
                }
                delete[] px;
                /*  }
                else
                    delete[] px; */
            }
            else
                delete[] px;
        }
    }
Skip3:

    pYStart = pY + bl;
    if (pYStart > window.bottom)
        pYStart = window.bottom;

    for (int y = pYStart; y > pY - bl; y -= 2)
    {

        int *px = getPx(centerX, y);

        int sum = getColorSum(px);
        /*  cout << "second" << endl; */
        /*     Sleep(1);

        mouseMove(centerX, y);
        cout << abs(px[0] - p1[0]) << "&" << abs(px[1] - p1[1]) << "\n"
             << abs(px[0] - p2[0]) << "&" << abs(px[1] - p2[1]) << "\n"
             << abs(px[0] - p3[0]) << "&" << abs(px[1] - p3[1]) << "\n"
             << abs(px[0] - p4[0]) << "&" << abs(px[1] - p4[1]) << "\n"
             << abs(px[0] - p5[0]) << "&" << abs(px[1] - p5[1]) << "\n"
             << endl; */

        if (abs(sum - sum3) < 3 || abs(sum - sum5) < 3 || abs(sum - sum2) < 3 || abs(sum - sum1) < 3 || abs(sum - sum4) < 3)
        {
            if ((abs(px[0] - p1[0]) < 3 && abs(px[1] - p1[1]) < 3) ||
                (abs(px[0] - p5[0]) < 3 && abs(px[1] - p5[1]) < 3) ||
                (abs(px[0] - p2[0]) < 3 && abs(px[1] - p2[1]) < 3) ||
                (abs(px[0] - p3[0]) < 3 && abs(px[1] - p3[1]) < 3) ||
                (abs(px[0] - p4[0]) < 3 && abs(px[1] - p4[1]) < 3))
            {
                delete[] px;

                centerY = pY + (y - pY) * 0.5 /* + (bl / 4) */;

                /*     mouseMove(centerX, pY);
                cout << "Y1" << endl;
                Sleep(3000);
                mouseMove(centerX, y);
                cout << "Y2" << endl;
                Sleep(3000);
                mouseMove(centerX, centerY);
                cout << "centerY" << endl;
                Sleep(3000); */
                break;
            }
            else
                delete[] px;
        }
        else
            delete[] px;
    }
    int *coords = new int[2];

    coords[0] = (int)centerX; //(int)topX + (bl * 0.31);
    coords[1] = (int)centerY; //(int)topY + (bl * 0.5);
                              // cout << "Player center: " << coords[0] << " " << coords[1] << endl;
    mouseMove(coords[0], coords[1]);
    return coords;
}
//sets blockWidth and gets player colors
void setBlockSize()

{
    screenshot();
    int yCoord = 0;
    int pastelBlueSum = getColorSum(colors.pastelBlue);
    int doorSum = getColorSum(colors.doorGreyBorder);

    // get Y coord by
    //  checking for not pastelBlue anymore
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
    int xForP5 = pCX;
    int yForP5 = pCY;

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
    //color5,Latzhose farbe;
    //lastcolor was p3;
    bool wasP3 = false;
    int *color3 = colors.p3;
    int sum3 = getColorSum(color3);
    int newY = (int)(yForP5 + (83.0 / 149) * blockSize);
    for (int x = xForP5; x < xForP5 + blockSize; x += 2)
    {
        int *px = getPx(x, newY);
        int sum = getColorSum(px);

        if (!wasP3 && abs(sum - sum3) < 3)
        {
            delete[] px;
            wasP3 = true;
        }

        else if (wasP3 && abs(sum - sum3) >= 3)
        {
            colors.p5 = px;
            break;
        }
        else
        {
            delete[] px;
        }
    }

    int *a = colors.p1;
    cout << "p1 " << a[0] << " " << a[1] << " " << a[2] << endl;
    a = colors.p2;
    cout << "p2 " << a[0] << " " << a[1] << " " << a[2] << endl;
    a = colors.p3;
    cout << "p3 " << a[0] << " " << a[1] << " " << a[2] << endl;
    a = colors.p4;
    cout << "p4 " << a[0] << " " << a[1] << " " << a[2] << endl;
    a = colors.p5;
    cout << "p5 " << a[0] << " " << a[1] << " " << a[2] << endl;

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
    if (action == "none")
        return;
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
    else
        delayTime = 0;
}
//punch thread ends when moving finished;
bool movingFinished;
void turnThreadX(string action, int turns, int delay, int waitTime)
{
    for (int i = 0; i < turns; i++)
    {
        cout << action << " delay " << delay << " waittime " << waitTime << endl;
        doMove(action, delay);
        Sleep(waitTime);
    }
}
void punchThread(string action, int delay)
{
    if (action == "none")
        return;
    while (!movingFinished)
        doMove(action, delay);
}

void turnHandler(vector<turnHandle> *arr)
{
    for (turnHandle &t : *arr)
    {
        //vector<string> moves;
        movingFinished = false;
        int moveDelay = 0, punchDelay = 0, delayTimeBiggest;
        int moveWait = 50, punchWait = 0;

        getDelay(t.moveAction, moveDelay);
        getDelay(t.punchAction, punchDelay);
        if (moveDelay < punchDelay)
        {
            moveWait = punchDelay - moveDelay;
        }
        thread t1(punchThread, t.punchAction, punchDelay);
        for (int i = 0; i < t.turns; i++)
        {

            doMove(t.moveAction, moveDelay);
            Sleep(moveWait * 2.5);
        }
        movingFinished = true;
        t1.join();
        /*
        if (moveDelay > delayTime2)
        {
            waitDelay2 = 100 + delayTime1 - delayTime2;
            waitDelay1 = 100;
        }
        else
        {
            waitDelay1 = 100 + delayTime2 - delayTime1;
            waitDelay2 = 100;
        }
        if (t.action1 == "none")
        {

            thread t2(turnThreadX, t.action2, t.turns, delayTime2, delayTime2 + 40);

            t2.join();
        }
        else if (t.action2 == "none")
        {

            thread t1(turnThreadX, t.action1, t.turns, delayTime1, delayTime1 + 40);

            t1.join();
        }
        else
        {
            thread t1(turnThreadX, t.action1, t.turns, delayTime1, waitDelay1);
            thread t2(turnThreadX, t.action2, t.turns, delayTime2, waitDelay2);
            t1.join();
            t2.join();
        } */

        /*   getDelay(t.action3, delayTimes[2]); */

        /*     if (t.action1 != "none")
            moves.push_back(t.action1);
        if (t.action2 != "none")
            moves.push_back(t.action2);
       
        if(t.action1.find("bottom") != string::npos)
             

        for (int i = 0; i < t.turns; i++)
        {
            Sleep(500);
            vector<thread> threads;
            for (int j = 0; j < moves.size(); j++)
            {
                Sleep(500);
                cout << "startthread " << moves[j] << endl;
                threads.push_back(thread(doMove, moves[j], delayTimes[j]));
            }
            for (int j = 0; j < moves.size(); j++)
            {
                threads[j].join();
                cout << "joinedthread " << j << endl;
            }

         
        } */
    }
}

int main()
{
    EnumWindows(EnumWindowsProc, NULL);
    mouseClick(window.left, window.top);

    keyDown(0x27, 30);
    screenshot();
    setBlockSize();
    /*  int *a1 = new int[3];
    int *a2 = new int[3];
    int *a3 = new int[3];
    int *a4 = new int[3];
    a1[0] = 125;
    a1[1] = 95;
    a1[2] = 83;
    a2[0] = 138;
    a2[1] = 106;
    a2[2] = 93;
    a3[0] = 153;
    a3[1] = 117;
    a3[2] = 102;
    a4[0] = 166; 
      a4[1] = 127;
    a4[2] = 111;
    colors.p1 = a1;
    colors.p2 = a2;
    colors.p3 = a3;
    colors.p4 = a4;
    window.blockSize = 85; */

    vector<turnHandle> turnArr = {{1, "right"}, {2, "jump2"}, {96, "right", "punch6"}};

    // turnArr.push_back({10, "relpunch1center", "relpunch1top1", "relpunch1bottom1"});

    // turnArr.push_back({96, "right", "relpunch6top2"});
    turnHandler(&turnArr);
    // getPlayer();

    /*  while (true)
    {
        char c = getch();
        if (c == 27)
            break;
        else
        { */
    /*   int howMany = 200;
            double sum = 0.0;

            for (int i = 0; i < howMany; i++)
            {
                auto start = std::chrono::system_clock::now(); */
    // Some computation here
    /* while (true)
    {
        char c = getch();
        if (c == 27)
            break;
        else
            getPlayer();
    } */
    /*   auto end = std::chrono::system_clock::now();

                std::chrono::duration<double> elapsed_seconds = end - start;
                std::time_t end_time = std::chrono::system_clock::to_time_t(end);

                sum += elapsed_seconds.count();
            }
            double time = sum / howMany;
            cout << "elapsed time: " << time << endl; */
    /*      }
    } */
    /* int *a = getPx(0, 0);
    cout << a[0] << a[1] << a[2] << endl; */

    return 0;
}