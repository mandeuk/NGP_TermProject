#include "main.h"

void DrawMap()
{
	glPushMatrix(); //Save
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//glBindTexture(GL_TEXTURE_2D, image);
	glBegin(GL_QUADS);
		//��ȣ's �����ڵ�
	glTexCoord2d(1.0f, 1.0f);
	glVertex3f(-4000, 0, 4000);  //1
	glTexCoord2d(0.0f, 1.0f);
	glVertex3f(4000, 0, 4000);   //4
	glTexCoord2d(0.0f, 0.0f);
	glVertex3f(4000, 0, -4000);   //6
	glTexCoord2d(1.0f, 0.0f);
	glVertex3f(-4000, 0, -4000);   //5

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();


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
}