// ClientChatRoom.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ClientChatRoom.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance	
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

/*****************Khu vực define các chức năng*****************/
#define NEWGROUPCHAT L"QUBCJSDNFKJKZLZKPXOCKLSSDLFJDJFKJSDKJKFDF"
#define ENDCHAT L"UAJHJDBAJYIUIDNKJUAKJIQUIDKJSUDIYSUWHJHEJHKS"
#define ACCEPT L"IQWIJDKSAJKAKSSJDKJSKDSJKASJDKSAJDKJADKJKKDJ"
#define DENY L"QWYYGHBDNSBSJAHDJASHJDHADSAJDJHJASHDJHSASJDH"
#define SENDFILE L"HIWUEINMNSDLKFOEKLJSKDJFKEJSKJDKFJKJKEJWKSJ"
#define SIGNIN L"UYOWMNXNCHSKKDJKFJKJSSEJKJKJSKDJIFJUIWIEIRIS"
#define LOGIN L"MKWHOQJFKJDFKJSKJEIUEIJFKJSSKDKGHJEKDJSJDJFJ"
#define SOCKETRECEIVE L"WYUHKLSDJFKDSJKFJDSKFJJFSJFKDSJFKSJKFSW"
#define SENDTEXT L"JHAJJAJAJHJQUYUQYUYUQYUYUDUUDUHDJHDJHDJH"

