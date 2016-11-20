#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include <math.h>


GLvoid DrawScene(GLvoid);
GLvoid Reshape(int w, int h);

SOCKET sock;

//ĳ���� ����ü
Player player_client;
Player_Socket player_socket;
Server_Player server_data;
int client_imei = -1;//���° Ŭ������ ����ϱ�

void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Keyboardup(unsigned char key, int x, int y);
void Motion(int x, int y);
void TimerFunction(int value);

void Target(int x, int y);//ī�޶� �������� �Լ�
void Keyinput(int key);//Ű���� �����Է��� ���� �Է�ó�� �Լ�
void drawHud();//HUD �׸��� �Լ�
void crashCheck();//�浹üũ

int Time = 10, Bullet_Time = 1;//Ÿ�̸� ���Žð�
int width, height;
char Input;

int rifleammo = 120, rifleload = 30, hp = 100;
char ammo[10], health[10];


//ĳ���� �� ī�޶� ���� ����
//float camxrotate = 0, camyrotate = -90;
float Viewx = 0, Viewy = 0, Viewz = -1000, Camx, Camy, Camz;
//float Charx = 0, Charz = 0, Chary = 0;
float nx = 0, ny = 0, bx = 0, by = 0;//���콺 �����ӿ� ���̴� �ӽú���

float gravity = 0;
int Charspeed = 8, Camdistance = 400, MouseSens = 25;
bool RotateCam = true, FirstPersonView = true;
bool Keybuffer[256];


//�ִϸ��̼� ����
int character_up_state = 2, character_down_state = CHARACTER_STAND, legtimer = 0;//up_state�� ��ü, down_state�� ��ü
float head_angle_x;//�Ӹ�ȸ��
float left_sholder_x, left_sholder_y, left_sholder_z, right_sholder_x, right_sholder_y, right_sholder_z, left_elbow_x, right_elbow_x;//��ȸ��
float left_leg_x, left_leg_y, left_knee_x, right_leg_x, right_leg_y, right_knee_x, left_leg_z, right_leg_z;//�ٸ�ȸ��
bool sight = false, jump = false, crouch = false;//������

												 //�浹üũ �Ÿ�
int crashdist = 20;

//�Ѿ˵�����
Bullet * Head = NULL;
Bullet * Temp = NULL;
Bullet * CTemp = NULL;
Bullet * DTemp = NULL;



// ���� ����
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

	//�������ӽõ�
	//sock = init_sock();
	//send_Player(sock, player);
	
	//�÷��̾��� �����͸� ���� �� init_sock���� ��� �����͸� �Ѱ��ָ� ���� ������?
	//Win_Sock.cpp���� ���Ḯ��Ʈ�� ���� �߰��� �����ϸ� �ɵ�
	//��â���� �ڵ� ������ �׽�Ʈ�غ���

	//������ �ʱ�ȭ �� ����
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //���÷��� ��� ����
	glutInitWindowPosition(500, 0); // ������ ��ġ ����
	glutInitWindowSize(800, 600); //������ ũ�� ����
	glutCreateWindow("ComputerGrapgics TermProject"); //������ ����(������ �̸�)

													  //���� ���� �ʱ�ȭ �Լ�
	glutSetCursor(GLUT_CURSOR_NONE);//���콺Ŀ�� ���ֱ�
	for (int i = 0; i < 256; i++)
		Keybuffer[i] = false;

	//�ʿ��� �ݹ� �Լ� ����

	glutDisplayFunc(DrawScene); //��� �Լ��� ����
	glutTimerFunc(Time, TimerFunction, 1);//Ÿ�̸� �ݹ� �Լ�

	glutPassiveMotionFunc(Motion);//���콺���
	glutMouseFunc(Mouse);//���콺�Է�
	glutMotionFunc(Motion);//Ŭ���ÿ��� �����̰Բ� ���콺��� ����

	glutKeyboardFunc(Keyboard);//Ű�����Է�
	glutKeyboardUpFunc(Keyboardup);//Ű���� ��ư�� ���� ��

	glutReshapeFunc(Reshape);//�ٽñ׸���
	glutMainLoop();//�̺�Ʈ ���� �����ϱ�
}

