
#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#include <math.h>
#include"windows.h"

#define MAX_CHAR 128
GLfloat win_x = 600, win_y = 600;
GLfloat win_x_left = win_x / 3, win_x_right = win_x / 3 * 2, win_y_top = win_y / 3 * 2, win_y_bottom = win_y / 3;

void drawString(const char* str) { // Display characters for the outcodes
	static int isFirstCall = 1;
	static GLuint lists;
	if (isFirstCall)
	{
		isFirstCall = 0;
		lists = glGenLists(MAX_CHAR);
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	for (; *str != '\0'; ++str)
	{
		glCallList(lists + *str);
	}
}

void selectFont(int size, int charset, const char* face) { // Select a font face and size
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0, charset, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

void PlotLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) { // Draw a solid straight line
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
} 

void DashLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) { // Draw a dash straight line
	glLineWidth(1);
	glLineStipple(1, 0x0F0F);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
}

int encode(GLfloat x1, GLfloat y1) { // assign each point a region code
	int code = 0;
	if (x1 < win_x_left ) {
		code = code | 0x1; //0001
	}
	if (x1 > win_x_right) {
		code = code | 0x2; //0010
	}
	if (y1 < win_y_bottom) {
		code = code | 0x4; //0100
	}
	if (y1 > win_y_top) {
		code = code | 0x8; //1000
	}
	return code;
}

void cohen_sutherland(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
	bool done = false, outside=false;
	int code1 = encode(x1, y1);
	int code2 = encode(x2, y2);
	do {
		// trivial accept
		if (code1 == 0 && code2 == 0) {
			done = true;
		}
		// trivial reject
		else if ((code1 & code2) != 0) {
			done = true;
			outside = true;
		}
		//non-trivial reject
		else if ((code1 & code2) == 0 && (code1 | code2) != 0) {
			DashLine(x1,y1,x2,y2);
			float x = 0, y = 0;
			int code = 0;
			// check which point is outside the region
			if (code1 != 0)
				code = code1;
			else
				code = code2;
			// calculate the intersection point based on different cases 
			if (code & 8)// top
			{
				x = x1 + (x2 - x1) * (win_y_top - y1) / (y2 - y1);
				y = win_y_top;
			}
			else if (code & 4) // bottom
			{
				x = x1 + (x2 - x1) * (win_y_bottom - y1) / (y2 - y1);
				y = win_y_bottom;
			}
			else if (code & 2) // right
			{
				y = y1 + (y2 - y1) * (win_x_right - x1) / (x2 - x1);
				x = win_x_right;
			}
			else if (code & 1) //left
			{

				y = y1 + (y2 - y1) * (win_x_left - x1) / (x2 - x1);
				x = win_x_left;
			}
			// if both two points are outside the region
			if (code == code1)
			{
				x1 = x;
				y1 = y;
				code1 = encode(x1, y1);
			}
			else
			{
				x2 = x;
				y2 = y;
				code2 = encode(x2, y2);
			}
		}
	} 
	while (!done);
	if (!outside) {
		PlotLine(x1, y1, x2, y2);
	}
	else{ 
		DashLine(x1, y1, x2, y2); 
	}
}

void renderScene(void) { // Draw outcode regions, outcode values, and line segments have been clipped
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1, 1, 1);
	selectFont(24, ANSI_CHARSET, "Comic Sans MS");
	//plot outcodes
	glRasterPos2f(win_x_left / 2 - 30, win_y_top / 2 + win_y / 2);
	drawString("1001");
	glRasterPos2f(win_x_left / 2 - 30, win_y_top / 2 + win_y_bottom / 2);
	drawString("0001");
	glRasterPos2f(win_x_left / 2 - 30, win_y_bottom / 2);
	drawString("0101");
	glRasterPos2f(win_x_right / 2 - 30 + win_x_left / 2, win_y_top / 2 + win_y / 2);
	drawString("1000");
	glRasterPos2f(win_x_right / 2 - 20 + win_x_left / 2, win_y_top / 2 + win_y_bottom / 2);
	drawString("0000");
	glRasterPos2f(win_x_right / 2 - 30 + win_x_left / 2, win_y_bottom / 2);
	drawString("0100");
	glRasterPos2f(win_x_right / 2 - 30 + win_x / 2, win_y_top / 2 + win_y / 2);
	drawString("1010");
	glRasterPos2f(win_x_right / 2 - 30 + win_x / 2, win_y_top / 2 + win_y_bottom / 2);
	drawString("0010");
	glRasterPos2f(win_x_right / 2 - 30 + win_x / 2, win_y_bottom / 2);
	drawString("0110");

	PlotLine(win_x_left, 0, win_x_left, win_y);
	PlotLine(0, win_y_top, win_x, win_y_top);
	PlotLine(win_x_right, 0, win_x_right, win_y);
	PlotLine(0, win_y_bottom, win_x, win_y_bottom);
	
    // display the clipped lines
	glColor3f(255, 0, 0);  //red
	
	cohen_sutherland(80, 70, 440, 420); //one case of non-trivial reject
	cohen_sutherland(140,250,440, 400); // another case of non-trivial reject
	cohen_sutherland(220, 220, 220, 450); // another case of non - trivial reject
	
	cohen_sutherland(280, 270, 340, 350); // trivial accept
	
	cohen_sutherland(480, 470, 540, 520);  // trivial reject
	cohen_sutherland(80, 470, 80, 20); // trivial reject

	glutSwapBuffers();
}
void myinit(void) { // Initialisation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, win_x, 0, win_y);
}

int main(int argc, char* argv[]) { // The main program
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(win_x, win_y);
	glutCreateWindow("Cohen-Sutherland");
	myinit();
	glutDisplayFunc(renderScene);
	glutMainLoop();
	return 0;
}