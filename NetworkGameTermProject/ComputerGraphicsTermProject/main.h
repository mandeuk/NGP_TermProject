#pragma once

#pragma comment(lib, "ws2_32")

#include <GL/glut.h> // include glut.h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>
#include <windows.h>

//�������
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define DataBufSize    1024

//��ü �ȵ���
#define CHARACTER_NOWEAPON 0
#define CHARACTER_PISTOL 1
//#define CHARACTER_RIFLE 2

//��ü ������
#define CHARACTER_STAND 0
#define CHARACTER_WALK 1
#define CHARACTER_CROUCH_JUMP 3

typedef struct Bullet
{
	float x, y, z;
	float fx, fy, fz;
	Bullet * next;
}Bullet;

typedef struct Player_Socket
{
	float x, y, z;
}Player_Socket;

typedef struct Player_Other
{
	float x, y, z;
	Player_Other * next;
}Player_Other;

typedef struct Player
{
	float camxrotate, camyrotate;
	float Viewx, Viewy, Viewz;
	float Camx, Camy, Camz;

	Player_Socket socket;
}Player;



//UI �׸���
void drawHud();

//��������
void err_quit( char *msg );
void err_display( char *msg );
SOCKET init_sock();
void send_Player( SOCKET sock, Player player );