//������ ��� �Լ�
GLvoid DrawScene(GLvoid)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//��������'black'������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// ������ ������ ��ü�� ĥ�ϱ�

	glViewport(0, 0, width, height);//����Ʈ ����
	glMatrixMode(GL_PROJECTION);//�Ÿ����� �繰�� �������� �׸��� ���
	glLoadIdentity();//�ʱ�ȭ? ã�ƺ��� ��������
	gluPerspective(60.0f, (float)width / (float)height, 0.1, 10000.0);//�þ߰� + �׷��� �Ÿ�
	glMatrixMode(GL_MODELVIEW);//???������


	glLoadIdentity();//�ʱ�ȭ? ã�ƺ��� ��������

	if (FirstPersonView)
	{//1��Ī�϶�
		gluLookAt(player.socket.x, player.socket.y + 170, player.socket.z, player.socket.x + Viewx, player.socket.y + Viewy + 170, player.socket.z + Viewz, 0.0, 1.0, 0.0);
	}
	else
	{//3��Ī�϶�
		gluLookAt(player.socket.x + Camx, player.socket.y + Camy + 190, player.socket.z + Camz, player.socket.x + Viewx, player.socket.y + Viewy + 190, player.socket.z + Viewz, 0.0, 1.0, 0.0);
	}

	//������
	glEnable(GL_DEPTH_TEST);                              // ������ �� ����
	glEnable(GL_CULL_FACE);                               // �ĸ� ����


	//�ڽ��׸���
	glPushMatrix();//��
	glColor3f(255, 0, 0);
	glTranslatef(0, 0, 1000);
	glutSolidCube(200);
	glPopMatrix();

	glPushMatrix();//��
	glColor3f(255, 0, 0);
	glTranslatef(0, 0, -1000);
	glutSolidCube(200);
	glPopMatrix();

	glPushMatrix();//��
	glColor3f(0, 255, 0);
	glTranslatef(0, 1000, 0);
	glutSolidCube(200);
	glPopMatrix();

	glPushMatrix();//��
	glColor3f(0, 255, 0);
	glTranslatef(0, -1000, 0);
	glutSolidCube(200);
	glPopMatrix();

	glPushMatrix();//��
	glColor3f(0, 0, 255);
	glTranslatef(1000, 0, 0);
	glutSolidCube(200);
	glPopMatrix();

	glPushMatrix();//��
	glColor3f(0, 0, 255);
	glTranslatef(-1000, 0, 0);
	glutSolidCube(200);
	glPopMatrix();
	//�ڽ��׸��� ��


	//�÷��̾� �׸���
	glPushMatrix();//���ΰ� û�ϻ�
	glColor3f(0, 255, 255);
	glTranslatef(player.socket.x, player.socket.y, player.socket.z);
	glScalef(0.7f, 2.0f, 0.7f);
	glutSolidCube(100);
	glPopMatrix();
	//�÷��̾� �׸��� ����


	//�ٸ� ĳ���� �׷��� ��
	


	drawHud();//HUD �׸��� �κ� ���ο� push pop �˾Ƽ� ��

	glutSwapBuffers(); //ȭ�鿡 ����ϱ�
}//end of drawScene


GLvoid Reshape(int w, int h)
{
	//����Ʈ ��ȯ ����
	glViewport(0, 0, w, h);
	width = w;
	height = h;

	//���� ��� ���� �缳��
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Ŭ���� ���� ���� : ��������
	gluPerspective(60.0f, (float)w / (float)h, 0.1, 5000.0);

	//�� �� ��� ���� �缳��
	glMatrixMode(GL_MODELVIEW);

}

void Mouse(int button, int state, int x, int y)//���콺 Ŭ������ üũ
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//��Ŭ��
	{
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)//��Ŭ��
	{
		if (FirstPersonView) {//1��Ī�ϰ�� ������
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

void Motion(int x, int y)//���콺 �Է�(��ǥ��)�� ���ڸ��� Target���� �Ѱ��ְ� Target���� Viewportó��
{
	Target(x, y);

}

void Keyboard(unsigned char key, int x, int y)
{
	Keybuffer[key] = true;

	switch (key)//
	{
	case ' '://�����̽���
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
			glutWarpPointer(400, 300);//�ػ󵵿� ���� ���������� �ٲ��

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
