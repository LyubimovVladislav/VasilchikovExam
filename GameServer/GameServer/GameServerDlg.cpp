
// GameServerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "GameServer.h"
#include "GameServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGameServerDlg dialog

#include <winsock2.h>
#include <list>

#define PORT 5150			
#define DATA_BUFSIZE 8192 	

bool bPrint = false;
int  iPort = PORT;

typedef struct {
	SOCKET player1;
	SOCKET player2;
	int turn;
	int** player1Field;
	int** player2Field;
	int player1GameOver;
	int player2GameOver;
} Board;

void StartGame(Board, CListBox* pLB);


std::list<Board> m_games;

typedef struct _SOCKET_INFORMATION {
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

void PrepareMessage(LPSOCKET_INFORMATION socketInfo, CListBox* pLB);
void AddInfo(LPSOCKET_INFORMATION socketInfo, CListBox* pLB);
void SendMsgtoOponent(LPSOCKET_INFORMATION socketInfo, CListBox* pLB);
int IsHit(int** matr, char* pos);
void SendInfo(SOCKET socket, char* message, int player, int flag, CListBox* pLB);
void SendGameOver(SOCKET socket, int win, CListBox* pLB);
void FreeGameInfo(LPSOCKET_INFORMATION SocketInfo);

BOOL CreateSocketInformation(SOCKET s, char* Str, CListBox* pLB);
void FreeSocketInformation(DWORD Event, char* Str, CListBox* pLB);

DWORD EventTotal = 0;
WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
LPSOCKET_INFORMATION SocketArray[WSA_MAXIMUM_WAIT_EVENTS];

HWND   hWnd_LB;  // Для вывода в других потоках

UINT ListenThread(PVOID lpParam);





CGameServerDlg::CGameServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GAMESERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGameServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX, m_ListBox);
}

BEGIN_MESSAGE_MAP(CGameServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CGameServerDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_PRINT, &CGameServerDlg::OnBnClickedPrint)
END_MESSAGE_MAP()


// CGameServerDlg message handlers

BOOL CGameServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	char Str[128];
	sprintf_s(Str, sizeof(Str), "%d", iPort);
	GetDlgItem(IDC_PORT)->SetWindowText(Str);
	AfxGetApp()->GetMainWnd()->SetWindowText("Game Server");

	((CButton*)GetDlgItem(IDC_PRINT))->SetCheck(bPrint);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGameServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGameServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGameServerDlg::OnBnClickedStart()
{
	char Str[81];

	hWnd_LB = m_ListBox.m_hWnd;   // Для ListenThread
	GetDlgItem(IDC_PORT)->GetWindowText(Str, sizeof(Str));
	iPort = atoi(Str);
	if (iPort <= 0 || iPort >= 0x10000)
	{
		AfxMessageBox("Incorrect Port number");
		return;
	}

	AfxBeginThread(ListenThread, NULL);

	GetDlgItem(IDC_START)->EnableWindow(false);

}

void CGameServerDlg::OnBnClickedPrint()
{
	bPrint = ((CButton*)GetDlgItem(IDC_PRINT))->GetCheck() == 1;
}


