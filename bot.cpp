#include <iostream>
#include <string>

#include <sstream>
#include <windows.h>
#include <vector>
#include <thread>
//for time testing
#include <conio.h>
#include <math.h>
//error handeling
#include <stdexcept>

using namespace std;
//for properties.json:
/*
   "X:/WinPython-32bit-3.4.3.5/python-3.4.3/share/mingwpy/i686-w64-mingw32/include/c++",
            "X:/WinPython-32bit-3.4.3.5/python-3.4.3/share/mingwpy/i686-w64-mingw32/include",
             "X:/WinPython-32bit-3.4.3.5/python-3.4.3/share/mingwpy/i686-w64-mingw32/include/c++/i686-w64-mingw32"
*/
//g++ bot.cpp -lgdi32
//X:\WinPython-32bit-3.4.3.5\python-3.4.3\share\mingwpy\bin\g++.exe bot.cpp -lgdi32
bool universalPause = false;

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
    //is fix because dropped items have same border, checks if (color = btn and lastcolorwasbtn1)
    int inventoryOpenBtn0[3] = {164, 225, 248};
    int inventoryOpenBtn1[3] = {130, 192, 216};
    int inventoryOpenBtn2[3] = {124, 187, 210};

    int fourInventoryBox[3] = {164, 225, 249};
    int inventoryFist[3] = {221, 196, 141};
    //floats are inventory colors
    /*  float fist[3] = {220, 195, 139};
    float pepperS[3] = {71, 67, 42};
    float pepperB[3] = {105, 75, 55};
    float platformB[3] = {128, 82, 33};
    float dirtS[3] = {128, 82, 33};
    float dirtB[3] = {128, 82, 33}; */

    //Pepper Block : {105, 75, 55};
    //Pepper Seed :{ 71, 67, 42};
    //Chandelier Block : {251, 217, 31};
    //Chandelier Seed : {172, 158, 64};
    //Growtorial Entrance Block : {212.52, 212.52, 212.52};
    //Small Lock : {169.971, 157.171, 98.4};

    float fist[3] = {220, 195, 139};
    float pepperS[3] = {172, 158, 64};
    float pepperB[3] = {251, 217, 31};

    float platformB[3] = {128, 82, 33};
    float dirtS[3] = {128, 82, 33};
    float dirtB[3] = {128, 82, 33};

} colors;

struct
{
    //  int punch[10] = {90, 220, 423, 625, 830, 1032, 1232, 2000, 2000, 2000};
    int punch[10] = {90, 220, 423, 650, 830, 1300, 1232, 1600, 2000, 2000};
    /*  int punch1 = 90, punch2 = 220, punch3 = 423,
        punch4 = 625, punch5 = 830, punch6 = 1032, punch7 = 1232, punch8 = 2000, punch9 = 2000, punch10 = 1000; */
    /*   int left = 163, right = 163; //no boots */
    int movement = 162;
    int movement2 = 185;
    int movement3 = 270;
    int movement50 = 5000;
    int movement100 = 11000;
    //turn right or left
    int turn = 40;
    int smallmovement = 40;
    /*   int jump[3] = {50, 153, 320}; */
    int jump[3] = {50, 180, 320};
    //select inventory item;
    int select = 300;
    int drop = 200;
    int respawn = 2750;

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
    // cout << "title: " << title << endl;
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
    /*  OpenClipboard(NULL);
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
int playerYCenter = 0;
int playerXCenter = 0;
int lastWasInCenter = false;
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
/* int deltaE(int *a, int *b)
{
    return sqrt(a[0] - b[0]) + sqrt(a[1] - b[1]) + sqrt(a[2] - b[2]);
} */

int *getPlayer(int divider = 3, bool isPunchCenter = false)
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
    //Skips all if player is ruffly in the horizontal middle of screen;
    if (isPunchCenter)
    {
        if (abs(window.width / 2 - pX) < bl * 1.1)
        {
            if (lastWasInCenter)
            {
                int *skipCoords = new int[2];
                skipCoords[0] = /*playerXCenter*/ window.width / 2; //(int)topX + (bl * 0.31);
                skipCoords[1] = playerYCenter;                      //(int)topY + (bl * 0.5);

                return skipCoords;
            }
            lastWasInCenter = true;
        }
        else
            lastWasInCenter = false;
    }

    int pYStart = pY - (int)(bl * 0.8);
    if (pYStart < 1)
        pYStart = 1;
    int pXStart = pX - (int)(bl * 0.7);
    if (pXStart < 1)
        pXStart = 1;
    int newX = 0, newY = 0;

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
            }

            delete[] px;
        }
    }
