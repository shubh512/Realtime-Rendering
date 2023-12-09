//Windows header file
#include<windows.h>

//Global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



//Entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszcmdline, int iCmdShow)
{
    //Local variable declarations 
    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("sbp_window");

    //code
    //WNDCLASS initialization
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW; 
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //actual return value is HGDIOBJ
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //using any resource like cursor, menu requires "load" keyword
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); // NULL indiacates user doesn't have its own RESOURCE so OS can provide its own 
                                                    // ID -> Identification Arrow cursor of windows
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION); 
    
    //Register wndclassex
    RegisterClassEx(&wndclass);

    //Create window
    hwnd = CreateWindow(szAppName,  // creates window in memory
                        // TEXTMACRO("shubham_Balaji_patil"),
                        TEXT("shubham_Balaji_patil"), //Convert ANSI string to UNICODE TEXT macro is used
                        WS_OVERLAPPEDWINDOW, // WS -> Window style that is overlapped window i.e. this window will pop up on other windows
                        CW_USEDEFAULT, // 
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        NULL,  // Parent window handle; if given NULL it assumes HEND_DESKTOP as parent windows as it is default window for OS
                        NULL,  // Handle of menu
                        hInstance, // Instance of which Window to be created
                        NULL);  

    ShowWindow(hwnd, iCmdShow);  // shows windows created by createwindow by passing handle and shown as default by OS iCmdShow


    //Paint/Redraw the window
    UpdateWindow(hwnd);

    //Message loop
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

//Callback function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    //code
    switch(iMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        
        default:
            break;
    }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);  //Def Window Proc-> Default Window Procedure
}


/* WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX */ 