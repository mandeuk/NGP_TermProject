#include "main.h"

void draw_Crosshair(bool sight){
	if (sight){
		glColor3f(0, 0, 1);
		glBegin(GL_QUADS);// Crosshair
		glVertex2f(396, 304);
		glVertex2f(404, 304);
		glVertex2f(404, 296);
		glVertex2f(396, 296);
		glEnd();
	}
	else{
		glColor3f(0, 1, 0);
		glBegin(GL_LINES);// Crosshair
		glVertex2f(380, 300);
		glVertex2f(390, 300);

		glVertex2f(410, 300);
		glVertex2f(420, 300);

		glVertex2f(400, 280);
		glVertex2f(400, 290);

		glVertex2f(400, 310);
		glVertex2f(400, 320);
		glEnd();
	}
}


void drawHud()
{
	glPushMatrix();

	glViewport(0, 0, 800, 600);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 800, 600, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);      // 조명 활성화
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_NORMALIZE);

	draw_Crosshair(false);// 크로스 헤어 구현


						  //glPushMatrix();//총알 표시
						  //glColor3f(0, 1, 0);
						  //glTranslatef(680, 570, 0);
						  //glRasterPos2f(0.0, 0.0);
						  //sprintf(ammo, "%d / %d", rifleload, rifleammo);
						  //int len = (int)strlen(ammo);
						  //for (int i = 0; i < len; i++)
						  //	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ammo[i]);
						  //glPopMatrix();//총알 표시

						  //glPushMatrix();//체력 표시
						  //glColor3f(1, 0, 0);
						  //glTranslatef(20, 570, 0);
						  //glRasterPos2f(0.0, 0.0);
						  //sprintf(health, "%d", hp);
						  //len = (int)strlen(ammo);
						  //for (int i = 0; i < len; i++)
						  //	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, health[i]);
						  //glPopMatrix();//체력 표시 pop

	glPopMatrix();
}
