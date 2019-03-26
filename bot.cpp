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
{ //make sure not to have too many of the same;
    float fist[3] = {220, 195, 139};
    float pepperS[3] = {71, 67, 42};
    float pepperB[3] = {105, 75, 55};
    float platformB[3] = {128, 82, 33};

} inventoryColors;
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
    int inventoryOpenBtn[3] = {124, 187, 210};
    int fourInventoryBox[3] = {164, 225, 249};
    int inventoryFist[3] = {221, 196, 141};

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
    //select inventory item;
    int select = 500;

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
void mousePress(int delay, int yOffset)
{
    int *coords = getPlayer();
    int x = (window.left + coords[0]) * (65535 / window.deskWidth);
    int y = (window.top + coords[1] + (int)(yOffset * window.blockSize)) * (65535 / window.deskHeight);

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
    int *inventoryBtn = colors.inventoryOpenBtn;
    int inventoryBtnSum = getColorSum(inventoryBtn);
    int xCenter = window.width / 2;
    for (int y = window.height / 3; y < window.height; y++)
    {
        int *px = getPx(xCenter, y);
        int sum = getColorSum(px);
        if (abs(sum - inventoryBtnSum) < 4 && abs(inventoryBtn[0] - px[0]) < 3 && abs(inventoryBtn[1] - px[1]) < 3)
        {
            int check = (366.0 / 768) * window.height;
            delete[] px;
            if (window.height - y < check)
            {
                mouseClick(xCenter, y);
                Sleep(500);
                return openInventory();
            }
            else
            {
                return y;
            }
        }
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
    WORD up = 0x26;
    WORD left = 0x25;
    WORD right = 0x27;
    WORD space = 0x20;
    if (action == "right")
        keyDown(right, delay);
    else if (action == "left")
        keyDown(left, delay);
    else if (action.find("rel") != string::npos)
    {
        int yOffset = 0;
        if (action.find("top") != string::npos)
            yOffset = -1 * ((int)action[action.length() - 1] - 48);
        else if (action.find("bottom") != string::npos)
            yOffset = (int)action[action.length() - 1] - 48;

        mousePress(delay, yOffset);
    }
    else if (action.find("punch") != string::npos)
    {

        keyDown(space, delay);
    }
    else if (action.find("jump") != string::npos)
    {
        keyDown(up, delay);
    }
    else if (action.find("select") != string::npos)
    {

        float *rgbColor;

        if (action.find("fist") != string::npos)
            rgbColor = inventoryColors.fist;
        else if (action.find("pepperB") != string::npos)
            rgbColor = inventoryColors.pepperB;
        else if (action.find("pepperS") != string::npos)
            rgbColor = inventoryColors.pepperS;
        else if (action.find("platformB") != string::npos)
            rgbColor = inventoryColors.platformB;
        cout << "selectItem" << rgbColor[0] << rgbColor[1] << endl;
        selectItem(rgbColor);
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

        delayTime = 1000;
}

//punch thread ends when moving finished;
bool movingFinished;
void punchThread(string action, int delay, int period)
{
    cout << "periodis1" << period << endl;
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
        //vector<string> moves;
        if (t.moveAction != "none")
        {
            movingFinished = false;
            int moveDelay = 0, punchDelay = 0, delayTimeBiggest;
            int moveWait = 170, punchWait = 0;
            if (!t.moveAction.find("select") != string::npos)
                getDelay(t.moveAction, moveDelay);
            //else select inventory doesnt need to be specified;

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

            cout << "delays " << moveDelay << " " << moveWait << " " << punchDelay << " " << punchWait << "\n";
            thread t1;
            if (t.punchAction != "none")
            {

                if (t.punchAction.find("rel") != string::npos)
                {
                    t1 = thread(punchThread, t.punchAction, punchDelay, (moveWait + moveDelay) / 3);
                }
                else
                {

                    t1 = thread(punchThread, t.punchAction, punchDelay, 0);
                }
            }
            for (int i = 0; i < t.turns; i++)
            {
                doMove(t.moveAction, moveDelay);
                cout << t.moveAction << endl;
                Sleep(moveWait);
            }

            if (t.punchAction != "none")
            {
                movingFinished = true;
                t1.join();
            }
        }
        else
        {
            int punchDelay;
            getDelay(t.punchAction, punchDelay);

            doMove(t.punchAction, punchDelay);
        }
    }
}

int main()
{
    EnumWindows(EnumWindowsProc, NULL);
    mouseClick(0, 0);
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

    //vector<turnHandle> turnArr = {{3, "right"}, {3, "right", "relpunch7center"}};
    vector<turnHandle> turnArr = {
        {1, "right"},
        {8, "jump2"},
        {1, "none", "relpunch8top1"},
        {1, "none", "relpunch8top2"},
        {1, "selectplatformB"},
        {1, "none", "relpunch1top1"},
        {1, "jump2"},
        {1, "selectfist"},

        //leftright
        {1, "right"},
        {1, "none", "punch8"},
        {97, "right", "punch8"},
        {1, "none", "relpunch8top1"},
        {1, "none", "relpunch8top2"},
        {1, "selectplatformB"},
        {1, "none", "relpunch1top1"},
        {1, "jump2"},
        {1, "selectfist"},
        //rightleft
        {1, "left"},
        {1, "none", "punch8"},
        {97, "left", "punch8"},
        {1, "none", "relpunch8top1"},
        {1, "none", "relpunch8top2"},
        {1, "selectplatformB"},
        {1, "none", "relpunch1top1"},
        {1, "jump2"},
        {1, "selectfist"},
        //leftright
        {1, "right"},
        {1, "none", "punch8"},
        {97, "right", "punch8"},
        {1, "none", "relpunch8top1"},
        {1, "none", "relpunch8top2"},
        {1, "selectplatformB"},
        {1, "none", "relpunch1top1"},
        {1, "jump2"},
        {1, "selectfist"},
        //rightleft
        {1, "left"},
        {1, "none", "punch8"},
        {97, "left", "punch8"},
        {1, "none", "relpunch8top1"},
        {1, "none", "relpunch8top2"},
        {1, "selectplatformB"},
        {1, "none", "relpunch1top1"},
        {1, "jump2"},
        {1, "selectfist"},
        //leftright
        {1, "right"},
        {1, "none", "punch8"},
        {97, "right", "punch8"},
        {1, "none", "relpunch8top1"},
        {1, "none", "relpunch8top2"},
        {1, "selectplatformB"},
        {1, "none", "relpunch1top1"},
        {1, "jump2"},
        {1, "selectfist"},
        //rightleft
        {1, "left"},
        {1, "none", "punch8"},
        {97, "left", "punch8"},
        {1, "none", "relpunch8top1"},
        {1, "none", "relpunch8top2"},
        {1, "selectplatformB"},
        {1, "none", "relpunch1top1"},
        {1, "jump2"},
        {1, "selectfist"}

    };
    turnHandler(&turnArr);
    // turnArr.push_back({10, "relpunch1center", "relpunch1top1", "relpunch1bottom1"});
    // turnArr.push_back({96, "right", "relpunch6top2"});
    // turnHandler(&turnArr);
    // openInventory();
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