#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdlib.h>
#include <atlstr.h> // cstring
#include <Windows.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE    1024
#define MAX_Client 4

bool client[MAX_Client] = { false }; //클라이언트가 들어오는 대로 처리한다.


typedef struct Player_Socket {
	// 전송을 위해 사용
	float x = -1000, y = -1000, z = -1000;
	float camxrotate = -1000, camyrotate = -1000;
	/*
	동작상태  : 걷기, 정지, 점프

	*/
}Player_Socket;

typedef struct Player {
	//클라 내에서만 사용
	Player_Socket socket;
}Player;

typedef struct Server_Player {
	//플레이어 데이터를 묶어놓은것
	Player_Socket player[MAX_Client];
}Server_Player;



Server_Player server_data;


// 소켓 함수 오류 출력 후 종료
void err_quit( char *msg ) {
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

// 소켓 함수 오류 출력
void err_display( char *msg ) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );
	printf( "[%s] %s", msg, (char *)lpMsgBuf );
	LocalFree( lpMsgBuf );
}

// 사용자 정의 데이터 수신 함수
int recvn( SOCKET s, char *buf, int len, int flags ) {
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
	int client_imei = -1;
	char buf[BUFSIZE + 1];

	// 클라이언트 정보 얻기
	addrlen = sizeof( clientaddr );
	getpeername( client_sock, (SOCKADDR *)&clientaddr, &addrlen );
	for ( int i = 0; i < MAX_Client; ++i ) {
		if ( client[i] == false ) {
			client[i] = true;
			client_imei = i;
			printf( "Client ID : %d\n", client_imei );
			break;
		}
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 클라가 현재 서버에서 ID가 몇번인지 확인을 시켜준다.
	char client_imei_c[10];
	itoa( client_imei, client_imei_c, 10 );
	len = sizeof( client_imei_c );
	retval = send( client_sock, (char *)&len, sizeof( int ), 0 );
	if ( retval == SOCKET_ERROR ) {
		err_display( "send()" );
		//exit( 1 );
	}

	// 데이터 보내기( 구조체 데이터를 보낸다. )
	retval = send( client_sock, (char*)&client_imei_c, sizeof( client_imei_c ), 0 );
	if ( retval == SOCKET_ERROR ) {
		err_display( "send()" );
		//exit( 1 );
	}

	if ( client_imei == -1 ) {
		// 인원이 꽉참 더이상 못들어 온다.
		closesocket( client_sock );
		printf( "클라이언트 종료: IP 주소=%s, 포트 번호=%d [인원 초과]\n", inet_ntoa( clientaddr.sin_addr ), ntohs( clientaddr.sin_port ) );
		return 0;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	while ( 1 ) {
		retval = recvn( client_sock, (char *)&len, sizeof( int ), 0 ); // 데이터 받기(고정 길이)
		if ( retval == SOCKET_ERROR ) {
			err_display( "recv()" );
			break;
		}
		else if ( retval == 0 )
			break;
		//printf( "데이터 크기 : %d\n", len );

		int GetSize;
		char suBuffer[BUFSIZE];
		Player *player;
		GetSize = recv( client_sock, suBuffer, len, 0 );

		suBuffer[GetSize] = '\0';
		player = (Player*)suBuffer;

		// 데이터 받기(가변 길이)
		if ( retval == SOCKET_ERROR ) {
			err_display( "recv()" );
			break;
		}
		else if ( retval == 0 )
			break;
		//printf( "------------------------------------------\n" );
		//printf( "Client ID : %d\n", client_imei );
		//printf( "Camxrotate : %f\nCamyrotate : %f\nPlayer->X : %f\nPlayer->Y : %f\nPlayer->Z : %f\n", player->camxrotate, player->camyrotate, player->socket.x, player->socket.y, player->socket.z );
		//printf( "------------------------------------------\n" );

		server_data.player[client_imei].camxrotate = player->socket.camxrotate;
		server_data.player[client_imei].camyrotate = player->socket.camyrotate;
		server_data.player[client_imei].x = player->socket.x;
		server_data.player[client_imei].y = player->socket.y;
		server_data.player[client_imei].z = player->socket.z;

		//printf( "Xrotate : %f\n", server_data.player[0].camxrotate );

		//int retval;
		// 데이터 보내기( 구조체 크기를 먼저 보낸다. )
		int len = sizeof( server_data );
		retval = send( client_sock, (char *)&len, sizeof( int ), 0 );
		if ( retval == SOCKET_ERROR ) {
			err_display( "send()" );
			//exit( 1 );
		}

		// 데이터 보내기( 구조체 데이터를 보낸다. )
		retval = send( client_sock, (char*)&server_data, sizeof( Server_Player ), 0 );
		if ( retval == SOCKET_ERROR ) {
			err_display( "send()" );
			//exit( 1 );
		}


	}
	closesocket( client_sock );
	server_data.player[client_imei].camxrotate = -1000.0f;
	server_data.player[client_imei].camyrotate = -1000.0f;
	server_data.player[client_imei].x = -1000.0f;
	server_data.player[client_imei].y = -1000.0f;
	server_data.player[client_imei].z = -1000.0f;
	client[client_imei] = false;

	return 0;
}

int main( int argc, char *argv[] ) {
	int retval;
	// 윈속 초기화
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

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	char hostname[512];
	hostent* localhost = gethostbyname( "localhost" );
	hostent* host = gethostbyname( localhost->h_name );
	for ( int i = 0; host->h_addr_list[i]; i++ ) {
		in_addr in;
		in.S_un.S_addr = *(ULONG*)host->h_addr_list[i];
		printf( "서버 IP : %s\n", inet_ntoa( in ) );
	}
	printf( "서버 Port : %d\n", SERVERPORT );
	printf( "Server is Ready..!\n\n" );

	while ( 1 ) {
		// accept()
		addrlen = sizeof( clientaddr );
		client_sock = accept( listen_sock, (SOCKADDR *)&clientaddr, &addrlen );
		if ( client_sock == INVALID_SOCKET ) {
			err_display( "accept()" );
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf( "클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa( clientaddr.sin_addr ), ntohs( clientaddr.sin_port ) );

		// 스레드 생성
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
	
	// 윈속 종료
	WSACleanup();
	return 0;
}