//Khai báo biến cho socket
HWND		hwndtext;
HWND		hwndLogin, hwndLogout, hwndSend, hwndSignin,hwndNewGroupChat,hwndStatus,hwndReceive;
CSocket		ClientSocketSend, ClientSocketReceive;
BOOL		quit = FALSE;
BOOL		sendtext = FALSE;
WCHAR		username[200];
HWND		g_hWnd = NULL;
WCHAR		ip[20];
int			port;
int			id; //vector<int>---------------------------------------------------------------------------------
//khai báo hàm cho socket
int ClientInit();
VOID ClientSent(CSocket *connector, WCHAR* str);
DWORD WINAPI ClientReceive(LPVOID lpParame);
BOOL InnitIPAndPort(WCHAR *fileName);
VOID SendText();
VOID ReceiveText();


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENTCHATROOM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTCHATROOM));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
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
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENTCHATROOM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIENTCHATROOM);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		g_hWnd = hWnd;
		if (InnitIPAndPort(L"config.txt") == FALSE) {
			MessageBox(hWnd, L"Hiện tại không thể mở file config.txt", L"Lỗi", MB_OK);
			PostQuitMessage(0);
		}

		if (ClientInit() == FALSE) {
			MessageBox(hWnd, L"Hiện tại không thể kết nối Server!", L"Lỗi", MB_OK);
			PostQuitMessage(0);
		}

		//tạo nút đăng nhập
		hwndLogin = CreateWindow(L"BUTTON", L"Đăng nhập", WS_VISIBLE | WS_CHILD, 10, 10, 200, 40, hWnd, (HMENU)IDC_LOGIN, hInst, NULL);
		hwndLogout = CreateWindow(L"BUTTON", L"Đăng xuất", WS_VISIBLE | WS_CHILD, 220, 10, 200, 40, hWnd, (HMENU)IDC_LOGOUT, hInst, NULL);
		EnableWindow(hwndLogout, false);
		hwndSignin = CreateWindow(L"BUTTON", L"Đăng ký", WS_VISIBLE | WS_CHILD, 430, 10, 200, 40, hWnd, (HMENU)IDC_SIGNIN, hInst, NULL);
		hwndSend = CreateWindow(L"BUTTON", L"Gửi", WS_VISIBLE | WS_CHILD, 640, 10, 200, 40, hWnd, (HMENU)IDC_BTNSENT, hInst, NULL);
		EnableWindow(hwndSend, false);
		hwndNewGroupChat = CreateWindow(L"BUTTON", L"Thêm group chat", WS_VISIBLE | WS_CHILD, 850, 10, 200, 40, hWnd, (HMENU)IDC_NEWGROUPCHAT, hInst, NULL);
		EnableWindow(hwndNewGroupChat, false);
		hwndStatus = CreateWindow(L"STATIC", L"Chưa đăng nhập", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 60, 400, 40, hWnd, NULL, hInst, NULL);

		hwndtext = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD|WS_BORDER, 10, 120, 400, 80, hWnd, (HMENU)NULL, hInst, NULL);

		hwndReceive = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 200, 400, 80, hWnd, (HMENU)NULL, hInst, NULL);

		break;
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDC_LOGIN:
			{
				//login
				ClientSent(&ClientSocketSend, LOGIN);
				WCHAR tempsend[200], tempreceive[200];
				GetWindowText(hwndtext, tempsend, 200);
				ClientSent(&ClientSocketSend, tempsend);

				//nhận trả lời từ server
				ClientSocketSend.Receive(tempreceive, 400, 0);
				if (StrCmpW(ACCEPT, tempreceive) == 0) {
					MessageBox(hWnd, L"Success", NULL, NULL);
					ClientSocketReceive.Create();
					if (ClientSocketReceive.Connect(ip, port) == TRUE) {
						//gửi thông điệp gửi luồng nhận cho server biết
						ClientSent(&ClientSocketReceive, SOCKETRECEIVE);
						ClientSent(&ClientSocketReceive, tempsend); //gửi username

																	//tạo biến để truyền dữ liêu giữa các thread
						MySocket *mySocket = new MySocket;
						//lấy SOCKET bỏ vào
						SOCKET *socket = new SOCKET;
						*socket = ClientSocketReceive.Detach();
						mySocket->socket = socket;

						//lấy CSocket bỏ vào
						mySocket->csocket = &ClientSocketReceive;
						CreateThread(NULL, 0, ClientReceive, mySocket, 0, 0);
						StrCpyW(username, tempsend);
						sendtext = TRUE;
						SetWindowText(hWnd, username);

						EnableWindow(hwndLogin, false);
						EnableWindow(hwndLogout, true);
						EnableWindow(hwndSignin, false);
						EnableWindow(hwndNewGroupChat, true);

						WCHAR temp[200];
						wsprintf(temp, L"Đã đăng nhập với username %s", username);
						SetWindowText(hwndStatus, temp);
						SetWindowText(hwndtext, L"");
					}
				}
				else
				{
					MessageBox(hWnd, L"False", NULL, NULL);
				}
				break;
			}
			case IDC_LOGOUT:
			{
				break;
			}
			case IDC_SIGNIN:
			{
				ClientSent(&ClientSocketSend, SIGNIN);
				WCHAR temp[200];
				GetWindowText(hwndtext, temp, 200);
				ClientSent(&ClientSocketSend,temp);
				ClientSocketSend.Receive(temp, 200, 0);
				if (StrCmpW(temp, ACCEPT) == 0)
				{
					MessageBox(hWnd, L"Thành công, vui lòng đăng nhập với username vừa đăng kí", L"Thông báo", MB_OK);
				}
				else {
					MessageBox(hWnd, L"Thất bại", L"Thông báo", MB_OK);
				}
				break;
			}
			case IDC_BTNSENT: //nhấn nút gửi
			{

				SendText();
				/*ClientSent(&ClientSocketSend, SOCKETTEXT);
				ClientSocketSend.Send(&id, 4);
				WCHAR temp[200];
				GetWindowText(hwndtext, temp, 200);
				ClientSent(&ClientSocketSend, temp);*/
				break;
			}
			case IDC_NEWGROUPCHAT:
			{
				//danh sách username được thêm cách nhau bằng dấu ','
				WCHAR text_list_user[200];
				WCHAR list_user[100][50];
				//lấy danh sách username
				GetWindowText(hwndtext, text_list_user, 200);
				if (StrCmpW(text_list_user, L"") == 0)
				{
					break;
				}
				int i = 0, j = 0, count = 0;
				while (text_list_user[i] != L'\0')
				{
					if (text_list_user[i] == L',') {
						list_user[count++][j] = L'\0';
						j = 0;
						i++;
						continue;
					}
					if (text_list_user[i] == L' ') {
						i++;
						continue;
					}
					list_user[count][j] = text_list_user[i];
					j++; i++;
				}
				list_user[count++][j] = L'\0';

				ClientSent(&ClientSocketSend, NEWGROUPCHAT);
				ClientSocketSend.Send(&count, 4);
				for (int k = 0; k < count; k++) {
					ClientSent(&ClientSocketSend, list_user[k]);
				}
				break;
			}
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                SendMessage(hWnd,WM_CLOSE,0,0);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		ClientSocketSend.CancelBlockingCall();
		quit = true;
		Sleep(1000);
        PostQuitMessage(0);
        break;
	case WM_CLOSE:
		//gửi thông điệp đóng kết nối
	{
		ClientSent(&ClientSocketSend, ENDCHAT);
		SendMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	}
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

