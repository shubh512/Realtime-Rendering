#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<gl/GL.h>
#include<GL/glu.h>

#include"OGL.h"

//Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// link with opengl library
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

//Global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


FILE *gpfile = NULL;

HWND ghwnd = NULL;
BOOL gbActive = FALSE;
DWORD DWSTYLE = 0;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};
BOOL gbFullScreen = FALSE;

//OpenGl Global Var
HDC ghdc = NULL;
HGLRC ghrc = NULL; //H->Handle, GL->Grphics Lib, RC->Rendering Context Creates OPENGL context from device context
GLfloat rotate_angle_pyramid = 0.0f;
GLfloat cube_angle = 0.0f;

//Texture Object
GLuint texture_stone = 0;
GLuint texture_kundali = 0;

//Entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszcmdline, int iCmdShow)
{
    //Function declarations
    int initialize(void);
    void uninitialize(void);
    void display(void);
    void update(void);

    //Local variable declarations 
    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("sbp_window");
    int iResult = 0;
    BOOL bDone = FALSE;


    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = WIN_WIDTH;
    int windowHeight = WIN_HEIGHT;

    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    gpfile = fopen("log.txt", "w");
    if(gpfile == NULL)
    {
        MessageBox(NULL, TEXT("Log File Cannot be Opened!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        exit(0);
    }
    fprintf(gpfile, "Program Started Successfully\n");
    //code
    //WNDCLASS initialization
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //actual return value is HGDIOBJ
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON)); //
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); // NULL indiacates user doesn't have its own RESOURCE so OS can provide its own 
                                                    // ID -> Identification Arrow cursor of windows
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));; 
    
    //Register wndclassex
    RegisterClassEx(&wndclass);

    //Create window
    hwnd = CreateWindowEx(WS_EX_APPWINDOW,
                        szAppName,  // creates window in memory
                        // TEXTMACRO("shubham_Balaji_patil"),
                        TEXT("shubham_Balaji_patil"), //Convert ANSI string to UNICODE TEXT macro is used
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, // WS -> Window style that is overlapped window i.e. this window will pop up on other windows
                        x, // x
                        y, // y
                        WIN_WIDTH, // width
                        WIN_HEIGHT, // height
                        NULL,  // Parent window handle; if given NULL it assumes HEND_DESKTOP as parent windows as it is default window for OS
                        NULL,  // Handle of menu
                        hInstance, // Instance of which Window to be created
                        NULL);  
    
    ghwnd = hwnd;

    //intialization 
    iResult = initialize();
    if(iResult != 0)
    {
        MessageBox(hwnd, TEXT("Initialization failed!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        DestroyWindow(hwnd);
    }


    ShowWindow(hwnd, iCmdShow);  // shows windows created by createwindow by passing handle and shown as default by OS iCmdShow
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
    // Paint/Redraw the window
    // UpdateWindow(hwnd); // not required in game loop


    //Game Loop
    while(bDone == FALSE)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
                bDone = TRUE;
            
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }               
        }
        else
        {            
            if(gbActive == TRUE)
            {
                //render
                display();
                //update
                update();
            }
        }
    }

    // uninitialization
    uninitialize();

    fclose(gpfile);
    return (int)msg.wParam;
}

//Callback function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    //code
    //Local Function declarations
    void toggleFullScreen(void);
    void resize(int, int);

    //code
    switch(iMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            if(gpfile)
            {
                fprintf(gpfile, "Program Ended Successfully\n");
            }
            break;

        case WM_SETFOCUS:
            gbActive = TRUE;
            break;

        case WM_KILLFOCUS:
            gbActive = FALSE;
            break;

        case WM_SIZE:
            resize(LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_ERASEBKGND:
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set the clear color to blue
            glClear(GL_COLOR_BUFFER_BIT);
            return 1;

        case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {
                case VK_ESCAPE:
                    DestroyWindow(hwnd);
                    break;
            }
            break;

        case WM_CHAR:
            switch(LOWORD(wParam))
            {
                case 'F':
                case 'f':
                    if(gbFullScreen == FALSE)
                    {
                        toggleFullScreen();
                        gbFullScreen = TRUE;
                    }
                    else
                    {
                        toggleFullScreen();
                        gbFullScreen = FALSE;
                    }
                    break;
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        default:
            break;
    }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);  //Def Window Proc-> Default Window Procedure
}

