#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include <math.h>


GLvoid DrawScene(GLvoid);
GLvoid Reshape(int w, int h);

SOCKET sock;

//캐릭터 구조체
Player player_client;
Player_Socket player_socket;
Server_Player server_data;
int client_imei = -1;//몇번째 클라인지 기억하기

void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Keyboardup(unsigned char key, int x, int y);
void Motion(int x, int y);
void TimerFunction(int value);

void Target(int x, int y);//카메라 시점관련 함수
void Keyinput(int key);//키보드 동시입력을 위한 입력처리 함수
void drawHud();//HUD 그리는 함수
void crashCheck();//충돌체크

int Time = 10, Bullet_Time = 1;//타이머 갱신시간
int width, height;
char Input;

int rifleammo = 120, rifleload = 30, hp = 100;
char ammo[10], health[10];


//캐릭터 및 카메라 관련 변수
//float camxrotate = 0, camyrotate = -90;
float Viewx = 0, Viewy = 0, Viewz = -1000, Camx, Camy, Camz;
//float Charx = 0, Charz = 0, Chary = 0;
float nx = 0, ny = 0, bx = 0, by = 0;//마우스 움직임에 쓰이는 임시변수

float gravity = 0;
int Charspeed = 8, Camdistance = 400, MouseSens = 25;
bool RotateCam = true, FirstPersonView = true;
bool Keybuffer[256];


//애니메이션 변수
int character_up_state = 2, character_down_state = CHARACTER_STAND, legtimer = 0;//up_state는 상체, down_state는 하체
float head_angle_x;//머리회전
float left_sholder_x, left_sholder_y, left_sholder_z, right_sholder_x, right_sholder_y, right_sholder_z, left_elbow_x, right_elbow_x;//팔회전
float left_leg_x, left_leg_y, left_knee_x, right_leg_x, right_leg_y, right_knee_x, left_leg_z, right_leg_z;//다리회전
bool sight = false, jump = false, crouch = false;//정조준

												 //충돌체크 거리
int crashdist = 20;

//총알데이터
Bullet * Head = NULL;
Bullet * Temp = NULL;
Bullet * CTemp = NULL;
Bullet * DTemp = NULL;



// 조명 설정
/*
bool turn_Light = true;
void show_Light(){
GLfloat AmbientLight[] = { 0.1, 0.1, 0.1, 1.0f };
GLfloat DiffuseLight[] = { 0.8, 0.8, 0.8, 1.0f };
GLfloat lightpos[] = { Charx, 200, Charz, 1 };
GLfloat SpecularLight[] = { 1.0, 1.0, 1.0, 1.0 };
if (sight)
{
glLightf(GL_LIGHT0, GL_SPOT_DIRECTION, (0, 0, 1));
glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, (30.0));
}
else{
glLightf(GL_LIGHT0, GL_SPOT_DIRECTION, (0, 0, 1));
glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, (70.0));
}
glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbientLight);
glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
}
*/


void main()
{
	srand((unsigned)time(NULL));

	//서버접속시도
	//sock = init_sock();
	//send_Player(sock, player);
	
	//플레이어의 데이터를 받을 때 init_sock으로 헤더 포인터만 넘겨주면 되지 않을까?
	//Win_Sock.cpp에서 연결리스트의 삭제 추가를 구현하면 될듯
	//권창에게 코드 받으면 테스트해보자

	//윈도우 초기화 및 생성
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //디스플레이 모드 설정
	glutInitWindowPosition(500, 0); // 윈도우 위치 지정
	glutInitWindowSize(800, 600); //윈도우 크기 지정
	glutCreateWindow("ComputerGrapgics TermProject"); //윈도우 생성(윈도우 이름)

													  //상태 변수 초기화 함수
	glutSetCursor(GLUT_CURSOR_NONE);//마우스커서 없애기
	for (int i = 0; i < 256; i++)
		Keybuffer[i] = false;

	//필요한 콜백 함수 설정

	glutDisplayFunc(DrawScene); //출력 함수의 지정
	glutTimerFunc(Time, TimerFunction, 1);//타이머 콜백 함수

	glutPassiveMotionFunc(Motion);//마우스모션
	glutMouseFunc(Mouse);//마우스입력
	glutMotionFunc(Motion);//클릭시에도 움직이게끔 마우스모션 적용

	glutKeyboardFunc(Keyboard);//키보드입력
	glutKeyboardUpFunc(Keyboardup);//키보드 버튼을 뗐을 때

	glutReshapeFunc(Reshape);//다시그리기
	glutMainLoop();//이벤트 루프 실행하기
}