//Khởi tạo Client
int ClientInit() //init client
{
    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
			return FALSE;
        }
        else
        {
			//Khởi tạo socket
			AfxSocketInit();
			ClientSocketSend.Create();

			//kết nối đến server
			if (ClientSocketSend.Connect(ip, port)) {
				return TRUE;
			}
            // TODO: code your application's behavior here.
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
		return FALSE;
    }

    return FALSE;
}

//Hàm bọc lại hàm gửi tránh lỗi
VOID ClientSent(CSocket *connector, WCHAR* str) {
	WCHAR temp[200];
	wsprintf(temp, L"%s", str);
	connector->Send(temp, 400, 0);
}

//Thread dùng để nhận dữ liệu từ server
DWORD WINAPI ClientReceive(LPVOID lpParame) {
	//lấy connector từ Thread chính đẩy qua
	MySocket *mySocket = (MySocket *)lpParame;

	CSocket *connector = mySocket->csocket;

	connector->Attach(*mySocket->socket);

	delete[]mySocket;

	WCHAR temp[200] = { 0 };
	int count = 0;
	while (true) {
		temp[0] = 0;
		//Nhận thông điệp từ server
		//mấy bạn code hàm và gọi hàm ở trong đây
		connector->Receive(temp, 400, 0);
		if (quit) { //Nếu client thoát thì exit thread
			ExitThread(0);
			return TRUE;
		}
		//kiểm tra xem server còn sống không, nếu chết thì thoát Thread
		if (temp[0] == 0)
		{
			count++;
		}
		else
		{
			count = 0;
		}
		if (count == 10) {
			ExitThread(0);
			return TRUE;
		}

		/***********************************************************/

		/*****  đập và code lại mấy phần phía dưới này hết nha *****/

		/***********************************************************/

		if (StrCmpW(temp, NEWGROUPCHAT) == 0) // trả lời của server về việc tạo group chat
		{
			WCHAR temp[200];
			ClientSocketReceive.Receive(temp, 400); //nhận thông điệp đồng ý hay từ chối
			if (StrCmpW(ACCEPT, temp) == 0) {
				MessageBox(g_hWnd, L"Thành công", L"Thông báo", MB_OK);

				ClientSocketReceive.Receive(&id, 4); //nhận id group chat
				EnableWindow(hwndNewGroupChat, false);
				EnableWindow(hwndSend, true);

			}
			else
			{
				ClientSocketReceive.Receive(temp, 400); //nhận nội dung lỗi
				MessageBox(g_hWnd, temp, L"Thông báo", MB_OK);
			}
		}
		if (StrCmpW(temp, SENDTEXT) == 0) {
			ReceiveText();
		}
	}
	ExitThread(0);
	return TRUE;
}

//khởi tạo Ip và Port từ file config.txt để kết nối đến server
BOOL InnitIPAndPort(WCHAR *fileName) {
	fstream fin;
	fin.open(fileName, ios_base::in);
	if (!fin.is_open())
	{
		return FALSE;
	}

	char temp[50];

	fin >> temp;

	for (int i = 0; i <= strlen(temp); i++) {
		ip[i] = temp[i];
	}

	fin >> temp;
	port = atoi(temp);

	fin.close();
	return TRUE;
}

//Hàm gửi yêu cầu gửi text và gửi text
VOID SendText() {
	ClientSent(&ClientSocketSend, SENDTEXT);
	ClientSocketSend.Send(&id, 4);
	//lấy nội dung từ ô nhập text
	WCHAR temp[200];
	GetWindowText(hwndtext, temp, 200);
	ClientSent(&ClientSocketSend, temp);
}

//Hàm nhận text từ server
VOID ReceiveText() {
	WCHAR temp[200];
	ClientSocketReceive.Receive(temp, 400);

	//hiển thị text nhận được lên màn hình
	SetWindowText(hwndReceive, temp);
}