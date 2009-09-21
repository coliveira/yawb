#include "stdafx.h"
#include "graphical.h"

#define MAX_LOADSTRING 100

class MainWindow {
    HINSTANCE hInst;								// current instance
    TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
    TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
public:
    MainWindow(HINSTANCE hInstance);
    ATOM RegisterClass();
    BOOL InitWindow(int nCmdShow);
    HINSTANCE GetInstance() { return hInst; };
    PTCHAR GetTitle() { return szTitle; }
    PTCHAR GetWindowClass() { return szWindowClass; }
};

static MainWindow *pMainWindow = 0;

HINSTANCE getGlobalInstance() {
    if (pMainWindow) return pMainWindow->GetInstance();
    return 0;
}

PTCHAR getAppTitle() {
    if (pMainWindow) return pMainWindow->GetTitle();
    return 0;
}

PTCHAR getWindowClass() {
    if (pMainWindow) return pMainWindow->GetWindowClass();
    return 0;
}

// some functions used in this module
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

class WindowProcessor {
public:
    LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

LRESULT WindowProcessor::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message) {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(getGlobalInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // this is a stub proc that just creates a new window and passes all messages to it
    static bool procCreated = false;
    if (procCreated) {
        WindowProcessor *proc = (WindowProcessor *)GetWindowLong(hWnd, GWL_USERDATA);
        return proc->WndProc(hWnd, message, wParam, lParam);
    }

    if (message == WM_CREATE) {
        SetWindowLong(hWnd, GWL_USERDATA, (LONG)new WindowProcessor());
        procCreated = true;
    } else {
        return DefWindowProc(hWnd, message, wParam, lParam); 
    }

    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

MainWindow::MainWindow(HINSTANCE hInstance) {
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    hInst = hInstance;
}


ATOM MainWindow::RegisterClass() {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInst;
    wcex.hIcon			= LoadIcon(hInst, MAKEINTRESOURCE(IDI_GRAPHICAL));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GRAPHICAL);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL MainWindow::InitWindow(int nCmdShow) {
   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 100, 100, NULL, NULL, hInst, NULL);
   if (!hWnd) return FALSE;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow) {
                         
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, getAppTitle(), MAX_LOADSTRING);
    LoadString(hInstance, IDC_GRAPHICAL, getWindowClass(), MAX_LOADSTRING);
    MainWindow myWindow(hInstance);
    myWindow.RegisterClass();
    pMainWindow = &myWindow; // this is safe because only this is the main function

    // Perform application initialization:
    if (!myWindow.InitWindow (nCmdShow)) return FALSE;

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICAL));

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))  {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

