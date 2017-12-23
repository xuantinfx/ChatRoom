// ServerChatRoom.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ServerChatRoom.h"
#include "Chatter.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
VOID OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
VOID OnPaint(HWND hwnd);
VOID OnDestroy(HWND hwnd);
VOID OnClose(HWND hwnd);

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
#define SENDFILE L"JASJHDHJASKJHFJKHSAJKHDKLLJSADJKKLSAD"

/*******************Khu vực khai báo biến toàn cục*******************/
CSocket				ServerSocket; //khai báo biến server
vector<ChatterInfo *>	list_User; //khai báo danh sách các client
vector<CGroupChat *> list_GroupChat; // Khai báo danh sách các room chat
BOOL				g_quit = FALSE; //Biến để báo hiệu tắt server để các tiến trình tự động tắt
int					g_port = 1234;  // Biến port server hoạt động
HWND				hwndstatus; //tình trạng kết nối của Client
int					g_countConnect = 0; //số lượng Client kết nối
HWND				g_hWnd = NULL; //HWND chính của window

//khai báo hàm cho server
DWORD WINAPI ListenClient(LPVOID lpParameter);
BOOL InitAccount(WCHAR *fileName);
BOOL SaveAccount(WCHAR *filename);
BOOL InitPort(WCHAR *fileName);
VOID KillClientListenThread();
DWORD WINAPI ClientConnect(LPVOID lpParame);
VOID SendClient(CSocket *csocket, WCHAR* str);
VOID UpdateCountClient(int count);
VOID ReceiveAndSendText(CSocket *connectorReceive, WCHAR *_username);
VOID ReceiveAndSendFile(CSocket *connectorReceive, WCHAR *_username);

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
    LoadStringW(hInstance, IDC_SERVERCHATROOM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVERCHATROOM));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVERCHATROOM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_BTNFACE+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SERVERCHATROOM);
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
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_CLOSE, OnClose);
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


BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
	g_hWnd = hwnd;
	//khởi tạo server
	if (InitAccount(L"account.txt") == FALSE) {
		MessageBox(hwnd, L"Không mở được account.txt", L"Thông báo", MB_OK);
	}
	if (InitPort(L"config.txt") == FALSE) {
		MessageBox(hwnd, L"Không lấy được post từ file config.txt, lấy port mặc định là 1234", L"Thông báo", MB_OK);
	}
	hwndstatus = CreateWindow(L"STATIC", L"Không có ai kết nối", WS_VISIBLE | WS_CHILD, 30, 50, 400, 20, hwnd, NULL, hInst, NULL);
	CreateThread(NULL, 0, ListenClient, 0, 0, 0);
	return TRUE;
}

VOID OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	switch (id)
	{
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
		break;
	case IDM_EXIT:
		SendMessage(hwnd, WM_CLOSE, NULL, NULL);
		break;
	default:
		break;
	}
}

VOID OnPaint(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	// TODO: Add any drawing code that uses hdc here...
	EndPaint(hwnd, &ps);
}

VOID OnClose(HWND hwnd) {
	SaveAccount(L"account.txt");
	//giải phóng dữ liệu
	for (int i = 0; i < list_User.size(); i++) {
		delete list_User[i];
	}
	//kill thread đang chạy
	KillClientListenThread();
	//chờ kill xong thread
	Sleep(100);
	PostQuitMessage(0);
}