Skip2:
    int centerX = 0, centerY = pY + (bl / 3);
    pXStart = pX + bl;
    if (pXStart > window.right)
        pXStart = window.right;

    for (int x = pXStart; x > pX; x -= 2)
    {
        for (int y = pYStart; y < pY; y += 3)
        {
            int *px = getPx(x, y);
            int sum = getColorSum(px);

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
            }

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

                break;
            }
        }

        delete[] px;
    }

    int *coords = new int[2];
    coords[0] = centerX; //(int)topX + (bl * 0.31);
    coords[1] = centerY; //(int)topY + (bl * 0.5);
    playerYCenter = centerY;
    playerXCenter = centerX;

    // cout << "Player center: " << coords[0] << " " << coords[1] << endl;
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
    x = x + window.left;
    y = y + window.top;
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
void mousePress(int delay, int yOffset, int xOffset = 0)
{
    int *coords;
    if (yOffset == 0)
    {
        //is relpunch1center dont need to check delay
        coords = getPlayer(4, true);
    }
    else
    {
        coords = getPlayer();
    }
    int x = (window.left + coords[0] + xOffset * window.blockSize) * (65535 / window.deskWidth);
    int y = (window.top + coords[1] + yOffset * window.blockSize) * (65535 / window.deskHeight);
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
int openInventory()
{

    screenshot();
    int *inventoryBtn0 = colors.inventoryOpenBtn0;
    int inventoryBtnSum0 = getColorSum(inventoryBtn0);
    int *inventoryBtn1 = colors.inventoryOpenBtn1;
    int inventoryBtnSum1 = getColorSum(inventoryBtn1);
    int *inventoryBtn2 = colors.inventoryOpenBtn2;
    int inventoryBtnSum2 = getColorSum(inventoryBtn2);

    int xCenter = window.width / 2;
    bool lastColorWas0 = false;
    bool lastColorWas1 = false;

    for (int y = window.height / 3; y < window.height; y++)
    {

        int *px = getPx(xCenter, y);
        int sum = getColorSum(px);

        if (abs(sum - inventoryBtnSum0) < 4 && abs(inventoryBtn0[0] - px[0]) < 3 && abs(inventoryBtn0[1] - px[1]) < 3)
        {
            lastColorWas0 = true;
        }
        else if (lastColorWas0 && (abs(sum - inventoryBtnSum1) < 4 && abs(inventoryBtn1[0] - px[0]) < 3 && abs(inventoryBtn1[1] - px[1]) < 3))
        {
            lastColorWas1 = true;
        }
        else if (lastColorWas0 && lastColorWas1 && (abs(sum - inventoryBtnSum2) < 4 && abs(inventoryBtn2[0] - px[0]) < 3 && abs(inventoryBtn2[1] - px[1]) < 3))

        {

            int check = (366.0 / 768) * window.height;
            delete[] px;
            if (window.height - y < check)
            {
                mouseClick(xCenter, y);
                Sleep(400);
                return openInventory();
            }
            else
            {
                return y;
            }
        }
        else if (!lastColorWas1)
            lastColorWas0 = false;

        delete[] px;
    }
}
int getMenuSquareSize(int y)
{
    int *fourInventoryBox = colors.fourInventoryBox;
    int fourInventoryBoxSum = getColorSum(fourInventoryBox);
    int firstX, secondX;
    bool firstFound = false;
    bool lastHadColor = false;
    for (int x = window.width / 4; x < window.width - window.width / 4; x++)
    {
        // cout << "b" << endl;
        int *px = getPx(x, y);
        int sum = getColorSum(px);
        if (abs(sum - fourInventoryBoxSum) < 4 && abs(fourInventoryBox[0] - px[0]) < 3 && abs(fourInventoryBox[1] - px[1]) < 3)
        {
            if (!lastHadColor && firstFound)
            {
                secondX = x;
                break;
            }
            lastHadColor = true;
        }
        else if (lastHadColor && !firstFound)
        {
            firstFound = true;
            firstX = x;
            lastHadColor = false;
        }
        else
        {
            lastHadColor = false;
        }
        delete[] px;
    }
    return ((secondX - firstX) / 4) * 0.98;
}
int *getFistCenter(int yStart, int squareSize)
{
    int *fist = colors.inventoryFist;
    int fistSum = getColorSum(fist);
    int xy[2];
    for (int x = 0 / 4; x < window.width / 7; x += 3)
    {
        for (int y = yStart; y < yStart + squareSize; y += 3)
        {
            int *px = getPx(x, y);
            int sum = getColorSum(px);
            if (abs(sum - fistSum) < 4 && abs(fist[0] - px[0]) < 3 && abs(fist[1] - px[1]) < 3)
            {
                xy[0] = x;
                xy[1] = y;
                delete[] px;
                goto SkipGetFist1;
            }
            delete[] px;
        }
    }
SkipGetFist1:
    int cX1, cY1;
    int *coords = new int[2];
    for (int y = xy[1]; y > xy[1] - squareSize; y--)
    {
        int *px = getPx(xy[0], y);
        //if black
        if (px[0] == 0 && px[1] == 0)
        {
            cY1 = y;
            delete[] px;
            break;
        }
        delete[] px;
    }
    for (int y = xy[1]; y < xy[1] + squareSize; y++)
    {
        int *px = getPx(xy[0], y);
        //if black
        if (px[0] == 0 && px[1] == 0)
        {
            coords[1] = cY1 + (y - cY1) / 2;
            delete[] px;
            break;
        }
        delete[] px;
    }
    for (int x = xy[0]; x > xy[0] - squareSize; x--)
    {
        int *px = getPx(x, xy[1]);
        //if black
        if (px[0] == 0 && px[1] == 0)
        {
            cX1 = x;
            delete[] px;
            break;
        }
        delete[] px;
    }
    for (int x = xy[0]; x < xy[0] + squareSize; x++)
    {
        int *px = getPx(x, xy[1]);
        //if black
        if (px[0] == 0 && px[1] == 0)
        {
            coords[0] = cX1 + (x - cX1) / 2;
            delete[] px;
            break;
        }
        delete[] px;
    }
    return coords;
}

typedef struct
{
    double r; // a fraction between 0 and 1
    double g; // a fraction between 0 and 1
    double b; // a fraction between 0 and 1
} rgb;

typedef struct
{
    double h; // angle in degrees
    double s; // a fraction between 0 and 1
    double v; // a fraction between 0 and 1
} hsv;

static hsv rgb2hsv(rgb in);
static rgb hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
    hsv out;
    double min, max, delta;
    min = in.r < in.g ? in.r : in.g;
    min = min < in.b ? min : in.b;
    max = in.r > in.g ? in.r : in.g;
    max = max > in.b ? max : in.b;
    out.v = max; // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if (max > 0.0)
    {                          // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max); // s
    }
    else
    {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN; // its now undefined
        return out;
    }
    if (in.r >= max)                   // > is bogus, just keeps compilor happy
        out.h = (in.g - in.b) / delta; // between yellow & magenta
    else if (in.g >= max)
        out.h = 2.0 + (in.b - in.r) / delta; // between cyan & yellow
    else
        out.h = 4.0 + (in.r - in.g) / delta; // between magenta & cyan
    out.h *= 60.0;                           // degrees
    if (out.h < 0.0)
        out.h += 360.0;
    return out;
}
void getDelay(string action, int &delay)
{
    if (action == "right" || action == "left")
        delay = delayTimes.movement;
    else if (action == "right2" || action == "left2")
        delay = delayTimes.movement2;
    else if (action == "right3" || action == "left3")
        delay = delayTimes.movement3;
    else if (action == "right50" || action == "left50")
        delay = delayTimes.movement50;
    else if (action == "right100" || action == "left100")
        delay = delayTimes.movement100;
    else if (action == "turnRight" || action == "turnLeft")
        delay = delayTimes.turn;
    else if (action.find("punch") != string::npos)
    {
        //rel and normal punch
        string delayIndex = "";
        for (int i = 0; i < action.length(); i++)
        {
            if (isdigit(action[i]))
            {
                for (int j = i; j < action.length(); j++)
                {
                    if (isdigit(action[j]))
                    {
                        delayIndex += action[j];
                    }
                    else
                        goto SkipGetDelayPunch;
                }
            }
        }
    SkipGetDelayPunch:
        delay = delayTimes.punch[stoi(delayIndex) - 1];
    }
    else if (action.find("jump") != string::npos)
    {
        string delayIndex = "";
        for (int i = 0; i < action.length(); i++)
        {
            if (isdigit(action[i]))
            {
                for (int j = i; j < action.length(); j++)
                {
                    if (isdigit(action[j]))
                    {
                        delayIndex += action[j];
                    }
                    else
                        goto SkipGetDelayJump;
                }
            }
        }
    SkipGetDelayJump:

        delay = delayTimes.jump[stoi(delayIndex) - 1];
    }
    else if (action == "adjust")
    {
        delay = delayTimes.smallmovement;
    }
    else if (action.find("select") != string::npos)
    {
        delay = delayTimes.select;
    }
    else if (action.find("drop") != string::npos)
    {
        delay = delayTimes.drop;
    }
    else if (action == "respawn")
    {
        delay = delayTimes.respawn;
    }
    else
        delay = 500;
    cout << "delay" << delay << endl;
}
void dropItem()
{
    WORD enter = 0x0D;
    openInventory();
    mouseClick(935, 615);
    Sleep(1200);
    keyDown(enter, 0);
    Sleep(400);
    /*    mouseMove(935, 615); */
}
void selectItem(float checkColorSum[3])
{
    int inventoryY = openInventory();
    Sleep(500);
    int squareSize = getMenuSquareSize(inventoryY + 65);
    cout
        << "squareSize: " << squareSize << endl;
    int *fistCenter = getFistCenter(inventoryY + squareSize * 1.1, squareSize);
    //runs through the boxes and checks, only for the first 3 columns;
    //checks 8x8 colorsum around each center
    int coords[2] = {0, 0};
    int tolerance = 5;
    cout << "checksum  rgb " << checkColorSum[0] << " "
         << checkColorSum[1] << " "
         << checkColorSum[2] << endl;
RepeatSelectItem:
    for (int xO = fistCenter[0]; xO < fistCenter[0] + squareSize * 10; xO += squareSize)
    {
        for (int yO = fistCenter[1]; yO < fistCenter[1] + squareSize * 3; yO += squareSize)
        {
            float pxCount = 0;
            float rsum = 0, gsum = 0, bsum = 0;
            /*  mouseMove(xO, yO);
            Sleep(100); */
            for (int x = xO - 10; x < xO + 10; x += 1)
            {
                for (int y = yO - 10; y < yO + 10; y += 1)
                {
                    int *px = getPx(x, y);
                    int sum = getColorSum(px);
                    hsv hsvColor = rgb2hsv({px[0] / 255.0, px[1] / 255.0, px[2] / 255.0});
                    int h, s, v;
                    h = hsvColor.h;
                    s = hsvColor.s * 255;
                    v = hsvColor.v * 255;
                    if (!(abs(h - 150) < 60 && abs(s - 100) < 80 && abs(v - 85) < 30))
                    {
                        if (sum != 0 && sum != 765)
                        {
                            pxCount++;
                            rsum += px[0];
                            gsum += px[1];
                            bsum += px[2];
                        }
                    }
                    delete[] px;
                }
            }
            rsum = rsum / pxCount;
            bsum = bsum / pxCount;
            gsum = gsum / pxCount;
            cout << "At r:" << 1 + (xO - fistCenter[0]) / squareSize << " c:" << 1 + (yO - fistCenter[1]) / squareSize;
            cout << "  rgb " << rsum << " "
                 << gsum << " "
                 << bsum << " total: " << rsum + bsum + gsum << endl;
            if (abs(rsum - checkColorSum[0]) < tolerance && abs(gsum - checkColorSum[1]) < tolerance && abs(bsum - checkColorSum[2]) < tolerance)
            {
                coords[0] = xO;
                coords[1] = yO;
                goto SkipSelectItem1;
            }
        }
    }
    tolerance += 1;
    if (tolerance < 20)
        goto RepeatSelectItem;
SkipSelectItem1:
    mouseClick(coords[0], coords[1]);
    return;
}