void toggleFullScreen(void)
{
    //local var declarations
    MONITORINFO MI = {sizeof(MONITORINFO)};

    if(gbFullScreen == FALSE)
    {
        DWSTYLE = GetWindowLong(ghwnd, GWL_STYLE);
        if(DWSTYLE & WS_OVERLAPPEDWINDOW)
        {
            if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &MI))
            {
                SetWindowLong(ghwnd, GWL_STYLE, DWSTYLE &~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd, HWND_TOP, MI.rcMonitor.left, MI.rcMonitor.top, MI.rcMonitor.right - MI.rcMonitor.left, MI.rcMonitor.bottom - MI.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        ShowCursor(FALSE);
    }

    else 
    {
        SetWindowPlacement(ghwnd, &wpPrev);
        SetWindowLong(ghwnd, GWL_STYLE, DWSTYLE | WS_OVERLAPPEDWINDOW);
        SetWindowPos(ghwnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }
}

/* WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX */ 

int initialize(void)
{
    //function declarations
    //code
    BOOL loadGLTexture(GLuint *texture, TCHAR resourceId[]);
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex = 0;
    BOOL bResult;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    //Initialization of PixelFormatDescriptor
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cBlueBits = 8;
    pfd.cGreenBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32; // 1. Step for 3D

    //get the DC
    ghdc = GetDC(ghwnd);
    if(ghdc == NULL)
    {
        fprintf(gpfile, "Get DC Failed!\n");
        return -1;
    }
    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
    if(iPixelFormatIndex == 0)
    {
        fprintf(gpfile, "Get iPixelFormatIndex Failed!\n");
        return -2;
    }
    
    if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
    {
        fprintf(gpfile, "Get SetPixelFormat Failed!\n");
        return -3;
    }


    ghrc = wglCreateContext(ghdc);
    if(ghrc == NULL) 
    {
        fprintf(gpfile, "wglCreateContext Failed!\n");
        return -4;
    }
    //ghdc will end its role and gives control to ghrc
    //make rendering context current
    if(wglMakeCurrent(ghdc, ghrc) == FALSE)
    {
        fprintf(gpfile, "wglMakeCurrent Failed!\n");
        return -5;
    }
    //Enabling Depth 2 Step
    glShadeModel(GL_SMOOTH); // Beautification step (optional)
    glClearDepth(1.0f); // Mandatory Step
    glEnable(GL_DEPTH_TEST); // Mandatory Step
    glDepthFunc(GL_LEQUAL); // Mandatory Step
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Beautification step (optional)



    //set the clear color of window to blue
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //loading images to create texture
    bResult = loadGLTexture(&texture_stone, MAKEINTRESOURCE(MY_STONE_BITMAP));
    if(bResult == FALSE)
    {
        fprintf(gpfile, "loadGLTexture Stone texture Failed!\n");
        return -6;
    }
    bResult = loadGLTexture(&texture_kundali, MAKEINTRESOURCE(MY_KUNDALI_BITMAP));
    if(bResult == FALSE)
    {
        fprintf(gpfile, "loadGLTexture Kundali texture Failed!\n");
        return -7;
    }
    //tell opengl to enable the texture
    glEnable(GL_TEXTURE_2D);
    resize(WIN_WIDTH, WIN_HEIGHT);

    return 0;
}

BOOL loadGLTexture(GLuint *texture, TCHAR image_resource_id[])
{
    //local var
    HBITMAP hBitmap = NULL;
    BITMAP bmp;

    //Load the image
    hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), image_resource_id, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION); // 0, 0 is not null when you are loading icon
    if(hBitmap == NULL)                                                                                              //0, 0 means OS should directly take height and width from image resource
    {
        fprintf(gpfile, "Load image Failed!\n");
        return FALSE;
    }

    GetObject(hBitmap, sizeof(bmp), &bmp);
    //create opengl texture
    glGenTextures(1, texture);  //generate textures                                                                                                           
    //Bind to the generated texture
    glBindTexture(GL_TEXTURE_2D, *texture);
    //unpack image data and store it in pixelstore in integer form
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //LINEAR means return the image data as it is 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  
    //create multiple mipmap images
    
    // gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, (void *)bmp.bmBits); // 3 = GL_RGB 
    // BGR <> RGB == Horizontal flip GL_BGR_EXT
    // https://learn.microsoft.com/en-us/windows/win32/opengl/glteximage2d and compare arguements 
    // for https://learn.microsoft.com/en-us/windows/win32/opengl/glubuild2dmipmaps
    glTexImage2D(GL_TEXTURE_2D, 0, 3, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, (void *)bmp.bmBits);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // 0 indicates unbind the texture
    DeleteObject(hBitmap);

    hBitmap = NULL;

    return TRUE;
}                                                                                                                     

