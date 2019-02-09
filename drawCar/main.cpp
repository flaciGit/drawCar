#include <GL/glut.h>
#include <bevgrafmath2017.h>
#include <math.h>
#include <time.h>

GLsizei winWidth = 600, winHeight = 500;

float activeParam = 0;
GLfloat lineSize = 2;
GLfloat pointSize = 5;


GLint keyStates[256];
bool drawStraightLines = false;
bool drawPoints = true;


GLint wheelSize = 50;
GLint wheelInnerSize = 30;

GLfloat t0 = -1;
GLfloat t1 = -0.5;
GLfloat t2 = 1;


GLint glutTimerFuncValue = 30;
						
vec2 points[18] = { {27,68},{23,84},{28,107},{51,107},{95,109},			 //1-5
					{66,141},{85,203},{107,261},{205,359},{360,300},	 //6-10
					{474,226},{582,123},{578,68},{162,190},{153,240},	 //11-15
					{239,274},{341,256},{357,192}						 //15-18

					};

GLint dragged = -1;

double oldX, oldY, currentX, currentY;
vec2 controlPoints[2];


void init()
{
	srand(time(NULL));
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
	glShadeModel(GL_FLAT);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE);
	glPointSize(pointSize);
	glLineWidth(5.0);
	glLineStipple(1, 0xFF00);
}

void keyPressed(unsigned char key, int x, int y)
{
	keyStates[key] = 1;
}

void keyUp(unsigned char key, int x, int y)
{
	keyStates[key] = 0;
}

void keyOperations(int value)
{
	if (keyStates[32]) { //32 space
		keyStates[32] = 0;
		drawStraightLines = !drawStraightLines;
	}
	if (keyStates['w']) {
		drawPoints = !drawPoints;
		keyStates['w'] = 0;
	}
	if (keyStates['d']) {
		if(activeParam<0.99)activeParam += 0.01;
		keyStates['d'] = 0;
		printf("activeParam: %g\n", activeParam);
	}
	if (keyStates['a']) {
		if (activeParam>0.01)activeParam -= 0.01;
		keyStates['a'] = 0;
		printf("activeParam: %g\n", activeParam);
	}
	

	glutPostRedisplay();

	glutTimerFunc(glutTimerFuncValue, keyOperations, 0);
}

int factorial(int n) {
	int r = 1;
	for (int i = n; i>0; i--) {
		r *= i;
	}
	return r;
}

void drawLine(float l, float m, float j, float i)
{
	
	glBegin(GL_LINES);
	glVertex2f(l, m);
	glVertex2f(j, i);
	glEnd();
	glFlush();
}

void calc4() {
	GLfloat tX = points[2].x - points[3].x;
	GLfloat tY = points[2].y - points[3].y;

	GLfloat n = sqrt(tX*tX + tY*tY);

	GLfloat x3 = tX * cos(pi()) - tY * sin(pi() );
	GLfloat y3 = tY * cos(pi() ) + tX * sin(pi() );

	points[4].x = x3 + points[3].x;
	points[4].y = y3 + points[3].y;

}

void calc7() {
	GLfloat tX = points[5].x - points[6].x;
	GLfloat tY = points[5].y - points[6].y;

	GLfloat n = sqrt(tX*tX + tY * tY);
	
	GLfloat x3 = tX * cos(pi() ) - tY * sin(pi() );
	GLfloat y3 = tY * cos(pi() ) + tX * sin(pi() );

	points[7].x = x3 + points[6].x;
	points[7].y = y3 + points[6].y;

}

void bezier(vec2 P0, vec2 P1, vec2 P2, vec2 P3)
{
	oldX = P0.x;
	oldY = P0.y;

	vec2 p[4] = { P0,P1,P2,P3 };
	double t;
	int i;
	for (t = 0; t <= 1.0; t = t + 0.01)
	{
		currentX = 0;
		currentY = 0;
		for (i = 0; i <= 3; i++)
		{
			currentX = currentX + double((double)factorial(3) / (double)(factorial(3 - i)*factorial(i)))*(pow((1 - t), 3 - i))*pow(t, i)*p[i].x;
			currentY = currentY + double((double)factorial(3) / (double)(factorial(3 - i)*factorial(i)))*(pow((1 - t), 3 - i))*pow(t, i)*p[i].y;
		}
		drawLine(oldX, oldY, currentX, currentY);
		oldX = currentX;
		oldY = currentY;
	}
	drawLine(currentX, currentY,P3.x,P3.y);
}

