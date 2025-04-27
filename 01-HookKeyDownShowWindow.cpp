#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>

HHOOK g_hHook = NULL;  // 用于保存钩子句柄

HWND hMainWnd;
// 键盘钩子回调函数
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    PKBDLLHOOKSTRUCT pKbd = (PKBDLLHOOKSTRUCT)lParam;
    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)  // 键盘按下事件
    {
        OutputDebugString(TEXT("Ctrl + H Pressed!\n"));
        // 在这里执行你想要的操作，比如显示或隐藏窗口
        if (hMainWnd) ShowWindow(hMainWnd, SW_SHOWNORMAL);
        SetWindowPos(hMainWnd, HWND_TOPMOST, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE));
    }
    else if (wParam == WM_KEYUP)  // 键盘松开事件
    {
        OutputDebugString(TEXT("Ctrl + H Released!\n"));
        // 在这里执行你想要的操作，比如显示或隐藏窗口
        if (hMainWnd) ShowWindow(hMainWnd, SW_HIDE);
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);  // 传递给下一个钩子
}

// 安装全局键盘钩子
bool InstallKeyboardHook()
{
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (g_hHook == NULL)
    {
        MessageBox(NULL, TEXT("Failed to install keyboard hook!"), TEXT("error"), MB_ICONERROR);
        return false;
    }
    return true;
}

// 卸载键盘钩子
void UninstallKeyboardHook()
{
    if (g_hHook != NULL)
    {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
    }
}

static int x = 1000;
static int y = 1000;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        InstallKeyboardHook();
        CreateWindow(TEXT("BUTTON"), TEXT("测试按钮"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            100, 100, 200, 200, hWnd, NULL, NULL, NULL);
        break;
    }
    case WM_PAINT:
    {
        break;
    }
    case WM_NCHITTEST:
        return HTCLIENT;
    case WM_SIZE:
    {
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);
        HDWP hDWP = BeginDeferWindowPos(1);
        if (hDWP)
        {
            DeferWindowPos(
                hDWP,
                NULL,
                NULL,
                0, 0,
                width, height,
                SWP_NOZORDER
            );
            EndDeferWindowPos(hDWP);
        }
        break;
    }
    case WM_KEYDOWN:
    {
        break;
    }
    case WM_LBUTTONDOWN:
    {
        PostMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
        break;
    }
    case WM_DESTROY:
        UninstallKeyboardHook();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.cbClsExtra = 0;                                                                    
    wcex.cbWndExtra = 0;                                                                    
    wcex.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);                                             
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);                                           
    wcex.hInstance = hInstance;                                                             
    wcex.lpfnWndProc = WindowProc;                                                          
    wcex.lpszClassName = TEXT("MainClass");                                                 
    wcex.lpszMenuName = NULL;                                                               
    wcex.style = CS_HREDRAW | CS_VREDRAW;                                                   

    RegisterClassEx(&wcex);

    hMainWnd = CreateWindowEx(WS_EX_TOPMOST, wcex.lpszClassName, TEXT("MainFrame"), WS_OVERLAPPEDWINDOW,
        100, 100, 400, 300,
        NULL, NULL, hInstance, NULL);
    if (hMainWnd == NULL)
    {
        return 0;
    }
    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    UpdateWindow(hMainWnd);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
