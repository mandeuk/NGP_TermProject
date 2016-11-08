#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdlib.h>
#include <atlstr.h> // cstring
#include <Windows.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE    1024

typedef struct Player_Socket
{
	float x, y, z;
}Player_Socket;

typedef struct Player
{
	float camxrotate, camyrotate;
	Player_Socket socket;
}Player;


// ���� �Լ� ���� ��� �� ����
void err_quit( char *msg )
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR );
	LocalFree( lpMsgBuf );
	exit( 1 );
}

// ���� �Լ� ���� ���
void err_display( char *msg )
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );
	printf( "[%s] %s", msg, (char *)lpMsgBuf );
	LocalFree( lpMsgBuf );
}

// ����� ���� ������ ���� �Լ�
int recvn( SOCKET s, char *buf, int len, int flags )
{
	int received;
	char *ptr = buf;
	int left = len;

	while ( left > 0 ) {
		received = recv( s, ptr, left, flags );
		if ( received == SOCKET_ERROR )
			return SOCKET_ERROR;
		else if ( received == 0 )
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}


DWORD WINAPI ProcessClient( LPVOID arg ) {
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	int len;
	char buf[BUFSIZE + 1];

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof( clientaddr );
	getpeername( client_sock, (SOCKADDR *)&clientaddr, &addrlen );

	while ( 1 ) {
		retval = recvn( client_sock, (char *)&len, sizeof( int ), 0 ); // ������ �ޱ�(���� ����)
		if ( retval == SOCKET_ERROR ) {
			err_display( "recv()" );
			break;
		}
		else if ( retval == 0 )
			break;
		printf( "������ ũ�� : %d\n", len );
		


//		Player *player;

		//retval = recvn( client_sock, (char*)&player, sizeof(Player), 0 );

		int GetSize;
		char suBuffer[BUFSIZE];
		Player *player;
		GetSize = recv( client_sock, suBuffer, len, 0 );

		suBuffer[GetSize] = '\0';
		player = (Player*)suBuffer;




		// ������ �ޱ�(���� ����)
		if ( retval == SOCKET_ERROR ) {
			err_display( "recv()" );
			break;
		}
		else if ( retval == 0 )
			break;
		printf( "Camxrotate : %f\nCamyrotate : %f\nPlayer->X : %f\nPlayer->Y : %f\nPlayer->Z : %f\n", player->camxrotate, player->camyrotate, 
			player->socket.x, player->socket.y, player->socket.z );


	}
	closesocket( client_sock );

	return 0;
}

int main( int argc, char *argv[] ) {
	int retval;
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if ( WSAStartup( MAKEWORD( 2, 2 ), &wsa ) != 0 )
		return 1;

	// socket()
	SOCKET listen_sock = socket( AF_INET, SOCK_STREAM, 0 );
	if ( listen_sock == INVALID_SOCKET ) err_quit( "socket()" );

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory( &serveraddr, sizeof( serveraddr ) );
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl( INADDR_ANY );
	serveraddr.sin_port = htons( SERVERPORT );
	retval = bind( listen_sock, (SOCKADDR *)&serveraddr, sizeof( serveraddr ) );
	if ( retval == SOCKET_ERROR ) err_quit( "bind()" );

	// listen()
	retval = listen( listen_sock, SOMAXCONN );
	if ( retval == SOCKET_ERROR ) err_quit( "listen()" );

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	//char hostname[512];
	//hostent* localhost = gethostbyname( "localhost" );
	//hostent* host = gethostbyname( localhost->h_name );
	//for ( int i = 0; host->h_addr_list[i]; i++ ) {
	//	in_addr in;
	//	in.S_un.S_addr = *(ULONG*)host->h_addr_list[i];
	//	printf( "���� IP : %s\n", inet_ntoa( in ) );
	//}
	//printf( "���� Port : %d\n", SERVERPORT );
	printf( "Server is Ready..!\n\n" );

	while ( 1 ) {
		// accept()
		addrlen = sizeof( clientaddr );
		client_sock = accept( listen_sock, (SOCKADDR *)&clientaddr, &addrlen );
		if ( client_sock == INVALID_SOCKET ) {
			err_display( "accept()" );
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf( "\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa( clientaddr.sin_addr ), ntohs( clientaddr.sin_port ) );

		// ������ ����
		hThread = CreateThread( NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL );
		if ( hThread == NULL ) {
			closesocket( client_sock );
		}
		else {
			CloseHandle( hThread );
		}

	}
	// closesocket()
	closesocket( listen_sock );

	// ���� ����
	WSACleanup();
	return 0;
}