#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include <math.h>


GLvoid DrawScene(GLvoid);
GLvoid Reshape(int w, int h);

//��������
SOCKET sock;


//ĳ���� ����ü
Player player_client;
Player_Socket player_socket;
Server_Player server_data;
int client_imei = -1;//���° Ŭ������ ����ϱ�


//�ʼ� �Լ���
void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Keyboardup(unsigned char key, int x, int y);
void Motion(int x, int y);
void TimerFunction(int value);

void Target(int x, int y);//ī�޶� �������� �Լ�
void Keyinput(int key);//Ű���� �����Է��� ���� �Է�ó�� �Լ�
void drawHud();//HUD �׸��� �Լ�
//void crashCheck();//�浹üũ

//Ÿ�̸� ���Žð�
int Time = 10, Bullet_Time = 1;
float fTime;
bool err;

//�ػ�
int width, height;
char Input;

//UI���� ������
int rifleammo = 120, rifleload = 30, hp = 100;
char ammo[10], health[10], PressSpace[50];
int len;

//ĳ���� �� ī�޶� ���� ����
float nx = 0, ny = 0, bx = 0, by = 0;//���콺 �����ӿ� ���̴� �ӽú���
float gravity = 0, Charspeed = 0.7f;
int  Camdistance = 400, MouseSens = 25;
bool RotateCam = true, FirstPersonView = true;
bool Keybuffer[256];//�����Է¿� ���Ǵ� ����

//�������� ���� �Լ�
bool GameStart = false;


//�ִϸ��̼� ����
int character_up_state = 1, character_down_state = CHARACTER_STAND, legtimer = 0;//up_state�� ��ü, down_state�� ��ü
float head_angle_x;//�Ӹ�ȸ��
float left_leg_x, left_leg_y, left_knee_x, right_leg_x, right_leg_y, right_knee_x, left_leg_z, right_leg_z;//�ٸ�ȸ��
bool sight = false, jump = false, crouch = false;//������, ��������, �ɱ����
//float left_sholder_x, left_sholder_y, left_sholder_z, right_sholder_x, right_sholder_y, right_sholder_z, left_elbow_x, right_elbow_x;//��ȸ��


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



DWORD WINAPI SR_Thread(LPVOID arg) {
	while (1) {
		send_Player(sock, player_socket);
		server_data = recv_Player(sock);
		//printf( "Client ID %d : %f\n", client_imei, server_data.player[client_imei].camxrotate );
	}
	return 0;
}


