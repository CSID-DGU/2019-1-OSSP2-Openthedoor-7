#include <Windows.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>

# define PI 3.141592

float width = 780.0;//윈도우 가로
float height = 600.0;//윈도우 세로

float radius = 5.0;
int num = 45;

float radius1 = 20.0;

float xp = 40.0;
float yp = 40.0;

float delta;
float point_size = 3.0;
GLenum draw_type;

void init() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-20, width, height, -20);
}

void Draw_Circle(float x, float y, float r) {
	delta = 2.0*PI / num;
	glBegin(GL_POLYGON);
	for (int i = 0; i < num; i++) {
		glVertex2f(x + r * cos(delta*i), y + r * sin(delta*i));
	}
	glEnd();
}

/*void Make_Brick(float x, float y, float r) {
	for (int row = 0; row < 1; row++) {
		for (int col = 0; col < 20; col++) {
			Draw_Circle(x*col, y*row, r);
		}
	}
}*/
void RenderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();

	//Make_Brick(xp, yp, radius1);
	Draw_Circle(xp, yp, radius1);

	glPointSize(point_size);
	glColor3f(1, 0, 0);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
}

void abc(void) {
	RenderScene();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(780, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("원그리기");
	init();

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(MyReshape);
	glutIdleFunc(abc);
	glutMainLoop();
}