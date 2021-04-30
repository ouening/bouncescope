// Main program for Bouncescope dev. version 2
// This version makes use of the new, correct formulas for collisions.
// This file contains all the Windows-related functions, such as creating the
// window and actually drawing the balls.
// Copyright 2006 Chad Berchek

#include "bsrc.h"
#include "ball.h"
#include "walls.h"
#include "ballssim.h"
#include <windows.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <time.h>
using namespace std;

// CONSTANTS
const UINT FRAME_DT = 10; // Frame duration in milliseconds
const DWORD OVERFLOW_THRESHOLD = 1000; // Threshold for detecting DWORD subtraction overflow (i.e., 1 - 1000 = 4294966297). Must be greater than FRAME_DT
const unsigned int MAX_NUM_BALLS = 60000; // Maximum number of balls in the simulator
const unsigned char LIGHT_COLOR_THRESHOLD = 0xE0; // If the R, G, and B components of color for a ball are greater than this, issue a notice
const int GET_INPUT_BUFFER_LEN = 100; // Length of buffer for reading text from edit controls
const double M_PI = 3.141592653589;
const double MAX_VX = 3000; // Maximum horizontal velocity that can be entered
const double MIN_VX = 0;    // Minimum horizontal velocity that can be entered
const double DEF_VX = 100;   // Default horizontal velocity
const double MIN_VY = 0;    // Minimum vertical velocity that can be entered
const double MAX_VY = 3000; // Maximum vertical velocity that can be entered
const double DEF_VY = 100;   // Default vertical velocity
const double MIN_MASS = 1;  // Minimum mass that can be entered
const double MAX_MASS = 1000; // Maximum mass that can be entered
const double DEF_MASS = 10;   // Default mass
const double MIN_DIAMETER = 2; // Minimum diameter that can be entered
const double MAX_DIAMETER = 500; // Maximum diameter that can be entered
const double DEF_DIAMETER = 20;  // Default diameter
const double MIN_RANDOM_V = 0; // Minimum velocity (x or y) to be used in generating random balls
const double MAX_RANDOM_V = 200; // Maximum velocity (x or y) to be used in generating random balls
const double MIN_RANDOM_R = 5; // Minimum radius to be used in generating random balls
const double MAX_RANDOM_R = 20; // Maximum radius to be used in generating random balls
const double M_TO_A_RATIO = .1; // Ratio of mass to area used in generating random balls
const UINT WMU_UPDATESIM = WM_USER + 0; // Message meaning the simulator should be updated by calling g_bsim.advanceSim()
const UINT WMU_PAUSESIM = WM_USER + 1; // Message meaning pause the simulation
const UINT WMU_RESUMESIM = WM_USER + 2; // Message meaning resume the simulation


// GLOBALS
BallsSim g_bsim = BallsSim(); // Note: it is absolutely vital to say = BallsSim() because otherwise
										// the constructor will not be called.
Ball g_bAdd = Ball(); // Stores settings of last ball added from "Add a ball" dialog box
										
// Set up defaults for ball to be added
void initAddBall() {
	g_bAdd.setVXY(DEF_VX, DEF_VY);
	g_bAdd.setM(DEF_MASS);
	g_bAdd.setR(DEF_DIAMETER / 2.); // Convert diameter to radius
	// Color is already black
}

// hdc = DC to draw on
// x, y = coordinates of circle center
// r = radius
// color = fairly self explanatory
void drawSolidCircle(const HDC hdc, const double x, const double y, const double r, const COLORREF color) {
	HPEN hPen = CreatePen(PS_SOLID, 0, color); // Create pen
	HPEN hPenOld = (HPEN)SelectObject(hdc, hPen); // Select new pen, save old
	HBRUSH hBrush = CreateSolidBrush(color); // Create brush
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdc, hBrush); // Select new brush, save old

	Ellipse(hdc, int((x - r)+.5), int((y - r) + .5), int((x + r) + .5), int((y + r) +.5)); // Draw circle

	SelectObject(hdc, hPenOld); // Replace with old pen
	DeleteObject(hPen); // Delete new pen
	SelectObject(hdc, hBrushOld); // Replace with old brush
	DeleteObject(hBrush); // Delete new brush
}

// Draw every ball in the ball simulator g_bsim
void drawAllBalls(const HDC hdc) {
	for (unsigned long i = 0; i < g_bsim.numBalls(); i++) {
		const Ball &b = g_bsim.getBall(i);
		drawSolidCircle(hdc, b.x(), b.y(), b.r(), (COLORREF)b.color());
	}
}