VOID OnDestroy(HWND hwnd) {
	PostQuitMessage(0);
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI ListenClient(LPVOID lpParameter)
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: code your application's behavior here.
			//khởi tạo thư viện socket
			if (AfxSocketInit() == FALSE)
			{
				MessageBox(GetForegroundWindow(), L"Không thể khởi tạo AfxSocketInit()", L"Thông báo", MB_OK);
				PostMessage(GetForegroundWindow(), WM_DESTROY, NULL, NULL);
			}
			else
			{
				//tạo socket cho server, port là PORT
				if (ServerSocket.Create(g_port, SOCK_STREAM, NULL) == FALSE) {
					WCHAR str[200];
					wsprintf(str, L"%d", ServerSocket.GetLastError());
					MessageBox(GetForegroundWindow(), str, L"Khởi tạo thất bại", MB_OK);
					PostMessage(GetForegroundWindow(), WM_DESTROY, NULL, NULL);
					ExitThread(0);
					return FALSE;
				}
				else {
					MessageBox(GetForegroundWindow(), L"Khởi tạo server thành công!", L"Thông báo", MB_OK);
					if (ServerSocket.Listen(1) == FALSE) {
						WCHAR str[200];
						wsprintf(str, L"Không thể lắng nghe trên port: %d", g_port);
						MessageBox(GetForegroundWindow(), str, L"Thông báo", MB_OK);
						ServerSocket.Close();
						PostMessage(GetForegroundWindow(), WM_DESTROY, NULL, NULL);
						ExitThread(0);
						return FALSE;
					}
					else {
						/*************/
						CSocket *Connector = new CSocket();
						//Chờ 1 kết nối từ Client
						while (TRUE) {
							if (ServerSocket.Accept(*Connector) == TRUE) {
								//nếu ứng dụng thoát thì thoát luôn Thread
								if (g_quit)
								{
									Connector->Close();
									ServerSocket.Close();
									ExitThread(0);
									return TRUE;
								}
								//Mở thêm 1 thread mới để xử lí kết nối
								g_countConnect++;
								UpdateCountClient(g_countConnect);

								//tạo biến để truyền dữ liêu giữa các thread
								MySocket *mySocket = new MySocket;
								//lấy SOCKET bỏ vào
								SOCKET *socket = new SOCKET;
								*socket = Connector->Detach();
								mySocket->socket = socket;

								//lấy CSocket bỏ vào
								mySocket->csocket = Connector;
								CreateThread(NULL, 0, ClientConnect, mySocket, 0, 0);
								Connector = new CSocket();
							}
						}
						/*************/
						return TRUE;
					}
				}
			}
			
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }
	ExitThread(0);
    return nRetCode;
}