float d1X, xAB, xBC, xABC, xBCD, xABCD, xBCDE;
float d1Y, yAB, yBC, yABC, yBCD, yABCD, yBCDE;

bool xy;  //true = x  flase = y

bool active = false;
float mix(float a, float b, float t,bool xy,bool ac)
{

	ac ? xy ? d1X = a * (1.0f - t) + b * t : d1Y = a * (1.0f - t) + b * t : false;
	return a * (1.0f - t) + b * t;
}

float BezierQuadratic(float A, float B, float C, float t, bool xy, bool ac)
{
	// degree 2
	float AB = mix(A, B, t,xy, ac);
	float BC = mix(B, C, t,xy, ac);
	if(ac)
		if (xy)
		{
			xAB = AB;
			xBC = BC;
		}
		else {
			yAB = AB;
			yBC = BC;
		}
	return mix(AB, BC, t,xy, ac);
}

float BezierCubic(float A, float B, float C, float D, float t, bool xy, bool ac)
{
	// degree 3
	float ABC = BezierQuadratic(A, B, C, t,xy, ac);
	float BCD = BezierQuadratic(B, C, D, t,xy, ac);
	if(ac)
		if (xy) {
			xABC = ABC;
			xBCD = BCD;
		}
		else {
			yABC = ABC;
			yBCD = BCD;
		}

	return mix(ABC, BCD, t, xy, ac);
}

float BezierQuartic(float A, float B, float C, float D, float E, float t, bool xy, bool ac)
{
	// degree 4
	float ABCD = BezierCubic(A, B, C, D, t,xy, ac);
	float BCDE = BezierCubic(B, C, D, E, t,xy, ac);

	if(ac)
		if (xy) {
			xABCD = ABCD;
			xBCDE = BCDE;
		}
		else {
			yABCD = ABCD;
			yBCDE = BCDE;
		}

	return mix(ABCD, BCDE, t,xy, ac);
}