void doMove(string action, int delay)
{

    if (action == "none")
        return;
    cout << "Action: " << action << endl;
    WORD up = 0x26;
    WORD left = 0x25;
    WORD right = 0x27;
    WORD space = 0x20;
    WORD enter = 0x0D;
    if (action == "right" || action == "right2" || action == "right3" || action == "turnRight" || action == "right50" || action == "right100")
        keyDown(right, delay);
    else if (action == "left" || action == "left2" || action == "left3" || action == "turnLeft" || action == "left50" || action == "left100")
        keyDown(left, delay);
    else if (action.find("rel") != string::npos)
    {
        int yOffset = 0;
        int xOffset = 0;
        if (action.find("top") != string::npos)
            yOffset = -1 * (action[action.length() - 1] - 48);
        else if (action.find("bottom") != string::npos)
            yOffset = action[action.length() - 1] - 48;
        else if (action.find("left") != string::npos)
            xOffset = -1 * (action[action.length() - 1] - 48);

        else if (action.find("right") != string::npos)
            xOffset = action[action.length() - 1] - 48;

        mousePress(delay, yOffset, xOffset);
    }
    else if (action.find("punch") != string::npos)

        keyDown(space, delay);

    else if (action.find("jump") != string::npos)
        keyDown(up, delay);

    else if (action.find("select") != string::npos)
    {
        float *rgbColor;
        if (action.find("fist") != string::npos)
            rgbColor = colors.fist;
        else if (action.find("pepperB") != string::npos)
            rgbColor = colors.pepperB;
        else if (action.find("pepperS") != string::npos)
            rgbColor = colors.pepperS;
        else if (action.find("platformB") != string::npos)
            rgbColor = colors.platformB;
        else if (action.find("dirtB") != string::npos)
            rgbColor = colors.dirtB;
        else if (action.find("dirtS") != string::npos)
            rgbColor = colors.dirtS;
        selectItem(rgbColor);
    }
    else if (action == "adjust")
    {

        int *coords = getPlayer();
        int horizontal = coords[0];
        delete[] coords;
        int delay, sleepdelay;
        getDelay("adjust", delay);

        int checkdistance = window.width / 2 < horizontal ? window.width - window.blockSize * 1.45 : window.blockSize * 1.5;
        bool firstTime = true;
        /*      char llOne = 'l', lOne = 'l'; */
        sleepdelay = delay * 2;

        while (!(abs(checkdistance - horizontal) < 5) /*  && llOne == lOne */)
        {
            /*  llOne = lOne; */
            cout << checkdistance << "checkdistacne hrizontal: " << horizontal << " width" << window.width << " blsize" << window.blockSize << endl;
            if (checkdistance > horizontal)
            {
                doMove("right", delay);
                //cout << "right" << endl;
                /*  lOne = 'r'; */
            }
            else
            {
                doMove("left", delay);
                //  cout << "left" << endl;
                /*  lOne = 'l'; */
            }
            Sleep(sleepdelay);
            coords = getPlayer();

            horizontal = coords[0];
            checkdistance = window.width / 2 < horizontal ? window.width - window.blockSize * 1.45 : window.blockSize * 1.5;
            /*   if (firstTime)
                llOne = lOne; */

            delete[] coords;
        }
    }
    else if (action == "respawn")
    {
        // cout << "respawn" << endl;

        mouseClick(980, 40);
        Sleep(500);
        mouseClick(512, 219);
        Sleep(delay);
    }
    else if (action == "drop")
        dropItem();
}