//윈도우 출력 함수
GLvoid DrawScene(GLvoid)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//바탕색을'black'로지정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 설정된 색으로 전체를 칠하기

	glViewport(0, 0, width, height);//뷰포트 설정
	glMatrixMode(GL_PROJECTION);//거리별로 사물이 가려지게 그리는 모드
	glLoadIdentity();//초기화? 찾아보고 이해하자
	gluPerspective(60.0f, (float)width / (float)height, 0.1, 10000.0);//시야각 + 그려줄 거리
	glMatrixMode(GL_MODELVIEW);//???뭐였지


	glLoadIdentity();//초기화? 찾아보고 이해하자

	if (FirstPersonView)
	{//1인칭일때
		gluLookAt(player.socket.x, player.socket.y + 170, player.socket.z, player.socket.x + Viewx, player.socket.y + Viewy + 170, player.socket.z + Viewz, 0.0, 1.0, 0.0);
	}
	else
	{//3인칭일때
		gluLookAt(player.socket.x + Camx, player.socket.y + Camy + 190, player.socket.z + Camz, player.socket.x + Viewx, player.socket.y + Viewy + 190, player.socket.z + Viewz, 0.0, 1.0, 0.0);
	}

	//조명설정
	glEnable(GL_DEPTH_TEST);                              // 가려진 면 제거
	glEnable(GL_CULL_FACE);                               // 후면 제거


	//박스그리기
	glPushMatrix();//앞
	glColor3f(255, 0, 0);
	glTranslatef(0, 0, 1000);
	glutSolidCube(200);
	glPopMatrix();

	glPushMatrix();//뒤
	glColor3f(255, 0, 0);
	glTranslatef(0, 0, -1000);
	glutSolidCube(200);
	glPopMatrix();

	glPushMatrix();//상
	glColor3f(0, 255, 0);
	glTranslatef(0, 1000, 0);
	glutSolidCube(200);
	glPopMatrix();

	glPushMatrix();//하
	glColor3f(0, 255, 0);
	glTranslatef(0, -1000, 0);
	glutSolidCube(200);
	glPopMatrix();

	glPushMatrix();//좌
	glColor3f(0, 0, 255);
	glTranslatef(1000, 0, 0);
	glutSolidCube(200);
	glPopMatrix();

	glPushMatrix();//우
	glColor3f(0, 0, 255);
	glTranslatef(-1000, 0, 0);
	glutSolidCube(200);
	glPopMatrix();
	//박스그리기 끝


	//플레이어 그리기
	glPushMatrix();//주인공 청록색
	glColor3f(0, 255, 255);
	glTranslatef(player.socket.x, player.socket.y, player.socket.z);
	glScalef(0.7f, 2.0f, 0.7f);
	glutSolidCube(100);
	glPopMatrix();
	//플레이어 그리기 종료


	//다른 캐릭터 그려야 함
	


	drawHud();//HUD 그리는 부분 내부에 push pop 알아서 함

	glutSwapBuffers(); //화면에 출력하기
}//end of drawScene


GLvoid Reshape(int w, int h)
{
	//뷰포트 변환 설정
	glViewport(0, 0, w, h);
	width = w;
	height = h;

	//투영 행렬 스택 재설정
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//클리핑 공간 설정 : 원근투영
	gluPerspective(60.0f, (float)w / (float)h, 0.1, 5000.0);

	//모델 뷰 행렬 스택 재설정
	glMatrixMode(GL_MODELVIEW);

}

void Mouse(int button, int state, int x, int y)//마우스 클릭관련 체크
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//좌클릭
	{
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)//우클릭
	{
		if (FirstPersonView) {//1인칭일경우 정조준
			if (sight) {
				sight = false;
				//turn_Light = true;
			}
			else {
				sight = true;
				//turn_Light = true;
			}
		}
		else {}
	}
}//end of Mouse

void Motion(int x, int y)//마우스 입력(좌표값)을 받자마자 Target으로 넘겨주고 Target에서 Viewport처리
{
	Target(x, y);

}

void Keyboard(unsigned char key, int x, int y)
{
	Keybuffer[key] = true;

	switch (key)//
	{
	case ' '://스페이스바
		if (jump) {}
		else
		{
			jump = true;
			gravity = 15;
		}
		break;
	}
}//end of Keyboard