void drawMoreLines(float t) {
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);

	BezierQuadratic(points[13].x, points[14].x, points[15].x, t,true,t == activeParam);
	BezierQuadratic(points[13].y, points[14].y, points[15].y, t, false, t == activeParam);
	float txAB, tyAB;
	float txBC, tyBC;

	txAB = xAB;
	tyAB = yAB;
	txBC = xBC;
	tyBC = yBC;

	glVertex2f(xAB, yAB);
	glVertex2f(xBC, yBC);

	BezierQuadratic(points[14].x, points[15].x, points[16].x, t, true, t == activeParam);
	BezierQuadratic(points[14].y, points[15].y, points[16].y, t, false, t == activeParam);

	float txCD, tyCD;
	txCD = xBC;
	tyCD = yBC;

	glVertex2f(txBC, tyBC);
	glVertex2f(txCD, tyCD);

	BezierQuadratic(points[15].x, points[16].x, points[17].x, t, true, t == activeParam);
	BezierQuadratic(points[15].y, points[16].y, points[17].y, t, false, t == activeParam);

	float txDE, tyDE;

	txDE = xBC;
	tyDE = yBC;

	glVertex2f(txCD, tyCD);
	glVertex2f(txDE, tyDE);

	BezierQuadratic(txAB, txBC, txCD, t, true, t == activeParam);
	BezierQuadratic(tyAB, tyBC, tyCD, t, false, t == activeParam);

	float txABBC = xAB;
	float tyABBC = yAB;
	float txBCCD = xBC;
	float tyBCCD = yBC;

	glVertex2f(txABBC, tyABBC);
	glVertex2f(txBCCD, tyBCCD);
	
	BezierQuadratic(txBC, txCD,txDE, t, true, t == activeParam);
	BezierQuadratic(tyBC, tyCD,tyDE, t, false, t == activeParam);

	float txCDDE = xBC;
	float tyCDDE = yBC;

	glVertex2f(txBCCD, tyBCCD);
	glVertex2f(txCDDE, tyCDDE);

	BezierQuadratic(txABBC, txBCCD, txCDDE, t, true, t == activeParam);
	BezierQuadratic(tyABBC, tyBCCD, tyCDDE, t, false, t == activeParam);

	float finalXA = xAB;
	float finalYA = yAB;

	float finalXD = xBC;
	float finalYD = yBC;

	glVertex2f(finalXA, finalYA);
	glVertex2f(finalXD, finalYD);

	glEnd();

}
void cd() {

	oldX = points[13].x;
	oldY = points[13].y;
	
	glPointSize(lineSize);
	for (float t = 0; t <= 1.0; t = t + 0.01){
		glColor3f(0.8, 0.8, 0.0);
		
		vec2 p;
		p.x = BezierQuartic(points[13].x, points[14].x, points[15].x, points[16].x, points[17].x, t,true, activeParam==t);
		p.y = BezierQuartic(points[13].y, points[14].y, points[15].y, points[16].y, points[17].y, t,false, activeParam == t);

		if (t == activeParam) {
			controlPoints[0] = { p.x,p.y };

			controlPoints[1] = { mix(points[13].x, points[17].x,t,true,true) , mix(points[13].y, points[17].y,t,false,true) };

		}

		glPointSize(lineSize);

		currentX = p.x;
		currentY = p.y;
		drawLine(oldX,oldY,currentX,currentY);

		oldX = currentX;
		oldY = currentY;
		
		if(drawStraightLines)
			drawMoreLines(t);
	}
	

	if (drawPoints) {
		glPointSize(pointSize);
		glBegin(GL_POINTS);

		glColor3f(1.0, 0.0, 1.0);
		
		glVertex2f(controlPoints[0].x, controlPoints[0].y);
		glVertex2f(controlPoints[1].x, controlPoints[1].y);

		glEnd();
	}

}

void hermiteCurve() {


	vec2 n10 = { points[9].x - points[10].x,points[9].y - points[10].y };
	mat24 G = { points[10],points[11],points[12],n10 };

	
	vec4 o0 = { t0*t0*t0, t0*t0, t0, 1 };
	vec4 o1 = { t1*t1*t1, t1*t1, t1, 1 };
	vec4 o2 = { t2*t2*t2, t2*t2, t2, 1 };
	vec4 o3 = { 3 * t2*t2, 2*t2, 1, 0 };

	mat4 N = { o2,o1,o0,o3,true };

	mat4 M = inverse(N);

	mat24 C = G * M;

	glPointSize(lineSize);
	glColor3f(1.0,1.0,0.0);
	glBegin(GL_POINTS);
	
	for (GLdouble t = -1; t <=1; t += 0.01) {

		vec4 TI = { (float)(t*t*t), (float)(t*t), (float)t, 1 };
		vec2 P = C * TI;
		glVertex2f(P.x, P.y);
	}
	
	glEnd();
	glPointSize(pointSize);

}