UINT ListenThread(PVOID lpParam)
{
	SOCKET Listen;
	SOCKET Accept;
	SOCKADDR_IN InternetAddr;
	DWORD Event;
	WSANETWORKEVENTS NetworkEvents;
	WSADATA wsaData;
	DWORD Ret;
	DWORD Flags;
	DWORD RecvBytes;
	DWORD SendBytes;
	char  Str[200];
	CListBox* pLB = (CListBox*)(CListBox::FromHandle(hWnd_LB));

	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0)
	{
		sprintf_s(Str, sizeof(Str), "WSAStartup() failed with error %d", Ret);
		pLB->AddString(Str);
		return 1;
	}

	if ((Listen = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		sprintf_s(Str, sizeof(Str), "socket() failed with error %d", WSAGetLastError());
		pLB->AddString(Str);
		return 1;
	}
	CreateSocketInformation(Listen, Str, pLB);

	if (WSAEventSelect(Listen, EventArray[EventTotal - 1], FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str), "WSAEventSelect() failed with error %d", WSAGetLastError());
		pLB->AddString(Str);
		return 1;
	}

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(iPort);

	if (bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str), "bind() failed with error %d", WSAGetLastError());
		pLB->AddString(Str);
		return 1;
	}

	if (listen(Listen, 5)) //Второй параметр функции listen() задает максимальную длину очереди ожидающих соединения клиентов
	{
		sprintf_s(Str, sizeof(Str), "listen() failed with error %d", WSAGetLastError());
		pLB->AddString(Str);
		return 1;
	}

	while (TRUE)
	{
		// Дожидаемся уведомления о событии на любом сокете
		if ((Event = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, WSA_INFINITE, FALSE)) == WSA_WAIT_FAILED)
		{
			sprintf_s(Str, sizeof(Str), "WSAWaitForMultipleEvents failed with error %d", WSAGetLastError());
			pLB->AddString(Str);
			return 1;
		}

		if (WSAEnumNetworkEvents(SocketArray[Event - WSA_WAIT_EVENT_0]->Socket, EventArray[Event - WSA_WAIT_EVENT_0], &NetworkEvents) == SOCKET_ERROR)
		{
			sprintf_s(Str, sizeof(Str), "WSAEnumNetworkEvents failed with error %d", WSAGetLastError());
			pLB->AddString(Str);
			return 1;
		}
		if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
		{
			if (NetworkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				sprintf_s(Str, sizeof(Str), "FD_ACCEPT failed with error %d", NetworkEvents.iErrorCode[FD_ACCEPT_BIT]);
				pLB->AddString(Str);
				break;
			}

			// Прием нового соединения и добавление его
			// в списки сокетов и событий
			if ((Accept = accept(SocketArray[Event - WSA_WAIT_EVENT_0]->Socket, NULL, NULL)) == INVALID_SOCKET)
			{
				sprintf_s(Str, sizeof(Str), "accept() failed with error %d", WSAGetLastError());
				pLB->AddString(Str);
				break;
			}

			// Слишком много сокетов. Закрываем соединение.
			if (EventTotal > WSA_MAXIMUM_WAIT_EVENTS)
			{
				sprintf_s(Str, sizeof(Str), "Too many connections - closing socket.");
				pLB->AddString(Str);
				closesocket(Accept);
				break;
			}

			CreateSocketInformation(Accept, Str, pLB);

			if (WSAEventSelect(Accept, EventArray[EventTotal - 1], FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
			{
				sprintf_s(Str, sizeof(Str), "WSAEventSelect()failed with error %d", WSAGetLastError());
				pLB->AddString(Str);
				return 1;
			}

			sprintf_s(Str, sizeof(Str), "Socket %d connected", Accept);
			pLB->AddString(Str);

			if (m_games.empty())
			{
				m_games.push_back({ Accept,NULL,0,NULL, NULL ,20 , 20 });
			}
			else if (m_games.back().player2 == NULL)
			{
				m_games.back().player2 = Accept;
				StartGame(m_games.back(), pLB);
			}
			else
			{
				m_games.push_back({ Accept,NULL,0, NULL, NULL ,20 , 20 });
			}

			sprintf_s(Str, sizeof(Str), "Player %d added to %d board ", Accept, m_games.size());
			pLB->AddString(Str);

		}

		// Пытаемся читать или писать данные, 
		// если произошло соответствующее событие

		if (NetworkEvents.lNetworkEvents & FD_READ || NetworkEvents.lNetworkEvents & FD_WRITE)
		{
			if (NetworkEvents.lNetworkEvents & FD_READ && NetworkEvents.iErrorCode[FD_READ_BIT] != 0)
			{
				sprintf_s(Str, sizeof(Str), "FD_READ failed with error %d", NetworkEvents.iErrorCode[FD_READ_BIT]);
				pLB->AddString(Str);
				break;
			}

			if (NetworkEvents.lNetworkEvents & FD_WRITE && NetworkEvents.iErrorCode[FD_WRITE_BIT] != 0)
			{
				sprintf_s(Str, sizeof(Str), "FD_WRITE failed with error %d", NetworkEvents.iErrorCode[FD_WRITE_BIT]);
				pLB->AddString(Str);
				break;
			}

			LPSOCKET_INFORMATION SocketInfo = SocketArray[Event - WSA_WAIT_EVENT_0];

			// Читаем данные только если приемный буфер пуст

			if (SocketInfo->BytesRECV == 0)
			{
				////////////////////////////////////////////////

				SocketInfo->DataBuf.buf = SocketInfo->Buffer;
				SocketInfo->DataBuf.len = DATA_BUFSIZE;

				////////////////////////////////////////////////

				Flags = 0;
				if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags, NULL, NULL) == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						sprintf_s(Str, sizeof(Str), "WSARecv()failed with error %d", WSAGetLastError());
						pLB->AddString(Str);
						FreeSocketInformation(Event - WSA_WAIT_EVENT_0, Str, pLB);
						return 1;
					}
				}
				else
				{

					SocketInfo->BytesRECV = RecvBytes;
					// Вывод сообщения, если требуется
					//////////////////////////////////////////////////////////////////////////////////////////////
					if (bPrint)
					{
						unsigned l = sizeof(Str) - 1;
						if (l > RecvBytes) l = RecvBytes;
						strncpy_s(Str, SocketInfo->Buffer, l);
						Str[l] = 0;
						pLB->AddString(Str);
					}
					//////////////////////////////////////////////////////////////////////////////////////////////

					//char temp[128];
					//sprintf_s(temp, sizeof(temp), "%d", RecvBytes);
					//AfxMessageBox(temp);

					if (RecvBytes == 1045)
					{
						AddInfo(SocketInfo, pLB);
					}
					else
					{
						PrepareMessage(SocketInfo, pLB);
					}
				}
				ZeroMemory(SocketInfo->Buffer, DATA_BUFSIZE);
				SocketInfo->BytesSEND = 0;
				SocketInfo->BytesRECV = 0;
			}
		}
		if (NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
			LPSOCKET_INFORMATION SocketInfo = SocketArray[Event - WSA_WAIT_EVENT_0];
			if (NetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
			{
				sprintf_s(Str, sizeof(Str), "FD_CLOSE failed with error %d", NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
				pLB->AddString(Str);
			}
			SendMsgtoOponent(SocketInfo, pLB);
			FreeGameInfo(SocketInfo);
			sprintf_s(Str, sizeof(Str), "Closing socket information %d", SocketArray[Event - WSA_WAIT_EVENT_0]->Socket);
			pLB->AddString(Str);

			FreeSocketInformation(Event - WSA_WAIT_EVENT_0, Str, pLB);
		}
	}
	return 0;
}