void Keyboardup(unsigned char key, int x, int y)
{
	Keybuffer[key] = false;
	if (!Keybuffer['w'] && !Keybuffer['a'] && !Keybuffer['s'] && !Keybuffer['d'])
		character_down_state = CHARACTER_STAND;
	if (Keybuffer['x'] == false)
	{
		Charspeed = 8;
		crouch = false;
	}
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1:
		if (RotateCam)
			glutWarpPointer(400, 300);//해상도에 따라 유동적으로 바뀌도록

		player.socket.y += gravity;
		if (jump)
		{
			gravity -= 0.5;
			character_down_state = CHARACTER_CROUCH_JUMP;
		}

		if (player.socket.y < 0)
		{
			player.socket.y = 0;
			gravity = 0;
			character_down_state = CHARACTER_STAND;
			jump = false;
		}

		for (int i = 0; i < 256; i++)
			if (Keybuffer[i])
				Keyinput(i);
		glutPostRedisplay();
		glutTimerFunc(Time, TimerFunction, 1);
		break;
	}


}//end of TimerFunction



void Target(int x, int y)
{
	if (RotateCam)
	{
		nx = 400 - x;
		ny = 300 - y;

		player.camxrotate = player.camxrotate + (nx / MouseSens);
		player.camyrotate = player.camyrotate + (ny / MouseSens);
		left_sholder_x = player.camyrotate;
		right_sholder_x = player.camyrotate;

		Viewz = 10000 * sin((player.camyrotate)* 3.141592 / 180) * cos((player.camxrotate)* 3.141592 / 180);
		Viewx = 10000 * sin((player.camyrotate)* 3.141592 / 180) * sin((player.camxrotate)* 3.141592 / 180);
		Viewy = 10000 * cos((player.camyrotate)* 3.141592 / 180);

		Camz = -Camdistance * sin((player.camyrotate)* 3.141592 / 180) * cos((player.camxrotate + 15)* 3.141592 / 180);
		Camx = -Camdistance * sin((player.camyrotate)* 3.141592 / 180) * sin((player.camxrotate + 15)* 3.141592 / 180);
		Camy = -Camdistance * cos((player.camyrotate - 15)* 3.141592 / 180);


		if (player.camxrotate <= -360 || player.camxrotate >= 360)
			player.camxrotate = 0;

		if (player.camyrotate < -179)
			player.camyrotate = -179;
		else if (player.camyrotate > -1)
			player.camyrotate = -1;
		bx = nx;
		by = ny;
	}
}



void Keyinput(int key)
{
	if (key == 'w')
	{
		player.socket.x += Charspeed * cos((-player.camxrotate - 90) * 3.141592 / 180);
		player.socket.z += Charspeed * sin((-player.camxrotate - 90) * 3.141592 / 180);
		if (jump)
			character_down_state = CHARACTER_CROUCH_JUMP;
		else
			character_down_state = CHARACTER_WALK;
	}
	else if (key == 's')
	{
		player.socket.x -= Charspeed * cos((-player.camxrotate - 90) * 3.141592 / 180);
		player.socket.z -= Charspeed * sin((-player.camxrotate - 90) * 3.141592 / 180);
		if (jump)
			character_down_state = CHARACTER_CROUCH_JUMP;
		else
			character_down_state = CHARACTER_WALK;
	}
	if (key == 'a')
	{
		player.socket.x -= Charspeed * cos((-player.camxrotate) * 3.141592 / 180);
		player.socket.z -= Charspeed * sin((-player.camxrotate) * 3.141592 / 180);
		if (jump)
			character_down_state = CHARACTER_CROUCH_JUMP;
		else
			character_down_state = CHARACTER_WALK;
	}
	else if (key == 'd')
	{
		player.socket.x += Charspeed * cos((-player.camxrotate) * 3.141592 / 180);
		player.socket.z += Charspeed * sin((-player.camxrotate) * 3.141592 / 180);
		if (jump)
			character_down_state = CHARACTER_CROUCH_JUMP;
		else
			character_down_state = CHARACTER_WALK;
	}


	switch (key)
	{
	case 'c':
		glutSetCursor(GLUT_CURSOR_NONE);
		if (RotateCam)
			RotateCam = false;
		else
			RotateCam = true;
		Keybuffer[key] = false;
		break;
	case 'f':
		if (FirstPersonView) {
			FirstPersonView = false;
			//sight = false;
			//turn_Light = true;
		}
		else {
			FirstPersonView = true;
			//turn_Light = true;
		}
		Keybuffer[key] = false;
		break;

	case 27:
		exit(0);
		break;
	}
}