// Redraw the client area using double buffering
void updateDisplay(const HWND hWnd) {
	HDC hdcWindow = GetDC(hWnd);
	HDC hdcBuffer = CreateCompatibleDC(hdcWindow);

	RECT windowRect;
	GetClientRect(hWnd, &windowRect);

	HBITMAP hbmBuffer = CreateCompatibleBitmap(hdcWindow, windowRect.right, windowRect.bottom);
	HBITMAP hbmBufferOld = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

	FillRect(hdcBuffer, &windowRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	drawAllBalls(hdcBuffer);
	BitBlt(hdcWindow, 0, 0, windowRect.right, windowRect.bottom, hdcBuffer, 0, 0, SRCCOPY);
	
	// Cleanup:
	SelectObject(hdcBuffer, hbmBufferOld);
	DeleteObject(hbmBuffer);
	DeleteDC(hdcBuffer);
	ReleaseDC(hWnd, hdcWindow);
}

// Returns a random number in the range [min, max]
double getRandomNumber(double min, double max) {
	return (max - min) * rand() / RAND_MAX + min;
}

// Displays message that the maximum number of balls has been reached
// hWnd is the handle of the window displaying this message
void maxNumBallsMsg(HWND hWnd) {
	MessageBox(hWnd, "The maximum number of balls has been reached.", "Note", MB_ICONWARNING);
}

// Adds 10 random balls to the simulator
// hWnd is the handle of the window calling this function (used to display message in case of error)
void add10RandomBalls(HWND hWnd) {
	for (unsigned char i = 0; i < 10; i++) {
		if (g_bsim.numBalls() >= MAX_NUM_BALLS) {
			maxNumBallsMsg(hWnd);
			break;
		}

		double vx = getRandomNumber(MIN_RANDOM_V, MAX_RANDOM_V);
		double vy = getRandomNumber(MIN_RANDOM_V, MAX_RANDOM_V);
		double r = getRandomNumber(MIN_RANDOM_R, MAX_RANDOM_R);
		double area = M_PI * r * r;
		double m = M_TO_A_RATIO * area;
		unsigned int color = (unsigned int)getRandomNumber(0, 0xE0E0E0);
		
		Ball b;
		b.setVXY(vx, vy);
		b.setR(r);
		b.setM(m);
		b.setColor(color);
		g_bsim.addBall(b);
	}
}
		

// Add a ball from the "Add a ball" dialog.
// hWnd is the window handle of the "Add a ball" dialog from which the parameters will be retrieved.
// Return value is true for success, false if an error in input occurred.
// This function displays a message box on error.
// Parameters retrieved: diameter, mass, x velocity, y velocity
// These parameters are stored in ball g_bAdd.
// If the parameters are retrieved successfully, the ball is added to BallSim g_bsim.
bool addBallFromDlg(HWND hWnd) {
	const unsigned int bufLen = GET_INPUT_BUFFER_LEN;
	char buf[bufLen];
	
	g_bAdd.setXY(0., 0.); // Set coordinates to (0, 0)

	// Get x velocity
	GetWindowText(GetDlgItem(hWnd, IDC_VXENTRY), buf, bufLen);
	double vx = atof(buf);
	if (vx >= MIN_VX && vx <= MAX_VX) g_bAdd.setVX(vx);
	else {
		sprintf(buf, "The horizontal velocity must be between %.0f and %.0f, inclusive.", MIN_VX, MAX_VX);
		MessageBox(hWnd, buf, "Error", MB_ICONWARNING);
		SetFocus(GetDlgItem(hWnd, IDC_VXENTRY));
		return false;
	}
	
	// Get y velocity
	GetWindowText(GetDlgItem(hWnd, IDC_VYENTRY), buf, bufLen);
	double vy = atof(buf);
	if (vy >= MIN_VY && vy <= MAX_VY) g_bAdd.setVY(vy);
	else {
		sprintf(buf, "The vertical velocity must be between %.0f and %.0f, inclusive.", MIN_VY, MAX_VY);
		MessageBox(hWnd, buf, "Error", MB_ICONWARNING);
		SetFocus(GetDlgItem(hWnd, IDC_VYENTRY));
		return false;
	}
	
	// Get mass
	GetWindowText(GetDlgItem(hWnd, IDC_MASSENTRY), buf, bufLen);
	double m = atof(buf);
	if (m >= MIN_MASS && m <= MAX_MASS) g_bAdd.setM(m);
	else {
		sprintf(buf, "The mass must be between %.0f and %.0f, inclusive.", MIN_MASS, MAX_MASS);
		MessageBox(hWnd, buf, "Error", MB_ICONWARNING);
		SetFocus(GetDlgItem(hWnd, IDC_MASSENTRY));
		return false;
	}
	
	// Get diameter
	GetWindowText(GetDlgItem(hWnd, IDC_DIAMETERENTRY), buf, bufLen);
	double dia = atof(buf);
	if (dia >= MIN_DIAMETER && dia <= MAX_DIAMETER) g_bAdd.setR(dia / 2.); // Convert from diameter to radius
	else {
		sprintf(buf, "The diameter must be between %.0f and %.0f, inclusive.", MIN_DIAMETER, MAX_DIAMETER);
		MessageBox(hWnd, buf, "Error", MB_ICONWARNING);
		SetFocus(GetDlgItem(hWnd, IDC_DIAMETERENTRY));
		return false;
	}
	
	g_bsim.addBall(g_bAdd);
	return true;
}

// Sets up and calls Windows' ChooseColor() dialog to get the ball color.
// hWnd is the handle of the dialog calling this function
void chooseColorForAddBallDlg(HWND hWnd) {
	static COLORREF customColors[16]; // Array to hold custom colors used for "Choose Color" dialog
	CHOOSECOLOR cc;
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hWnd;
	cc.rgbResult = g_bAdd.color(); // Select color of last ball created
	cc.lpCustColors = customColors;
	cc.Flags = CC_RGBINIT; // Initialize color to value of rgbResult
	BOOL ccRetVal = ChooseColor(&cc);
	if (ccRetVal) g_bAdd.setColor(cc.rgbResult);
	if (GetRValue(cc.rgbResult) > LIGHT_COLOR_THRESHOLD && GetGValue(cc.rgbResult) > LIGHT_COLOR_THRESHOLD && GetBValue(cc.rgbResult) > LIGHT_COLOR_THRESHOLD) {
		MessageBox(hWnd, "Note: Light colored balls may be difficult to see.\r\nYou might wish to choose a different color.", "Note", MB_ICONINFORMATION);
	}
}

// When the "Add ball" dialog is created, this function sets the last-used values in the edit fields
// hWnd is the handle of the dialog calling this function
void setLastUsedValuesInAddBallDlg(HWND hWnd) {
	const unsigned long bufLen = GET_INPUT_BUFFER_LEN;
	char buf[bufLen];
	
	// Set diameter
	sprintf(buf, "%.2f", g_bAdd.r() * 2.); // Convert from radius to diameter
	SetWindowText(GetDlgItem(hWnd, IDC_DIAMETERENTRY), buf);
	
	// Set mass
	sprintf(buf, "%.2f", g_bAdd.m());
	SetWindowText(GetDlgItem(hWnd, IDC_MASSENTRY), buf);
	
	// Set horizontal velocity
	sprintf(buf, "%.2f", g_bAdd.vx());
	SetWindowText(GetDlgItem(hWnd, IDC_VXENTRY), buf);
	
	// Set vertical velocity
	sprintf(buf, "%.2f", g_bAdd.vy());
	SetWindowText(GetDlgItem(hWnd, IDC_VYENTRY), buf);
	
	// Color is set in the chooseColorForAddBallDlg() function
}

// Process messages for "Add a ball" dialog
BOOL CALLBACK addBallDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_INITDIALOG:
			setLastUsedValuesInAddBallDlg(hWnd);
		return TRUE;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) {
				case IDC_CANCEL:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
				
				case IDC_ADDBALL:
					if (addBallFromDlg(hWnd)) PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
				
				case IDC_COLOR:
					chooseColorForAddBallDlg(hWnd);
				break;
			}
		}
		return TRUE;
		
		case WM_CLOSE:
			EndDialog(hWnd, 0);
		return TRUE;
	}

	return FALSE; // Message was not processed
}