DWORD WINAPI ClientConnect(LPVOID lpParame) {
	int index = 0;

	MySocket *mySocket = (MySocket *)lpParame;

	CSocket *connector = mySocket->csocket;

	connector->Attach(*mySocket->socket);

	delete []mySocket;

	//xử lí cho mỗi kết nối
	int count = 0;
	WCHAR message[200];
	while (true)
	{
		message[0] = 0;
		connector->Receive(message, 400, 0); //ở đây sẽ nhận thông điệp mà Client gửi lên
		//mấy bạn gọi xử lí và viết hàm để gọi trong đây
		
		if (message[0] == 0)//xử lí nếu kết nối không còn thì phá Thread
		{
			count++;
		}
		else
		{
			count = 0;
		}
		if (count == 10) {
			g_countConnect--;
			UpdateCountClient(g_countConnect);
			ExitThread(0);
			return TRUE;
		}

		/***********************************************************/

		/*****  đập và code lại mấy phần phía dưới này hết nha *****/

		/***********************************************************/
		//Yêu cầu gửi file (file nhị phân)
		if (StrCmpW(message, SENDFILE) == 0)
		{
			ReceiveAndSendFile(connector, list_User[index]->username);
		}
		//yêu cầu gửi tin nhắn
		if (StrCmpW(message, SENDTEXT) == 0) {
			ReceiveAndSendText(connector, list_User[index]->username);
		}
		//yêu cầu login,
		if (StrCmpW(message, LOGIN) == 0) {
			WCHAR username[200];
			connector->Receive(username, 400, 0);
			int i;
			for (i = 0; i < list_User.size(); i++) {
				if (StrCmpW(list_User[i]->username, username) == 0) {
					SendClient(connector, ACCEPT);
					list_User[i]->sv_connectorReceive = connector;
					index = i;
					break;
				}
			}
			if (i == list_User.size()) {
				SendClient(connector, DENY);
			}
		}
		//Yêu cầu thoát nhóm chat
		if (StrCmpW(message, ENDCHAT) == 0) {

		}

		//Cái này giữ nguyên không được sửa nha
		//Gửi một CSocket đến để nhận dữ liệu từ server của client
		if (StrCmpW(SOCKETRECEIVE, message) == 0) {
			WCHAR username[200];
			connector->Receive(username, 400,0);
			for (int i = 0; i < list_User.size(); i++) {
				if (StrCmpW(username, list_User[i]->username) == 0) {
					list_User[i]->sv_connectorSend = connector;
					list_User[i]->socketSendAttach = connector->Detach();
					break;
				}
			}
		}

		//Yêu cầu tạo 1 username mới
		if (lstrcmpW(SIGNIN, message) == 0) {
			WCHAR username[200];
			connector->Receive(username, 400, 0);
			//kiểm tra xem có trùng username nào trong dữ liệu k?
			int i;
			for (i = 0; i < list_User.size(); i++) {
				if (StrCmpW(username, list_User[i]->username) == 0)
				{
					SendClient(connector, DENY);
					break;
				}
			}
			if (i == list_User.size()) {
				SendClient(connector, ACCEPT);
				ChatterInfo * chatter = new ChatterInfo(username);
				list_User.push_back(chatter);
			}
		}
		//yêu cầu tạo nhóm chat
		if (StrCmpW(message, NEWGROUPCHAT) == 0) {
			int sl = 0;
			connector->Receive(&sl, 4);
			CGroupChat *groupchat = new CGroupChat();
			list_GroupChat.push_back(groupchat);
			WCHAR temp[200];
			//thêm chính nó vào group chat
			groupchat->lMember.push_back(list_User[index]);
			for (int i = 0; i < sl; i++) {//duyệt qua danh sách user client gửi
				connector->Receive(temp, 400);
				for (int j = 0; j < list_User.size(); j++) { //tìm user trong data
					if (StrCmpW(list_User[j]->username, temp) == 0) { //nếu tìm thấy
						groupchat->lMember.push_back(list_User[j]); // bỏ chatter đó vào list thành viên của group
						break;
					}
				}
			}
			//trả về id nhóm cho mọi người trong group vừa mới tạo
			for (int j = 0; j < groupchat->lMember.size(); j++) {
				groupchat->lMember[j]->sv_connectorSend->Attach(groupchat->lMember[j]->socketSendAttach);
				SendClient(groupchat->lMember[j]->sv_connectorSend, NEWGROUPCHAT);
				SendClient(groupchat->lMember[j]->sv_connectorSend, ACCEPT);
				groupchat->lMember[j]->sv_connectorSend->Send(&groupchat->id, 4);
				groupchat->lMember[j]->socketSendAttach = groupchat->lMember[j]->sv_connectorSend->Detach();
			}
		}
		
	}
	ExitThread(0);
	return TRUE;
}

//hàm kill thread khi Server đóng
VOID KillClientListenThread() {
	CSocket ClientSocket;
	ClientSocket.Create();

	// Ket noi den Server
	if (ClientSocket.Connect(_T("127.0.0.1"), g_port) != 0)
	{
		char ss[1];
		ClientSocket.Send(ss, 1, 0);
		g_quit = TRUE;
		ClientSocket.Close();
	}
}

//Khởi tạo danh sách account
BOOL InitAccount(WCHAR *fileName) {
	fstream fin;
	fin.open(fileName, ios_base::in);

	if (!fin.is_open()) {
		return FALSE;
	}

	WCHAR temp[51];
	char tempctr[51];
	ChatterInfo *tempacc;
	

	while (!fin.eof())
	{
		fin >> tempctr;

		for (int i = 0; i < 51; i++) {
			temp[i] = tempctr[i];
		}
		
		tempacc = new ChatterInfo(temp);
		list_User.push_back(tempacc);
	}

	fin.close();
	return TRUE;
}

//Lưu lại danh sách Account
BOOL SaveAccount(WCHAR *filename) {
	fstream fout;
	fout.open(filename, ios_base::out);

	if (!fout.is_open()) {
		return FALSE;
	}
	char tempctr[51];
	int i;
	for (i = 0; i < list_User.size() -1; i++) {
		for (int j = 0; j < 51; j++) {
			tempctr[j] = list_User[i]->username[j];
		}
		fout << tempctr << endl;
	}
	for (int j = 0; j < 51; j++) {
		tempctr[j] = list_User[i]->username[j];
	}
	fout << tempctr;

	fout.close();
	return TRUE;
}