BOOL CreateSocketInformation(SOCKET s, char* Str, CListBox* pLB)
{
	LPSOCKET_INFORMATION SI;

	if ((EventArray[EventTotal] = WSACreateEvent()) ==
		WSA_INVALID_EVENT)
	{
		sprintf_s(Str, sizeof(Str), "WSACreateEvent() failed with error %d", WSAGetLastError());
		pLB->AddString(Str);
		return FALSE;
	}

	if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL)
	{
		sprintf_s(Str, sizeof(Str), "GlobalAlloc() failed with error %d", GetLastError());
		pLB->AddString(Str);
		return FALSE;
	}

	// Подготовка структуры SocketInfo для использования.
	SI->Socket = s;
	SI->BytesSEND = 0;
	SI->BytesRECV = 0;

	SocketArray[EventTotal] = SI;
	EventTotal++;
	return(TRUE);
}

void FreeSocketInformation(DWORD Event, char* Str, CListBox* pLB)
{
	LPSOCKET_INFORMATION SI = SocketArray[Event];
	DWORD i;

	closesocket(SI->Socket);
	GlobalFree(SI);
	WSACloseEvent(EventArray[Event]);

	// Сжатие массивов сокетов и событий

	for (i = Event; i < EventTotal; i++)
	{
		EventArray[i] = EventArray[i + 1];
		SocketArray[i] = SocketArray[i + 1];
	}

	EventTotal--;
}


void StartGame(Board board, CListBox* pLB)
{
	char* sendbuf = "0";
	char Str[256];
	int iResult;

	iResult = send(board.player1, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		sprintf_s(Str, sizeof(Str), "GlobalAlloc() failed with error %d", WSAGetLastError());
		pLB->AddString(Str);
		closesocket(board.player1);
		WSACleanup();
	}
	sendbuf = "1";
	iResult = send(board.player2, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		sprintf_s(Str, sizeof(Str), "GlobalAlloc() failed with error %d", WSAGetLastError());
		pLB->AddString(Str);
		closesocket(board.player2);
		WSACleanup();
	}
}

void AddInfo(LPSOCKET_INFORMATION socketInfo, CListBox* pLB)
{
	auto pFront = m_games.begin();
	for (int i = 0; i < m_games.size(); i++)
	{
		if (pFront->player1 == socketInfo->Socket)
		{
			pFront->player1Field = new int* [10];
			for (int i = 0; i < 10; i++)
			{
				pFront->player1Field[i] = new int[10];
				for (int j = 0; j < 10; j++) {
					if (socketInfo->Buffer[i * 10 + j] == '1')
						pFront->player1Field[i][j] = 1;
					else
						pFront->player1Field[i][j] = 0;
				}
			}

		}
		if (pFront->player2 == socketInfo->Socket)
		{
			pFront->player2Field = new int* [10];
			for (int i = 0; i < 10; i++)
			{
				pFront->player2Field[i] = new int[10];
				for (int j = 0; j < 10; j++) {
					if (socketInfo->Buffer[i * 10 + j] == '1')
						pFront->player2Field[i][j] = 1;
					else
						pFront->player2Field[i][j] = 0;
				}
			}
		}
		std::advance(pFront, 1);
	}

}