void circle(GLfloat x, GLfloat y, GLdouble r) {
	glBegin(GL_TRIANGLE_FAN);
	for (GLdouble t = 0; t <= 2 * pi(); t += 0.01)
		glVertex2d(x + r * cos(t), y + r * sin(t));

	glEnd();

}
void sokszog(GLfloat x, GLfloat y, GLdouble r,GLint n) {
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i<n; i++) {
		double angle = i * 2 * pi() / n;
		glVertex2d(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();

}

void calsCar() {

	GLfloat tempX = points[12].x - points[0].x;
	GLfloat tempY = points[12].y - points[0].y;

	GLfloat vLen = sqrt(tempX*tempX+ tempY* tempY);
	tempX /= vLen;
	tempY /= vLen;

	glColor3f(0.5, 0.5, 0.5);
	circle(tempX * 1 / 4 * vLen + points[0].x, tempY * 1 / 4 * vLen + points[0].y, wheelSize);
	circle(tempX * 3 / 4 * vLen + points[0].x, tempY * 3 / 4 * vLen + points[0].y, wheelSize);

	glColor3f(0.0, 0.0, 0.0);
	sokszog(tempX * 1 / 4 * vLen + points[0].x, tempY * 1 / 4 * vLen + points[0].y, wheelInnerSize,10);
	sokszog(tempX * 3 / 4 * vLen + points[0].x, tempY * 3 / 4 * vLen + points[0].y, wheelInnerSize,10);

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex2f(points[9].x, points[9].y);
	glVertex2f(points[10].x, points[10].y);
	glEnd();

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex2f(points[13].x, points[13].y);
	glVertex2f(points[17].x, points[17].y);
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2f(points[0].x, points[0].y);
	glVertex2f(points[12].x, points[12].y);
	glEnd();


}

void display()
{
	GLint i;
	glClear(GL_COLOR_BUFFER_BIT);

	glClearColor(0.5,0.7,1.0, 0.0);

	glColor3f(0.0, 0.0, 1.0);

	glPointSize(pointSize);
	
	if (drawPoints) {
		glBegin(GL_POINTS);
		for (i = 0; i < 18; i++)
			glVertex2f(points[i].x, points[i].y);
		glEnd();
	
	}
	glLineWidth(lineSize);

	glColor3f(0.0, 0.0, 0.0);

	if (drawStraightLines) {
		glBegin(GL_LINE_LOOP);
		for (i = 0; i < 13; i++)
			glVertex2f(points[i].x, points[i].y);
		glEnd();

		glBegin(GL_LINE_LOOP);
		for (i = 13; i < 18; i++)
			glVertex2f(points[i].x, points[i].y);
		glEnd();

	}

	glColor3f(0.5, 1.0, 0.7);
	glRectf(0,0,winWidth,winHeight/10);

	calc4();
	calc7();
	calsCar();
	hermiteCurve();
	glColor3f(1.0, 0.0, 0.0);
	bezier(points[0], points[1], points[2], points[3]);
	glColor3f(0.8, 0.2, 0.0);
	bezier(points[3], points[4], points[5], points[6]);
	glColor3f(0.6, 0.4, 0.0);
	bezier(points[6], points[7], points[8], points[9]);
	cd();

	glutSwapBuffers();
}

void update(int n)
{
	glutPostRedisplay();
	glutTimerFunc(glutTimerFuncValue, update, 0);
}

GLint getActivePoint1(vec2 p[], GLint size, GLint sens, GLint x, GLint y)
{
	GLint i, s = sens * sens;
	vec2 P = { (float)x, (float)y };

	for (i = 0; i < size; i++)
		if (dist2(p[i], P) < s)
			return i;
	return -1;
}

GLint getActivePoint2(vec2 *p, GLint size, GLint sens, GLint x, GLint y)
{
	GLint i;
	for (i = 0; i < size; i++)
		if (fabs((*(p + i)).x - x) < sens && fabs((*(p + i)).y - y) < sens)
			return i;
	return -1;
}

void processMouse(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	GLint i;

	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN) {
		if ((i = getActivePoint1(points, 18, 8, xMouse, winHeight - yMouse)) != -1)
			if(i != 4 && i != 7)
		
			{
				dragged = i;
				printf("i: %d, points[%d]:[%g,%g] \n",i,i,points[i].x, points[i].y);
			
			}
	}
	
	if (button == GLUT_LEFT_BUTTON && action == GLUT_UP)
		dragged = -1;
}

void processMouseActiveMotion(GLint xMouse, GLint yMouse)
{
	GLint i;
	if (dragged >= 0) {
		points[dragged].x = xMouse;
		points[dragged].y = winHeight - yMouse;
		glutPostRedisplay();
		calc4();
		calc7();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Draw a car");
	init();
	glutDisplayFunc(display);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutTimerFunc(0, update, 0);
	glutTimerFunc(0, keyOperations, 0);
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);
	glutMainLoop();
	return 0;
}