//Mở file đọc port của server
BOOL InitPort(WCHAR *fileName) {
	fstream fin;
	fin.open(fileName, ios_base::in);

	if (!fin.is_open()) {
		return FALSE;
	}

	char temp[51];
	fin >> temp;

	g_port = atoi(temp);
	
	fin.close();
	return TRUE;
}

//hàm bọc lại hàm gửi để tránh sinh ra lỗi
VOID SendClient(CSocket *csocket,WCHAR* str) {
	WCHAR temp[200];
	wsprintf(temp, L"%s", str);
	int lenth = csocket->Send(temp, 400, 0);
}

//cập nhật số kết nối đến Server
VOID UpdateCountClient(int count) {
	WCHAR strCount[200];
	wsprintf(strCount, L"Có %d kết nối", count);
	SetWindowText(hwndstatus, strCount);
}


//hàm đảm nhận chức năng nhận và gửi text
VOID ReceiveAndSendText(CSocket *connectorReceive,WCHAR *_username) {
	int id = 0;
	WCHAR content[200];
	connectorReceive->Receive(&id, 4);
	connectorReceive->Receive(content, 400);

	//tìm group mang ID = id để gửi
	//send
	for (int i = 0; i < list_GroupChat.size(); i++) {
		if (id == list_GroupChat[i]->id) {
			for (int j = 0; j < list_GroupChat[i]->lMember.size(); j++) {
				if (StrCmpW(list_GroupChat[i]->lMember[j]->username, _username) != 0) {
					list_GroupChat[i]->lMember[j]->sv_connectorSend->Attach(list_GroupChat[i]->lMember[j]->socketSendAttach);
					SendClient(list_GroupChat[i]->lMember[j]->sv_connectorSend, SENDTEXT);
					list_GroupChat[i]->lMember[j]->sv_connectorSend->Send(content, 400);
					list_GroupChat[i]->lMember[j]->socketSendAttach = list_GroupChat[i]->lMember[j]->sv_connectorSend->Detach();
				}
			}
			break;
		}
	}
}

//Hàm nhận và gửi file
VOID ReceiveAndSendFile(CSocket *connectorReceive, WCHAR *_username) {
	int id = 0;
	CHAR content[200];
	WCHAR fname[200];
	unsigned long size;
	//Nhận id từ client
	connectorReceive->Receive(&id, 4);

	//Nhận tên file từ client
	connectorReceive->Receive(fname, 400);

	//Nhận kích thước file từ client
	connectorReceive->Receive(&size, sizeof(size));
									  //tìm group mang ID = id để gửi
									  //send
	for (int i = 0; i < list_GroupChat.size(); i++) {
		if (id == list_GroupChat[i]->id) {
			for (int j = 0; j < list_GroupChat[i]->lMember.size(); j++) {
				if (StrCmpW(list_GroupChat[i]->lMember[j]->username, _username) != 0) {
					list_GroupChat[i]->lMember[j]->sv_connectorSend->Attach(list_GroupChat[i]->lMember[j]->socketSendAttach);
					SendClient(list_GroupChat[i]->lMember[j]->sv_connectorSend, SENDFILE);

					// Chi tiết gửi
					list_GroupChat[i]->lMember[j]->sv_connectorSend->Send(fname, 400); //Gửi tên file
					list_GroupChat[i]->lMember[j]->sv_connectorSend->Send(&size, sizeof(size));  //Gửi kích thước file
					unsigned long count = 0;
					while (true)
					{
						if (size - count >= 200)
						{
							//Nhận nội dung từng khối và gửi cho client
							connectorReceive->Receive(content, 200);
							list_GroupChat[i]->lMember[j]->sv_connectorSend->Send(content, 200);
							count += 200;
						}
						else
						{
							//Gửi nội dung cuối của gói tin
							int left = size - count;
							connectorReceive->Receive(content, left);
							list_GroupChat[i]->lMember[j]->sv_connectorSend->Send(content, left);
							break;
						}
					}
					//Kết thúc nội dung gửi

					list_GroupChat[i]->lMember[j]->socketSendAttach = list_GroupChat[i]->lMember[j]->sv_connectorSend->Detach();
				}
			}
			break;
		}
	}
}