// Process messages for "About" dialog
BOOL CALLBACK aboutDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) {
				case IDC_ABOUT_OK:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			}
		}
		return TRUE;
		
		case WM_CLOSE:
			EndDialog(hWnd, 0);
		return TRUE;
	}
	
	return FALSE; // Message was not processed
}

// Process all messages to the main window
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	const UINT_PTR TIMER_ID_FRAME_DT = 1; // ID of the frame update timer
	static DWORD timeAtLastUpdateSim = 0; // time when the last WM_TIMER message with an ID of TIMER_ID_FRAME_DT was received
	static bool timerRunning = false; // Keeps track of whether or not the timer is running

	switch (msg) {
		case WM_PAINT:
		{
			RECT rc;
			if (GetUpdateRect(hWnd, &rc, FALSE)) { // check if there is an update region
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);

				// No need for double buffering here because we're just drawing on top of what's
				// already there, not clearing the display and drawing new circles
				drawAllBalls(hdc);

				EndPaint(hWnd, &ps);
			}
		}
		break;

		case WM_TIMER:
			if (wParam == TIMER_ID_FRAME_DT) PostMessage(hWnd, WMU_UPDATESIM, 0, 0);
		break;

		case WMU_UPDATESIM:
		{
			DWORD timeSinceLastFrame;
			const DWORD currentTime = timeGetTime();
			if (timeAtLastUpdateSim == 0) timeSinceLastFrame = FRAME_DT; // Account for uninitialized timeAtLastWM_TIMER
			else timeSinceLastFrame = currentTime - timeAtLastUpdateSim; // Normal calculation
			if (timeSinceLastFrame > OVERFLOW_THRESHOLD) timeSinceLastFrame = FRAME_DT; // Account for timer wrap-around
			timeAtLastUpdateSim = currentTime; // Update timeAtLastUpdateSim
			g_bsim.advanceSim(timeSinceLastFrame / 1000.); // Convert from milliseconds (timeSinceLastFrame) to sec.
			updateDisplay(hWnd);
		}
		break;
		
		case WMU_PAUSESIM:
			if (timerRunning) {
				KillTimer(hWnd, TIMER_ID_FRAME_DT);
				timerRunning = false;
			}
		break;
		
		case WMU_RESUMESIM:
			if (!timerRunning) {
				SetTimer(hWnd, TIMER_ID_FRAME_DT, FRAME_DT, NULL);
				timerRunning = true;
			}
		break;

		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED) PostMessage(hWnd, WMU_PAUSESIM, 0, 0);
			else {
				// LOWORD(lParam) and HIWORD(lParam) give the width and height of the window, so the coordinates
				// of the bottom-right pixel are actually (LOWORD(lParam) - 1, HIWORD(lParam) - 1). However, it
				// is necessary to add 1 to the coordinates of the bottom-right pixel because the walls are based
				// at the upper-left edge of the pixel, so for the wall to be positioned at the bottom-right edge
				// of the bottom-right pixel, the wall coordinates must be set one beyond the coordinates of the
				// bottom-right pixel.
				Walls w(0, 0, LOWORD(lParam), HIWORD(lParam));
				g_bsim.moveWalls(w);
				PostMessage(hWnd, WMU_RESUMESIM, 0, 0);
			}
		}
		break;

		case WM_GETMINMAXINFO:
		{
			RECT clientRC;
			GetClientRect(hWnd, &clientRC);
			RECT windowRC;
			GetWindowRect(hWnd, &windowRC);
			unsigned long extraWidth = windowRC.right - windowRC.left - clientRC.right;
			unsigned long extraHeight = windowRC.bottom - windowRC.top - clientRC.bottom;
			MINMAXINFO &mmi = *((MINMAXINFO*)lParam);
			POINT pt;
			pt.x = long(ceil(extraWidth + g_bsim.getMinWallDimension(clientRC.bottom)));
			pt.y = long(ceil(extraHeight + g_bsim.getMinWallDimension(clientRC.right)));
			mmi.ptMinTrackSize = pt;
		}
		break;
		
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) {
				case IDMI_MENU_ADDBALL:
					if (g_bsim.numBalls() >= MAX_NUM_BALLS) maxNumBallsMsg(hWnd);
               else DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADDBALL), hWnd, addBallDlgProc);
				break;
				
				case IDMI_MENU_ADD10BALLS:
					add10RandomBalls(hWnd);
				break;
				
				case IDMI_MENU_REMOVEALLBALLS:
					g_bsim.resetBalls();
				break;
				
				case IDMI_MENU_ABOUT:
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hWnd, aboutDlgProc);
				break;
				
				case IDMI_MENU_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			}
		}
		break;

		case WM_DESTROY:
			PostMessage(hWnd, WMU_PAUSESIM, 0, 0);
			PostQuitMessage(0);
		break;

		default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