//punch thread ends when moving finished;
bool movingFinished;
void punchThread(string action, int delay, int period)
{
    //test relpunch with no period;
    period = 0;
    if (period == 0)
    {
        while (!movingFinished)
            doMove(action, delay);
    }
    else
    {
        while (!movingFinished)
        {
            doMove(action, delay);
            Sleep(period);
        }
    }
}

void turnHandler(vector<turnHandle> *arr)
{
    for (turnHandle &t : *arr)
    {
        if (t.moveAction != "none")
        {
            movingFinished = false;
            int moveDelay = 0, punchDelay = 0, delayTimeBiggest;
            int moveWait = 170, punchWait = 0;
            getDelay(t.moveAction, moveDelay);
            thread t1;
            if (t.punchAction != "none")
            {
                getDelay(t.punchAction, punchDelay);
                if (moveDelay < punchDelay - moveWait)
                {
                    moveWait = punchDelay + 50 - moveDelay;
                }
            }
            else
            {
                if (t.moveAction.find("jump") != string::npos)
                {
                    moveWait = moveDelay;
                }
            }
            if (t.punchAction != "none")
            {
                //punchaction
                if (t.punchAction.find("rel") != string::npos)
                {
                    t1 = thread(punchThread, t.punchAction, punchDelay, (moveWait + moveDelay) / 3);
                }
                else
                {
                    t1 = thread(punchThread, t.punchAction, punchDelay, 0);
                }
            }

            //moveaction
            for (int i = 0; i < t.turns; i++)
            {
                //cout << t.moveAction << endl;
                doMove(t.moveAction, moveDelay);

                Sleep(moveWait);
            }
            if (t.punchAction != "none")
            {
                movingFinished = true;
                t1.join();
            }
        }
        else
        { //if no moveaction but punchaction
            int punchDelay;
            getDelay(t.punchAction, punchDelay);
            for (int i = 0; i < t.turns; i++)
            {
                doMove(t.punchAction, punchDelay);
            }
        }
    }
}
vector<turnHandle> parseToTurnHandle(string &s)
{
    string selectSeedAndBlockName = "pepper";
    vector<string> vTurns;
    string delimiter = ",";
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos)
    {
        token = s.substr(0, pos);
        vTurns.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    //for the last
    vTurns.push_back(s);
    vector<turnHandle> shorthandTurnHandle;
    for (string &t : vTurns)
    {
        string delimiter = " ";
        size_t pos = 0;
        string token;
        int turnCount = 1;
        string tmaction = "none", tpaction = "none";
        //if string looks like "p7";
        //while loop wont trigger, only the following if statement
        //so no need to check in while loop if punch or moveaction
        while ((pos = t.find(delimiter)) != string::npos)
        {
            token = t.substr(0, pos);
            bool isNum = true;
            bool hasDigit = false;
            for (int i = 0; i < token.size(); i++)
            {
                if (!isdigit(token[i]))
                    isNum = false;
                else
                    hasDigit = true;
            }
            if (isNum)
            {

                turnCount = stoi(token);
            }
            else
            {

                tmaction = token;
            }
            t.erase(0, pos + delimiter.length());
        }
        if (tmaction == "none" || tpaction == "none")
        {
            if (((t[0] == 'r' && t[1] == 'p') || t[0] == 'p'))
                tpaction = t;
            else
                tmaction = t;
        }

        shorthandTurnHandle.push_back({turnCount, tmaction, tpaction});
    }
    vector<turnHandle> finalVector;
    //rp1t2 = relativepunch1top2;
    //r = right,l = left; ss select seed; sb selectblock;
    //j1;
    bool lastWasPunchAction = false;
    for (turnHandle &t : shorthandTurnHandle)
    {

        string moveAction = "none", punchAction = "none";
        int mAFirstIndex = 0, mASecondIndex = 0;

        if (t.moveAction[0] == 'j')
        {
            //jump
            moveAction = "jump";
            for (int i = 0; i < t.moveAction.length(); i++)
            {
                if (isdigit(t.moveAction[i]))
                {
                    moveAction += t.moveAction[i];
                }
            }
        }
        else if (t.moveAction == "r")
            moveAction = "right";
        else if (t.moveAction == "r2")
            moveAction = "right2";
        else if (t.moveAction == "r3")
            moveAction = "right3";
        else if (t.moveAction == "r50")
            moveAction = "right50";
        else if (t.moveAction == "r100")
            moveAction = "right100";

        else if (t.moveAction == "l")
            moveAction = "left";
        else if (t.moveAction == "l2")
            moveAction = "left2";

        else if (t.moveAction == "l3")
            moveAction = "left3";
        else if (t.moveAction == "l50")
            moveAction = "left50";
        else if (t.moveAction == "l100")
            moveAction = "left100";

        else if (t.moveAction == "tr")
            moveAction = "turnRight";
        else if (t.moveAction == "tl")
            moveAction = "turnLeft";
        else if (t.moveAction == "ss")

            moveAction = "select" + selectSeedAndBlockName + "S";
        else if (t.moveAction == "sb")
            moveAction = "select" + selectSeedAndBlockName + "B";
        else if (t.moveAction == "sds")
            moveAction = "selectdirtS";
        else if (t.moveAction == "sdb")
            moveAction = "selectdirtB";
        else if (t.moveAction == "spb")
            moveAction = "selectplatformB";
        else if (t.moveAction == "sf")
            moveAction = "selectfist";
        else if (t.moveAction == "drp")
            moveAction = "drop";
        else if (t.moveAction == "adj")
            moveAction = "adjust";
        else if (t.moveAction == "rspwn")
            moveAction = "respawn";

        //punchAction
        if (t.punchAction[0] == 'r')
        {
            //relpunch
            string punchCount = "";
            string relSpecific = "";
            string topbtmcenter;
            bool isPunchAction = true;
            for (int i = 0; i < t.punchAction.length(); i++)
            {
                if (isdigit(t.punchAction[i]))
                {
                    for (int j = i; j < t.punchAction.length(); j++)
                    {
                        if (isdigit(t.punchAction[j]))
                        {
                            if (isPunchAction)
                                punchCount += t.punchAction[j];
                            else
                                relSpecific += t.punchAction[j];
                        }
                        else
                        {
                            i = j;
                            break;
                        }
                    }
                    isPunchAction = false;
                }
            }
            if (t.punchAction.find("t") != string::npos)
            {
                topbtmcenter = "top";
            }
            else if (t.punchAction.find("b") != string::npos)
            {
                topbtmcenter = "bottom";
            }
            else if (t.punchAction.find("c") != string::npos)
                topbtmcenter = "center";
            else if (t.punchAction.find("l") != string::npos)
                topbtmcenter = "left";
            else
                topbtmcenter = "right";

            /*   cout << "center"
                 << " " << punchCount << " " << relSpecific << endl;*/
            punchAction = "relpunch" + punchCount + topbtmcenter + relSpecific;
            lastWasPunchAction = false;
        }
        else if (t.punchAction[0] == 'p')
        {
            //punch
            punchAction = "punch";
            for (int i = 0; i < t.punchAction.length(); i++)
            {
                if (isdigit(t.punchAction[i]))
                {
                    punchAction += t.punchAction[i];
                }
            }
            //selectfist before punching
            if (!lastWasPunchAction)
                finalVector.push_back({1, "selectfist"});
            lastWasPunchAction = true;
        }
        else
            lastWasPunchAction = false;
        finalVector.push_back({t.turns, moveAction, punchAction});
    }
    cout << finalVector[0].moveAction << endl;
    return finalVector;
}
string rowTurnMaker(int rows)
{
    //harvest is same as break but onepunch
    string jump = ",adj,j2";

    string harvestRowRight = ",tr,p1,97 r p1" + jump;
    string harvestRowLeft = ",tl,p1,97 l p1" + jump;
    string harvest = "r,j2";
    for (int i = 0; i < rows; i++)
    {
        if (i % 2 == 0)
            harvest += harvestRowRight;
        else
            harvestRowLeft;
    }

    //collect
    //rows == respawns
    string drop = ",rspwn,4 r,sb,drp,4 l,r";
    string collectFirstHalf = ",17 r3";
    string collectSecondHalf = ",33 r3";
    string collect = drop;
    for (int i = 1; i < rows + 1; i++)
    {
        string jumpString = ",adj," + to_string(i) + " j2";
        collect += jumpString + collectFirstHalf + drop + jumpString + collectSecondHalf;
    }

    //place + break + dropseed
    int blocksInTotal = rows * 3.75;
    //to break
    int howManyRows = 1 + ((blocksInTotal - 1) / 100);

    /* int howManyTimes26Rows = 1 + ((howManyRows - 1) / 26);
    string place = "";
    int currentRows = 0;
    for (int rows26 = 0; rows26 < howManyTimes26Rows; rows26++)
    {
        if (rows26 == 0)
            currentRows =
                for (int i = 0; i < ; i++)
            {
                if (i % 2 == 0)
                    harvest += harvestRowRight;
                else
                    harvestRowLeft;
            }
    } */

    //break + drop seed left;
    //plant
};
int main()
{
    EnumWindows(EnumWindowsProc, NULL);
    mouseClick(0, 0);
    keyDown(0x27, 30);
    screenshot();
    setBlockSize();

    string bRowL = ",tl,p1,l100 p1";
    string bRowR = ",tr,p1,r100 p1";
    string jmp = ",adj,j2";
    string breakTotal = ",rspwn,r,j2" +
                        bRowR + ",rspwn,r,j2" + bRowR + ",rspwn,r,2 j2" +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp /* +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp +
                        bRowR + bRowL + jmp */
        ;

    string drop2 = ",rspwn,3 r,sb,drp,3 l,r";
    string drop = ",rspwn,4 r,sb,drp,4 l,r";
    string cF = ",r50 p1";
    string cS = ",r100 p1";
    string collect = drop + ",j2" + cF + drop + ",j2" + cS + drop +
                     ",2 j2" + cF + drop + ",2 j2" + cS + drop +
                     ",3 j2" + cF + drop + ",3 j2" + cS + drop +
                     ",4 j2" + cF + drop + ",4 j2" + cS + drop +
                     ",5 j2" + cF + drop + ",5 j2" + cS + drop +
                     ",6 j2" + cF + drop + ",6 j2" + cS + drop +
                     ",7 j2" + cF + drop + ",7 j2" + cS + drop +
                     ",8 j2" + cF + drop + ",8 j2" + cS + drop +
                     ",9 j2" + cF + drop + ",9 j2" + cS + drop +
                     ",10 j2" + cF + drop + ",10 j2" + cS + drop +
                     ",11 j2" + cF + drop2 + ",11 j2" + cS + drop2 +
                     ",12 j2" + cF + drop2 + ",12 j2" + cS + drop2 +
                     ",13 j2" + cF + drop2 + ",13 j2" + cS + drop2 +
                     ",14 j2" + cF + drop2 + ",14 j2" + cS + drop2 +
                     ",15 j2" + cF + drop2 + ",15 j2" + cS + drop2 /* +
                     ",16 j2" + cF + drop2 + ",16 j2" + cS + drop2 +
                     ",17 j2" + cF + drop2 + ",17 j2" + cS + drop2 +
                     ",18 j2" + cF + drop2 + ",18 j2" + cS + drop2 +
                     ",19 j2" + cF + drop2 + ",19 j2" + cS + drop2 +
                     ",20 j2" + cF + drop2 + ",20 j2" + cS + drop2 +
                     ",21 j2" + cF + drop2 + ",21 j2" + cS + drop2 +
                     ",22 j2" + cF + drop2 + ",22 j2" + cS + drop2 */
        ;

    string pRL = ",sb,33 r3 rp1c1,j2,33 l3 rp1c1,rspwn,5 r,5 l,r,adj";
    //l because left side menus make accidental click easy
    string place = ",rspwn,4 r,4 l,r,adj,j2" + pRL +
                   /*      ",3 j2" + pRL +
                   ",5 j2" + pRL +
                   ",7 j2" + pRL +
                   ",9 j2" + pRL +
                   ",11 j2" + pRL +
                   ",13 j2" + pRL + */
                   ",15 j2" + pRL +
                   ",17 j2" + pRL +
                   ",19 j2" + pRL +
                   ",21 j2" + pRL +
                   ",23 j2" + pRL +
                   ",25 j2" + ",sb,33 r3 rp1c1,l";

    ;
    string bR = ",tr,2 p6,97 r p6,l" + jmp;
    string bL = ",tl,2 p6,97 l p6,ss,adj,tl,drp" + jmp;
    string breakBlock = ",rspwn,r,j2" +
                        bR + bL +
                        bR + bL +
                        bR + bL +
                        bR + bL +
                        bR + bL +
                        bR + bL +
                        bR + bL +
                        bR + bL +
                        bR + bL +
                        bR + bL +
                        bR + bL +
                        bR + bL +
                        bR;
    string cWWRight = ",tr,p8,97 r p8,adj,rp10t1,rp10t2,spb,rp1t1,j2";
    string cWWLeft = ",tl,p8,97 l p8,adj,rp10t1,rp10t2,spb,rp1t1,j2";
    string createWorld = "r,3 j2" + cWWRight + cWWLeft + cWWRight + cWWLeft + cWWRight + cWWLeft + cWWRight + cWWLeft + cWWRight + cWWLeft + cWWRight + cWWLeft;

    //whitedoorscript
    //onecicle
    string onecicle = "ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp,ss,rp8t2,rp6t2,sf,rp8t2,sb,drp";
    /*   string fourcicle = onecicle + onecicle + onecicle + onecicle;
    string twentycicle = fourcicle + fourcicle + fourcicle + fourcicle + fourcicle;
    string fcycle = twentycicle + twentycicle; */
    string whitedoorscript = "ss,sb";
    string abc = "10 r,ss,rp8r2,rp6r2,sf,rp10r2,sb,drp";
    string ab = ",ss,rp8r2,rp6r2,sf,rp10r2,sb,drp";
    string finalstrings = ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab + ab;
    string finalfinal = "51 r" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l" + finalstrings + ",l";

    /* vector<turnHandle> turnArr = parseToTurnHandle(finalfinal);
    turnHandler(&turnArr); */
    /*     string abc = "92 r";
    vector<turnHandle>
        turnArr = parseToTurnHandle(abc);
    turnHandler(&turnArr);
    for (int j = 0; j < 96; j++)
    {
        string abc = "l";
        vector<turnHandle>
            turnArr = parseToTurnHandle(abc);
        turnHandler(&turnArr);
        for (int i = 0; i < 40; i++)
        {

            Sleep(2000);
            string whitedoor = "ss,rp10t2,rp6t2,sf,rp8t2,sb,tl,tr,drp";
            vector<turnHandle>
                turnArr = parseToTurnHandle(whitedoor);
            turnHandler(&turnArr);
        }
    } */
    string cleanr = ",adj,rp8t1,rp8t2,spb,rp1t1,j2,tr,p8,97 r p8";
    string cleanl = ",adj,rp8t1,rp8t2,spb,rp1t1,j2,tl,p8,97 l p8";
    string cleanrl = cleanr + cleanl;
    string cleanWorld = "r,2 j2" + cleanrl + cleanrl + cleanrl + cleanrl + cleanrl + cleanrl + cleanrl + cleanrl;

    string s = cleanWorld /* breakTotal +   collect */ /*createWorld "r,j2"  + breakTotal  + collect + + place + breakBlock*/;

    //lock is pepperS
    //whitedoor is pepperB

    vector<turnHandle>
        turnArr = parseToTurnHandle(s);

    turnHandler(&turnArr);

    return 0;
}