void PrepareMessage(LPSOCKET_INFORMATION socketInfo, CListBox* pLB)
{
	auto pFront = m_games.begin();
	for (int i = 0; i < m_games.size(); i++)
	{
		
		if (pFront->player1 == socketInfo->Socket)
		{
			int flag = IsHit(pFront->player2Field, socketInfo->Buffer);
			if (flag) {
				if (pFront->turn)
					pFront->player2GameOver -= 1;
				else
					pFront->player1GameOver -= 1;
			}
			else
			{
				if (pFront->turn)
					pFront->turn = 0;
				else
					pFront->turn = 1;
			}
			SendInfo(pFront->player2, socketInfo->Buffer, 1, pFront->turn, pLB);
			SendInfo(pFront->player1, socketInfo->Buffer, 0, pFront->turn, pLB);
			if (pFront->player1GameOver == 0)
			{
				SendGameOver(pFront->player1, 0, pLB);
				SendGameOver(pFront->player2, 1, pLB);
				m_games.erase(pFront);
			}
			else if (pFront->player2GameOver == 0)
			{
				SendGameOver(pFront->player1, 1, pLB);
				SendGameOver(pFront->player2, 0, pLB);
				m_games.erase(pFront);
			}
			break;
		}
		if (pFront->player2 == socketInfo->Socket)
		{
			int flag = IsHit(pFront->player1Field, socketInfo->Buffer);
			if (flag) {
				if (pFront->turn)
					pFront->player2GameOver -= 1;
				else
					pFront->player1GameOver -= 1;
			}
			else
			{
				if (pFront->turn)
					pFront->turn = 0;
				else
					pFront->turn = 1;
			}
			SendInfo(pFront->player1, socketInfo->Buffer, 0, pFront->turn, pLB);
			SendInfo(pFront->player2, socketInfo->Buffer, 1, pFront->turn, pLB);
			if (pFront->player1GameOver == 0)
			{
				SendGameOver(pFront->player1, 0, pLB);
				SendGameOver(pFront->player2, 1, pLB);
				m_games.erase(pFront);
			}
			else if (pFront->player2GameOver == 0)
			{
				SendGameOver(pFront->player1, 1, pLB);
				SendGameOver(pFront->player2, 0, pLB);
				m_games.erase(pFront);
			}
			break;
		}
		std::advance(pFront, 1);
	}
}

int IsHit(int** matr, char* pos)
{
	return matr[((int)pos[0] - (int)'0')][((int)pos[1] - (int)'0')] == 1;
}

void SendInfo(SOCKET socket, char* message, int player, int flag, CListBox* pLB)
{
	//char* sendbuf = "0";
	if (player == flag)
		message[2] = '0';
	else
		message[2] = '1';

	char Str[256];
	int iResult;
	iResult = send(socket, message, (int)strlen(message), 0);
	if (iResult == SOCKET_ERROR) {
		sprintf_s(Str, sizeof(Str), "GlobalAlloc() failed with error %d", WSAGetLastError());
		pLB->AddString(Str);
		closesocket(socket);
		WSACleanup();
	}
}

void SendGameOver(SOCKET socket, int win, CListBox* pLB)
{
	char message[DATA_BUFSIZE];
	char Str[256];
	int iResult;
	sprintf_s(message, sizeof(message), "E%d", win);
	iResult = send(socket, message, (int)strlen(message), 0);
	if (iResult == SOCKET_ERROR) {
		sprintf_s(Str, sizeof(Str), "GlobalAlloc() failed with error %d", WSAGetLastError());
		pLB->AddString(Str);
		closesocket(socket);
		WSACleanup();
	}
}

void SendMsgtoOponent(LPSOCKET_INFORMATION socketInfo, CListBox* pLB)
{
	auto pFront = m_games.begin();
	for (int i = 0; i < m_games.size(); i++)
	{
		if (m_games.empty())
			return;

		if (pFront->player1 == socketInfo->Socket)
		{
			SendGameOver(pFront->player2, 0, pLB);
			break;
		}
		if (pFront->player2 == socketInfo->Socket)
		{
			SendGameOver(pFront->player1, 0, pLB);
			break;
		}
		std::advance(pFront, 1);
	}
}

	void FreeGameInfo(LPSOCKET_INFORMATION socketInfo)
	{
		auto pFront = m_games.begin();
		for (int i = 0; i < m_games.size(); i++)
		{
			if (m_games.empty())
				return;

			if (pFront->player1 == socketInfo->Socket || pFront->player2 == socketInfo->Socket)
			{
				m_games.erase(pFront);
				break;
			}
			std::advance(pFront, 1);
		}
	}