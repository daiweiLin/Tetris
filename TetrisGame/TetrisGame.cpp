//// TetrisGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
//
#include <iostream>
#include <Windows.h>
#include<thread>
#include<vector>
using namespace std;

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned int* pField = nullptr;

int nScreenWidth = 80;
int nScreenHeight = 30;

int Rotate(int x, int y, int r) 
{
    switch (r%4)
    {
    case 0: return y * 4 + x;           // 0
    case 1: return 12 + y - (x * 4);    // 90
    case 2: return 15 - (y * 4) - x;    // 180
    case 3: return 3 - y + (x * 4);     // 270
    }
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            // Index in 4*4 tetromino block
            int pi = Rotate(x, y, nRotation);
            // Index in field array
            int fi = (nPosY + y) * nFieldWidth + (nPosX + x);
            
            if (nPosX + x >= 0 && nPosY + y < nFieldHeight)
            {
                if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
                    return false; //collision
            }
        }
    }
}

int main()
{
    // Assets
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L".X..");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L"..X.");
    tetromino[1].append(L"....");

    tetromino[2].append(L"..X.");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L".X..");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"....");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"..X.");

    tetromino[5].append(L"....");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L".X..");
    tetromino[5].append(L".X..");

    tetromino[6].append(L"....");
    tetromino[6].append(L"..X.");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L"..X.");


    pField = new unsigned int[nFieldHeight * nFieldWidth];
    for (int i = 0; i < nFieldWidth; i++)
    {
        for (int j = 0; j < nFieldHeight; j++)
        {

            pField[j * nFieldWidth + i] = (i == 0 || i == nFieldWidth - 1 || j == nFieldHeight - 1) ? 9 : 0;
        }
    }
    wchar_t* screen = new wchar_t[nScreenHeight * nScreenWidth];
    for (int i = 0; i < nScreenHeight * nScreenWidth; i++) screen[i] = L' ';
    
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Game logic variables
    bool bGameOver = false;

    int nCurrentPiece = 0;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;
    int nTotalPieces = 0;

    bool bKeys[4];
    bool bRotatePressed = false;

    int nSpeed = 20; // 1 sec
    int nSpeedCounter = 0;
    bool bForceDown = false;

    int nScore = 0;

    vector<int> vLines;

    while (!bGameOver)
    {
        // Timing
        this_thread::sleep_for(50ms);
        nSpeedCounter++;
        bForceDown = nSpeedCounter == nSpeed;

        // Input
        for (int k = 0; k < 4; k++)
            bKeys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x27\x28\x20"[k]))) != 0; // Left, right, down, spacebar


        // Game logic
        if (bKeys[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX-1, nCurrentY))
                nCurrentX -= 1;
        if (bKeys[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX+1, nCurrentY))
                nCurrentX += 1;
        
        if (bKeys[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY+1))
                nCurrentY += 1;
        if (bKeys[3])
        {
            if (!bRotatePressed && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY))
                nCurrentRotation += 1;
            bRotatePressed = true;    
        }
        else
            bRotatePressed = false;

        if (bForceDown)
        {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                nCurrentY += 1;
            else
            {
                // Lock current piece into the field
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tetromino[nCurrentPiece][Rotate(px, py,nCurrentRotation)] == L'X')
                            pField[(nCurrentY + py) * nFieldWidth + nCurrentX + px] = nCurrentPiece+1;
                nTotalPieces++;
                if (nTotalPieces % 10)
                    if(nSpeed > 10) 
                        nSpeed--;

                // Detect if there is a complete line                
                for (int py = 0; py < 4; py++)
                {
                    if (nCurrentY + py < nFieldHeight - 1)
                    {
                        bool bLineComplete = true;
                        for (int px = 1; px < nFieldWidth-1; px++)
                            bLineComplete &= pField[(nCurrentY + py) * nFieldWidth + px] != 0;
                        
                        if (bLineComplete)
                        {
                            for (int px=1; px < nFieldWidth -1; px++)
                                pField[(nCurrentY + py) * nFieldWidth + px] = 8;
                            vLines.push_back(nCurrentY + py);
                        }
                    }
                }

                nScore += 10;
                if (!vLines.empty())
                    nScore += 100 * (1 << vLines.size()); // reward increases exponentially if multiple lines are completed at the same time
                // Generate a new tetromino
                nCurrentPiece = rand() % 7;
                nCurrentRotation = 0;
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;

                bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }
            nSpeedCounter = 0;
        }


        // Render Output
        // Draw field
        for (int x = 0; x < nFieldWidth; x++)
            for (int y = 0; y < nFieldHeight; y++)
                screen[(y + 2) * nScreenWidth + x + 2] = L" OOOOOOO=#"[pField[y * nFieldWidth + x]]; //" ABCDEFG=#" corresponds to "empty, piece0, 1, 2, 3, 4, 5, 6, ??, border"

        // Draw Current Piece
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
            {
                if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                    screen[(py + nCurrentY + 2) * nScreenWidth + (nCurrentX + px + 2)] = L"OOOOOOO"[nCurrentPiece];
            }
        // Draw Score
        swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

        if (!vLines.empty())
        {
            // Show complete lines
            WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
            this_thread::sleep_for(100ms);
            // Delete complete lines
            for(auto &DeleteY: vLines)
            {
                for (int px = 1; px < nFieldWidth - 1; px++)
                {
                    for (int py = DeleteY; py > 0; py--)
                        pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
                    pField[0 + px] = 0;
                }
            }
            vLines.clear();
        }
        // Display Frame
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    // Game complete
    CloseHandle(hConsole);
    cout << "Game over -- Your score:" << nScore << endl;
    system("pause");

    

    //Sleep(10000);
}