void resize(width, height)
{
    //code
    if(height <= 0)
        height = 1; 

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height,0.1f, 100.0f);

}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glTranslatef(-1.5f, 0.0f, -6.0f);
    glRotatef(rotate_angle_pyramid, 0.0f, 1.0f, 0.0f);
    // Internally gluLookAt creates LookAt/Camera/View matrix
    // gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    // First 3 coordinates are called Camera vectors or side vetors or eye/view vectors or x vector
    // Second 3 coordinates are called forward vector or z vector
    // Last 3 are called up vector or Y vector
    glBindTexture(GL_TEXTURE_2D, texture_stone);
    glBegin(GL_TRIANGLES);
    // front face
    // glColor3f(1.0f, 0.0f, 0.0f); 
    glTexCoord2f(0.5f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f); //Apex
    // glColor3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);  // Left Bottom
    // glColor3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f); // Right Bottom

    //right face
    // glColor3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.5f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    // glColor3f(0.0f, 0.0f, 1.0f); 
    glTexCoord2f(1.0f, 0.0f);  
    glVertex3f(1.0f, -1.0f, 1.0f);
    // glColor3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    //back face
    // glColor3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.5f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    // glColor3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    // glColor3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);

    //left face
    // glColor3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.5f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    // glColor3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    // glColor3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
  

    glLoadIdentity();

    glTranslatef(1.5f, 0.0f, -6.0f);
    glScalef(0.75f, 0.75f, 0.75f);
    glRotatef(cube_angle, 1.0f, 0.0f, 0.0f);
    glRotatef(cube_angle, 0.0f, 1.0f, 0.0f);
    glRotatef(cube_angle, 0.0f, 0.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, texture_kundali);
    glBegin(GL_QUADS);
   // front face
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

    // right face
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// back face
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

    // left face
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

    // top face
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

    // bottom face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
	SwapBuffers(ghdc);
}

void update(void)
{
    rotate_angle_pyramid = rotate_angle_pyramid + 0.4f;

    if(rotate_angle_pyramid >= 360.0f)
    {
        rotate_angle_pyramid = 0.0f;
    }

    cube_angle = cube_angle + 0.4f;
    if(cube_angle >= 360.0f)
    {
        cube_angle = 0.0f;
    }
}

void uninitialize(void)
{
    //function declarations
    void toggleFullScreen();

    //make hdc as current dc
    if(wglGetCurrentDC() == ghdc)
    {
        wglMakeCurrent(NULL, NULL);
    }

    //code
    if(gbFullScreen == TRUE)
    {
        toggleFullScreen();
        gbFullScreen = FALSE;
    }

    //Destroy rendering context
    if(ghrc)
    {
        wglDeleteContext(ghrc);
    }
    ghrc = NULL;
    
    //release the hdc
    if(ghdc)
    {
        ReleaseDC(ghwnd, ghdc);
    }
    ghdc = NULL;

    if(texture_kundali)
    {
        glDeleteTextures(1, &texture_kundali);
        texture_kundali = 0;
    }
    if(texture_stone)
    {
        glDeleteTextures(1, &texture_stone);
        texture_stone = 0;
    }
    DeleteDC(ghdc);
    fprintf(gpfile, "Program Ended Successfully!\n");
}