// Entry point of the whole program
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	initAddBall(); // Set default values for the ball to be added
	
	srand(unsigned(timeGetTime())); // Initialize random number generator
	// srand(unsigned(time(NULL))); // Initialize random number generator

	const char mainWndClassName[] = "main";
	
	const HACCEL haccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDA_MAINACCEL));

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAINICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = MAKEINTRESOURCE(IDM_MENU);
	wc.lpszClassName = mainWndClassName;
	wc.hIconSm = NULL;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window class registration failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	HWND hWnd = CreateWindowEx(
	   WS_EX_CLIENTEDGE, // exStyle
	   mainWndClassName, // class name
	   "Bouncescope", // window title
	   WS_OVERLAPPEDWINDOW, // style
	   CW_USEDEFAULT, // x pos.
	   CW_USEDEFAULT, // y pos.
	   300, // width
	   300, // height
	   NULL, // hWndParent
		NULL, // hMenu
		hInst, // hInstance
		NULL // lpParam
	);

	if (hWnd == NULL) {
		MessageBox(NULL, "Main window creation failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	BOOL gmRet;
	while( (gmRet = GetMessage(&msg, NULL, 0, 0)) != 0 ) {
		if (gmRet == -1) {
			MessageBox(hWnd, "GetMessage error on main window.", "Error", MB_OK | MB_ICONEXCLAMATION);
			return 1;
		}
		else {
			if (!TranslateAccelerator(hWnd, haccel, &msg)) { // Check for keyboard accelerators
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return msg.wParam;
}
