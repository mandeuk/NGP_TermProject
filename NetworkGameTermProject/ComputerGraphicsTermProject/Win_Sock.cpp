#include "main.h"

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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

SOCKET init_sock() {
	int retval;
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if ( WSAStartup( MAKEWORD( 2, 2 ), &wsa ) != 0 ) {
	}
	// socket()
	SOCKET sock = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sock == INVALID_SOCKET ) err_quit( "socket()" );

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory( &serveraddr, sizeof( serveraddr ) );
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr( SERVERIP );
	serveraddr.sin_port = htons( SERVERPORT );
	retval = connect( sock, (SOCKADDR *)&serveraddr, sizeof( serveraddr ) );
	if ( retval == SOCKET_ERROR ) err_quit( "connect()" );

	printf( "[�˸�] %s:%d ���������� ���� �Ǿ����ϴ�..!\n", SERVERIP, SERVERPORT );
	return sock;
}

void send_Player(SOCKET sock, Player player) {
	int retval;
	// ������ ������( ����ü ũ�⸦ ���� ������. )
	int len = sizeof(player);
	retval = send( sock, (char *)&len, sizeof( int ), 0 );
	if ( retval == SOCKET_ERROR ) {
		err_display( "send()" );
		exit( 1 );
	}

	// ������ ������( ����ü �����͸� ������. )
	retval = send( sock, (char*)&player, sizeof(Player), 0 );
	if ( retval == SOCKET_ERROR ) {
		err_display( "send()" );
		exit( 1 );
	}
}