#ifndef __MAIN_LOAD_H
#define __MAIN_LOAD_H

#pragma comment(lib, "ws2_32")

#include <GL/glut.h> // include glut.h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>
#include <windows.h>


#include "OpenGL_Aux/GLAUX.H"
// OpenGL Aux ������� ���� ��� �߰�
#pragma comment(lib, "./OpenGL_Aux/GLAUX")
// OpenGL Aux ������� ���� ��� �߰� ����
#pragma comment(lib, "winmm.lib") 
// ��������� �����ϱ� ���� ��� �߰�.


//�ð����� ��ũ�� http://egloos.zum.com/flowismylife/v/2761189
#define CHECK_TIME_START __int64 freq, start, end; if (QueryPerformanceFrequency((_LARGE_INTEGER*)&freq)) {QueryPerformanceCounter((_LARGE_INTEGER*)&start);
#define CHECK_TIME_END(a,b) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((double)(end - start)/freq*1000); b=TRUE; } else {b=FALSE;}


//�������
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    1024
#define MAX_Client 4

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
	float camxrotate, camyrotate;
	bool live;
	bool team;
}Player_Socket;

typedef struct Server_Player
{
	Player_Socket Players[MAX_Client];
}Player_Other;

typedef struct Player
{
	float Viewx, Viewy, Viewz;
	float Camx, Camy, Camz;
}Player;



//UI �׸���
void drawHud();

//��������
void err_quit( char *msg );
void err_display( char *msg );
SOCKET init_sock();
int get_ClientID(SOCKET sock);
void send_Player(SOCKET sock, Player_Socket player);
Server_Player recv_Player(SOCKET sock);
int get_ClientTeam(SOCKET sock);

//�� �׸���
void DrawMap();

//�ڽ��׸���
void drawBoxFront(int, bool, GLuint);//�ڽ� �׸��� �Լ�
void drawBoxBack(int, bool, GLuint);
void drawBoxTop(int, bool, GLuint);
void drawBoxBottom(int, bool, GLuint);
void drawBoxRight(int, bool, GLuint);
void drawBoxLeft(int, bool, GLuint);

//ĳ���� �׸��� ����
static GLuint character_head_object[6];
static GLuint character_body_object[6];
static GLuint character_arm_top_object[6];
static GLuint character_arm_bottom_object[6];
static GLuint character_leg_top_object[6];
static GLuint character_leg_bottom_object[6];

//���� �׸��� ����
static GLuint zombie_head_object[6];
static GLuint zombie_body_object[6];
static GLuint zombie_arm_top_object[6];
static GLuint zombie_arm_bottom_object[6];
static GLuint zombie_leg_top_object[6];
static GLuint zombie_leg_bottom_object[6];

//�ؽ��� �ҷ�����
void init_Character_Texture();
void head_Texture( GLuint object[] );
void body_Texture( GLuint object[] );
void arm_Texture( GLuint object[], GLuint object2[] );
void leg_Texture( GLuint object[], GLuint object2[] );

void init_Zombie_Texture();
void zombie_head_Texture(GLuint[]);
void zombie_body_Texture(GLuint[]);
void zombie_arm_Texture(GLuint[], GLuint[]);
void zombie_leg_Texture(GLuint[], GLuint[]);

void drawCharacter(Player_Socket *player_socket, bool sight, int character_up_state, float right_leg_x, float right_leg_z, float left_leg_x, float left_leg_z, float right_knee_x, float left_knee_x);
void drawZombie(Player_Socket *player_socket, bool sight, int zombie_up_state, float right_leg_x, float right_leg_z, float left_leg_x, float left_leg_z, float right_knee_x, float left_knee_x);


//BMP���� �ҷ�����
void Load_TextureBMP(GLuint object[], int num, char *imagedata);
AUX_RGBImageRec *LoadBMP(char *Filename);


#endif