void main()
{
	srand((unsigned)time(NULL));

	//�������ӽõ�
	//HANDLE hThread;
	//sock = init_sock();
	//client_imei = get_ClientID(sock);
	//player_socket.team = get_ClientTeam(sock);
	//hThread = CreateThread(NULL, 0, SR_Thread, 0, 0, NULL);
	
	//�÷��̾��� �����͸� ���� �� init_sock���� ��� �����͸� �Ѱ��ָ� ���� ������?
	//Win_Sock.cpp���� ���Ḯ��Ʈ�� ���� �߰��� �����ϸ� �ɵ�
	//��â���� �ڵ� ������ �׽�Ʈ�غ���

	//������ �ʱ�ȭ �� ����
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //���÷��� ��� ����
	glutInitWindowPosition(500, 0); // ������ ��ġ ����
	glutInitWindowSize(800, 600); //������ ũ�� ����
	glutCreateWindow("ComputerGrapgics TermProject"); //������ ����(������ �̸�)
	init_Character_Texture();
	init_Zombie_Texture();
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
	CHECK_TIME_START;//�ð����� ����

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//��������'black'������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// ������ ������ ��ü�� ĥ�ϱ�

	glViewport(0, 0, width, height);//����Ʈ ����
	glMatrixMode(GL_PROJECTION);//�Ÿ����� �繰�� �������� �׸��� ���
	glLoadIdentity();//�ʱ�ȭ? ã�ƺ��� ��������
	gluPerspective(60.0f, (float)width / (float)height, 0.1, 10000.0);//�þ߰� + �׷��� �Ÿ�
	glMatrixMode(GL_MODELVIEW);//???������

	glLoadIdentity();//�ʱ�ȭ? ã�ƺ��� ��������


	if (GameStart)
	{
		//1��Ī�϶�
		if (FirstPersonView){	gluLookAt(player_socket.x, player_socket.y + 240, player_socket.z, player_socket.x + player_client.Viewx, player_socket.y + player_client.Viewy + 170, player_socket.z + player_client.Viewz, 0.0, 1.0, 0.0);	}
		//3��Ī�϶�
		else{	gluLookAt(player_socket.x + player_client.Camx, player_socket.y + player_client.Camy + 190, player_socket.z + player_client.Camz, player_socket.x + player_client.Viewx, player_socket.y + player_client.Viewy + 190, player_socket.z + player_client.Viewz, 0.0, 1.0, 0.0);	}

		//������
		glEnable(GL_DEPTH_TEST);			// ������ �� ����
		glEnable(GL_CULL_FACE);				// �ĸ� ����

		//�� �׸���
		DrawMap();

		//�÷��̾� �׸���
		if(player_socket.live)
			drawCharacter(&player_socket, sight, character_up_state, right_leg_x, right_leg_z, left_leg_x, left_leg_z, right_knee_x, left_knee_x);

		//�ٸ� �÷��̾� �׸���
		for (int i = 0; i < MAX_Client; i++)
		{
			if ((i == client_imei) || (server_data.Players[i].live == false)) {}
			else
			{
				if (player_socket.team == server_data.Players[i].team)
					drawCharacter(&server_data.Players[i], sight, character_up_state, right_leg_x, right_leg_z, left_leg_x, left_leg_z, right_knee_x, left_knee_x);
				else
					drawZombie(&server_data.Players[i], sight, character_up_state, right_leg_x, right_leg_z, left_leg_x, left_leg_z, right_knee_x, left_knee_x);
			}
		}

		drawHud();//HUD �׸��� �κ� ���ο� push pop �˾Ƽ� ��
	}
	else
	{
		gluLookAt(2000, 1800, 1500, 0, 0, 0, 0.0, 1.0, 0.0);

		//�� �׸���
		DrawMap();

		//�ٸ� �÷��̾� �׸���
		for (int i = 0; i < MAX_Client; i++)
		{
			if ((i == client_imei) || (server_data.Players[i].live == false)) {}
			else 
			{ 
				if(player_socket.team == server_data.Players[i].team)
					drawCharacter(&server_data.Players[i], sight, character_up_state, right_leg_x, right_leg_z, left_leg_x, left_leg_z, right_knee_x, left_knee_x); 
				else
					drawZombie(&server_data.Players[i], sight, character_up_state, right_leg_x, right_leg_z, left_leg_x, left_leg_z, right_knee_x, left_knee_x);
			}
		}

		glPushMatrix();//����ǥ�� ����
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, 800, 600, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor3f(1, 0, 0);
		glTranslatef(20, 570, 0);
		glRasterPos2f(0.0, 0.0);
		sprintf(PressSpace, "Please Press \"Spacebar\"");
		len = (int)strlen(PressSpace);
		for (int i = 0; i < len; i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, PressSpace[i]);
		glPopMatrix();//����ǥ�� ��

	}



	glutSwapBuffers(); //ȭ�鿡 ����ϱ�

	CHECK_TIME_END(fTime, err);//�ð����� ����
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
		if (GameStart)
		{
			if (jump) {}
			else
			{
				jump = true;
				gravity = 15;
				Keybuffer[key] = false;
			}
		}
		else
		{
			GameStart = true;
			player_socket.live = true;
			if (player_socket.team)
			{
				player_socket.x = 2000;
				player_socket.y = 0;
				player_socket.z = 0;
				player_client.Viewx = -2000;
				player_client.Viewy = 0;
				player_client.Viewz = 0;
			}
			else
			{
				player_socket.x = -2000;
				player_socket.y = 0;
				player_socket.z = 0;
				player_client.Viewx = 2000;
				player_client.Viewy = 0;
				player_client.Viewz = 0;
			}
		}
		break;
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
			sight = false;
			//turn_Light = true;
		}
		else {
			FirstPersonView = true;
			//turn_Light = true;
		}
		Keybuffer[key] = false;
		break;
	case 't':
		printf("\n%d\n", player_socket.live);
		for(int i = 0; i < MAX_Client; i++)
			printf("%d", server_data.Players[i].live);
		printf("\n");
		for (int i = 0; i < MAX_Client; i++)
			printf("%d", server_data.Players[i].team);
		break;
	case 27://ESC
		if (GameStart)
		{
			GameStart = false;
			Keybuffer[key] = false;
			player_socket.x = 0;
			player_socket.y = -1000;
			player_socket.z = 0;
			player_client.Camx = 0;
			player_client.Camy = 0;
			player_client.Camz = 0;
			player_client.Viewx = 0;
			player_client.Viewy = 0;
			player_client.Viewz = 0;
			player_socket.live = false;
		}
		else exit(0);
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
		//Charspeed = 8;
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

		player_socket.y += gravity;
		if (jump)
		{
			gravity -= 0.5;
			character_down_state = CHARACTER_CROUCH_JUMP;
		}

		if (player_socket.y < 0)
		{
			player_socket.y = 0;
			gravity = 0;
			character_down_state = CHARACTER_STAND;
			jump = false;
		}


		switch (character_down_state) {
		case 0://������ ������ ��
			left_leg_x = 0;
			left_leg_y = 0;
			left_leg_z = 0;
			left_knee_x = 0;
			right_leg_x = 0;
			right_leg_y = 0;
			right_leg_z = 0;
			right_knee_x = 0;
			break;
		case 1://����,����
			left_leg_z = 0;
			right_leg_z = 0;
			if (legtimer < 150)
			{
				legtimer += 2;
				if (legtimer < 75)
				{
					left_leg_x = legtimer - 45;
					right_leg_x = 30 - legtimer;
					left_knee_x = 0;
					if (legtimer < 38)
						right_knee_x = legtimer;
					else
						right_knee_x = 38 - (legtimer - 38);
				}
				else
				{
					left_leg_x = 105 - legtimer;
					right_leg_x = legtimer - 120;
					right_knee_x = 0;

					if (legtimer < 113)
						left_knee_x = legtimer - 75;
					else
						left_knee_x = 38 - (legtimer - 113);
				}
			}
			else
				legtimer = 0;
			break;
		case 2://�¿��̵�
			break;
		case 3://�ɱ�
			left_leg_x = -75;
			left_leg_y = 0;
			left_leg_z = 40;
			left_knee_x = 130;
			right_leg_x = -75;
			right_leg_y = 0;
			right_leg_z = -15;
			right_knee_x = 130;
			break;
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

		player_socket.camxrotate = player_socket.camxrotate + (nx / MouseSens);
		player_socket.camyrotate = player_socket.camyrotate + (ny / MouseSens);
		//left_sholder_x = player_socket.camyrotate;
		//right_sholder_x = player_socket.camyrotate;

		player_client.Viewz = 10000 * sin((player_socket.camyrotate)* 3.141592 / 180) * cos((player_socket.camxrotate)* 3.141592 / 180);
		player_client.Viewx = 10000 * sin((player_socket.camyrotate)* 3.141592 / 180) * sin((player_socket.camxrotate)* 3.141592 / 180);
		player_client.Viewy = 10000 * cos((player_socket.camyrotate)* 3.141592 / 180);

		player_client.Camz = -Camdistance * sin((player_socket.camyrotate)* 3.141592 / 180) * cos((player_socket.camxrotate + 15)* 3.141592 / 180);
		player_client.Camx = -Camdistance * sin((player_socket.camyrotate)* 3.141592 / 180) * sin((player_socket.camxrotate + 15)* 3.141592 / 180);
		player_client.Camy = -Camdistance * cos((player_socket.camyrotate - 15)* 3.141592 / 180);


		if (player_socket.camxrotate <= -360 || player_socket.camxrotate >= 360)
			player_socket.camxrotate = 0;

		if (player_socket.camyrotate < -179)
			player_socket.camyrotate = -179;
		else if (player_socket.camyrotate > -1)
			player_socket.camyrotate = -1;
		bx = nx;
		by = ny;
	}
}

