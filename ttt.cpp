// ttt.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ttt.h"
#include "brain.h"
#define MAX_LOADSTRING 100
#define PL 10 // боковая граница
#define CS 50 // размер ячейки в пикселях

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hLog;

INT tttField[9]; // игровое поле 0 - пусто, 1 - компьютер, 2 пользователь

	

INT computer, user;
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL				DrawTicTacToeField(HDC);
			// TODO: Add any drawing code here...
Brain *myBrains;
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TTT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TTT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TTT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TTT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 320, 400, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   // загрузить мозги в память
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
HDC hMemDC;
HBITMAP hBitmap;
HWND hStart;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT wndRect;
	
	switch (message)
	{
	case WM_CREATE:
	{
		INT logW, logH, startX;
			
		GetClientRect(hWnd,&wndRect);

		logW   = wndRect.right - 2 * PL;
		logH   = wndRect.bottom - 200 - PL;
		startX = wndRect.right - PL - 100;
		// Log box
		hLog = CreateWindowEx(WS_EX_CLIENTEDGE,
			_T("EDIT"),
			_T(""),
			WS_VSCROLL| WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_READONLY,
			PL,
			200, logW, logH, hWnd,
			(HMENU)IDC_MAIN_LOG,
			GetModuleHandle(NULL),
			NULL);
		hStart = CreateWindow( 
			_T("BUTTON"),  // Predefined class; Unicode assumed 
			_T("Начать игру"),      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			startX,         // x position 
			PL,         // y position 
			100,        // Button width
			30,        // Button height
			hWnd,     // Parent window
			(HMENU)IDC_MAIN_BTN,       // No menu.
			hInst, 
			NULL);      // Pointer not needed.
		HWND hLabel = CreateWindow(
			_T("STATIC"),
			_T("Счет: 0 - 0"),
			WS_VISIBLE | WS_CHILD,
			startX,
			50,
			100,
			30,
			hWnd,
			(HMENU)IDC_MAIN_LABEL,
			hInst,
			NULL);

	    SendMessage(hLog,WM_SETTEXT,NULL,(LPARAM)_T("Добро пожаловать на игру Крестики-Нолики\r\n"));

		hMemDC = CreateCompatibleDC(GetDC(hWnd));
		hBitmap = CreateCompatibleBitmap(GetDC(hWnd),CS * 3, CS * 3);
		SelectObject(hMemDC, hBitmap);

		DrawTicTacToeField(hMemDC);
		LOGFONT lf;
		GetObject( GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf );

		_tcscpy_s(lf.lfFaceName,_T("Verdana"));
		HFONT hfDefault = CreateFontIndirect( &lf );
///////////////////////////////////////////////////////////////////
 
		SendMessage(hLog,WM_SETFONT, (WPARAM)hfDefault, (LPARAM)true); //Set font
		SendMessage(hStart,WM_SETFONT, (WPARAM)hfDefault, (LPARAM)true); //Set font
		SendMessage(hLabel,WM_SETFONT, (WPARAM)hfDefault, (LPARAM)true); //Set font
		// загрузить мозги
		myBrains = new Brain(_T("brain.bin"));
		srand((unsigned)time(NULL));
		}
		break;
	 case WM_SIZE:
		{
            INT nWidth = LOWORD(lParam);
			INT nHeight = HIWORD(lParam);
            HWND hEditBox = GetDlgItem(hWnd, IDC_MAIN_LOG);
			HWND hStatic =GetDlgItem(hWnd, IDC_MAIN_LABEL);	
			INT logWidth = nWidth - 2 * PL;
			INT logHeight = nHeight - 200 - PL;

			MoveWindow(hEditBox, PL, 200, logWidth>=0? logWidth: 0, logHeight>=0? logHeight: 0, TRUE);
			if (nWidth > 280){
				MoveWindow(hStart, nWidth-110, PL, 100, 30, TRUE);
				MoveWindow(hStatic, nWidth-110, 50, 100, 30, TRUE);
			}
				
        }
        break;
	case WM_CTLCOLORSTATIC:
        {
			HWND hItem = (HWND)lParam;
			HDC hdcEdit = (HDC) wParam;
			if (GetDlgItem(hWnd, IDC_MAIN_LOG)==hItem || GetDlgItem(hWnd, IDC_MAIN_LABEL)==hItem)
			{
				SetBkColor(hdcEdit, RGB(255,255,255));

				return (INT_PTR)GetStockObject(WHITE_BRUSH);
			}
        }
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDC_MAIN_BTN:
			{
				EnableWindow(GetDlgItem(hWnd, IDC_MAIN_BTN),FALSE);
				RtlZeroMemory(&tttField,sizeof(tttField));
				DrawTicTacToeField(hMemDC);
				InvalidateRect(hWnd, NULL, FALSE);
				int TextLen = SendMessage(hLog, WM_GETTEXTLENGTH, 0, 0);
				SendMessage(hLog, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
				SendMessage(hLog, EM_REPLACESEL, FALSE, (LPARAM)_T("Игра началась\n"));
				int who = (rand()%2);
				myBrains->InitGame(who==1, tttField);
				if (who){
					myBrains->Move();					
					DrawTicTacToeField(hMemDC);
					InvalidateRect(hWnd, NULL, FALSE);
				}
					
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:{
		
			hdc = BeginPaint(hWnd, &ps);			
			BitBlt(hdc, PL, PL, CS * 3, CS * 3, hMemDC, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if (myBrains->CanMove())
			{
				POINT p;
				RECT r;
				TCHAR *txt = _T("Следующий ход\n");

				p.x = GET_X_LPARAM(lParam);
				p.y = GET_Y_LPARAM(lParam);
			
				r.top = r.left = PL;
				r.right = r.bottom = PL+CS*3;

				if (PtInRect(&r,p)){
					int i = (p.x-PL)/CS;
					int j = (p.y-PL)/CS;
					
					if (i + j * 3 >= 0 && i + j * 3 < 9 && !tttField[i + 3 * j]){
						
						myBrains->MoveUser(i+j*3);
						
						DrawTicTacToeField(hMemDC);
						InvalidateRect(hWnd, &r, FALSE);

						int result;
						if ((result = myBrains->AnalyseGame()) == IN_PROGRESS)
						{
							//computerMove = !computerMove;
							myBrains->Move();
							
							DrawTicTacToeField(hMemDC);
							InvalidateRect(hWnd, &r, FALSE);

							result = myBrains->AnalyseGame();
						
							//computerMove = !computerMove;
						}
						if (result != IN_PROGRESS){
							EnableWindow(GetDlgItem(hWnd, IDC_MAIN_BTN), TRUE);	
							myBrains->Learn(result);
						}
						switch(result){
						case COMPUTER_WIN:
							txt = _T("Компьютер выиграл\n");
							computer++;
							break;

						case USER_WIN:
							txt = _T("Вы выиграли\n");
							user++;
							break;
						case PARITY:
							txt = _T("Ничья\n");
							break;
						
						}
						int TextLen = SendMessage(hLog, WM_GETTEXTLENGTH, 0, 0);
						SendMessage(hLog, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
						SendMessage(hLog, EM_REPLACESEL, FALSE, (LPARAM)txt);
						TCHAR txt2[100] = {0};
						_stprintf_s(txt2, 100,_T("Счет: %d - %d"), computer, user);
						SetDlgItemText(hWnd, IDC_MAIN_LABEL, txt2);
					}
					
				}
			}
		}
		break;
	/*case WM_ERASEBKGND:
		return TRUE;*/
	case WM_DESTROY:
		DeleteDC(hMemDC);
		DeleteObject(hBitmap);
		delete myBrains;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
BOOL DrawTicTacToeField(HDC hdc)
{
	// TODO: Add any drawing code here...
	HGDIOBJ original = NULL;
	HPEN tttPen = CreatePen(PS_SOLID, 3, RGB(0,0,0));

    original = SelectObject(hdc,GetStockObject(DC_PEN));
    SelectObject(hdc, GetStockObject(BLACK_PEN));
	
	Rectangle(hdc, 0,0,CS * 3, CS * 3);
	
	for(int i = 0; i < 2; i++)
	{
		MoveToEx(hdc, 0, (i + 1) * CS, NULL);
		LineTo(hdc, CS * 3, (i + 1) * CS);
		
		MoveToEx(hdc, (i + 1) * CS, 0, NULL);
		LineTo(hdc, (i + 1) * CS, CS * 3);
	}
#define PAD_CELL 5
	
	SelectObject(hdc, tttPen);
	
	for(int i = 0; i < 9; i++)
	{
		int j = i % 3;
		int k = i / 3;
		
		if (tttField[i] == 1)
		{
			MoveToEx(hdc, j * CS + PAD_CELL, k * CS + PAD_CELL, NULL);
			LineTo(hdc, (j + 1) * CS - PAD_CELL, (k + 1) * CS - PAD_CELL);

			MoveToEx(hdc, j * CS + PAD_CELL, (k + 1) * CS - PAD_CELL, NULL);
			LineTo(hdc, (j + 1) * CS - PAD_CELL, k * CS + PAD_CELL);
		}
		else if (tttField[i] == 2)
		{
			::Ellipse(hdc, j * CS + PAD_CELL, k * CS + PAD_CELL, 
				(j + 1) * CS - PAD_CELL, (k + 1) * CS-PAD_CELL);
		}
	}

	SelectObject(hdc, original);
	DeleteObject(tttPen);
	
	return TRUE;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