void Keyinput(int key)
{
	if (key == 'w')
	{
		player_socket.x += fTime*Charspeed * cos((-player_socket.camxrotate - 90) * 3.141592 / 180);
		player_socket.z += fTime*Charspeed * sin((-player_socket.camxrotate - 90) * 3.141592 / 180);
		if (jump)
			character_down_state = CHARACTER_CROUCH_JUMP;
		else
			character_down_state = CHARACTER_WALK;
		//printf("%f, %f, %f\n", fTime, Charspeed, fTime*Charspeed);
	}
	else if (key == 's')
	{
		player_socket.x -= fTime*Charspeed * cos((-player_socket.camxrotate - 90) * 3.141592 / 180);
		player_socket.z -= fTime*Charspeed * sin((-player_socket.camxrotate - 90) * 3.141592 / 180);
		if (jump)
			character_down_state = CHARACTER_CROUCH_JUMP;
		else
			character_down_state = CHARACTER_WALK;
	}
	if (key == 'a')
	{
		player_socket.x -= fTime*Charspeed * cos((-player_socket.camxrotate) * 3.141592 / 180);
		player_socket.z -= fTime*Charspeed * sin((-player_socket.camxrotate) * 3.141592 / 180);
		if (jump)
			character_down_state = CHARACTER_CROUCH_JUMP;
		else
			character_down_state = CHARACTER_WALK;
	}
	else if (key == 'd')
	{
		player_socket.x += fTime*Charspeed * cos((-player_socket.camxrotate) * 3.141592 / 180);
		player_socket.z += fTime*Charspeed * sin((-player_socket.camxrotate) * 3.141592 / 180);
		if (jump)
			character_down_state = CHARACTER_CROUCH_JUMP;
		else
			character_down_state = CHARACTER_WALK;
	}
}
