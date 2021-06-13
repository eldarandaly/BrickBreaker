#include <iostream>
#include <cstdlib> // for srand() function
#include <ctime> // for time() function
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <irrKlang.h> // for music and sounds
#include "GL/freeglut.h"
#include "FreeImage.h"

#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "freeimage.lib")
#pragma comment(lib, "irrKlang.lib")

using namespace std;
using namespace irrklang;

ISoundEngine* creditsSound = createIrrKlangDevice();
ISoundEngine* gamePlaySound = createIrrKlangDevice();
ISoundEngine* hitblock = createIrrKlangDevice();
ISoundEngine* hitPaddle = createIrrKlangDevice();
ISoundEngine* GameOver = createIrrKlangDevice();
ISoundEngine* victory = createIrrKlangDevice();

// Declaring some of the constants
GLfloat posXP = 0, posYP = 10, posZP = 0; // Paddle position
GLfloat posXB = 0, posYB = 0, posZB = 0; // Ball Position
GLfloat dx = 0, dy = 1.0; // x-component change and y-component change

bool incx = false; // x shouldn't be incremented at first release
bool incy = true; // y should be incremented at first release
bool ballRel = false; // ball release 
bool pause = false; // pause should be false at first

int xmax, ymax;
int scrolly;
int padsize = 200;
int currentWindow = 1;
int score = 0;
const int column = 10;
const int row = 10;

// Texture IDs
GLuint menuDisplayTexture;
GLuint logoTexture;
GLuint newGameButtonTexture;
GLuint creditsButtonTexture;
GLuint gameDisplayTexture;
GLuint blockTexture;

GLfloat light1_ambient[] = { 1.0, 1.0,1.0, 1.0 };
GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light1_position[] = { -5, -5, 1.0, 0.0 };
GLfloat spot_direction[] = { 5, 5, 1.0 };
//GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//GLfloat mat_shininess[] = { 1000.0 };
int bricks[column][row];
int windows[4];

// Windows : 
// menudisplay : currentWindow = 1 --> windows[0]
// gamedisplay : currentWindow = 2 --> windows[1]
// scoredisplay : currentWindow = 3 --> windows[2]
// creditsdisplay : currentWindow = 4 --> windows[3]

/*All Function prototypes ---> for debugging purposes and out of order function calling*/
int loadTexture(const char* location);
void ball();
void paddle();
void block(float xv, float yv);
void menuDisplayBackground();
void gameLogo();
void newGameButton();
void creditsButton();
void brick();
void writestring(void* font, char* s, float x, float y);
void writestring(void* font, const char* s, float x, float y);
void detectblock();
void menudisplay();
void gamedisplay();
void scoredisplay();
void creditsdisplay();
void idlefunction();
void passivefunction(int x, int y);
void mousefunction(int button, int state, int x, int y);
void keyboardfunction(unsigned char key, int mousex, int mousey);
void init(int w, int h);
void light(void);
// loading Textures using FreeImage Library
int loadTexture(const char* location)
{
	GLuint texture;

	FIBITMAP* myImage = FreeImage_Load(FreeImage_GetFileType(location, 0), location);
	myImage = FreeImage_ConvertTo32Bits(myImage);

	int width = FreeImage_GetWidth(myImage);
	int height = FreeImage_GetHeight(myImage);

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
		GL_BGRA_EXT, GL_UNSIGNED_BYTE, FreeImage_GetBits(myImage));

	return texture;
}

// Creating a Ball
void ball()
{
	glColor3f(1.0f, 1.0f, 0.0f); // yellow color
	glScalef(5, 5, 0);
	glutSolidSphere(2.0, 100, 10);
}

// Creating a Paddle
void paddle()
{
	glColor3f(1.0f, 1.0f, 1.0f); // white color

	glBegin(GL_QUADS);

	glVertex2f(10.0, 0); // bottom left
	glVertex2f(padsize - 10.0, 0); // bottom right
	glVertex2f(padsize - 10.0, 20); // top right
	glVertex2f(10.0, 20); // top left

	glEnd();
}

// Creating a Brick
void block(float xv, float yv)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, blockTexture);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2f(0, 0); // Down Left
	glTexCoord2d(1, 0); glVertex2f(xv, 0); // Down Right
	glTexCoord2d(1, 1); glVertex2f(xv, yv); // Up Right
	glTexCoord2d(0, 1); glVertex2f(0, yv); // Up Left
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

// Draws menuDisplay Background
void menuDisplayBackground()
{
	glPushMatrix();
	glLoadIdentity();

	// Loading menuDisplay Texture
	glBindTexture(GL_TEXTURE_2D, menuDisplayTexture);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2f(0, 0); // Down Left
	glTexCoord2d(1, 0); glVertex2f(xmax, 0); // Down Right
	glTexCoord2d(1, 1); glVertex2f(xmax, ymax); // Up Right
	glTexCoord2d(0, 1); glVertex2f(0, ymax); // Up Left
	glEnd();

	glPopMatrix();
}

void GameDisplayTexture()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gameDisplayTexture);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2f(0, 0); // Down Left
	glTexCoord2d(1, 0); glVertex2f(xmax, 0); // Down Right
	glTexCoord2d(1, 1); glVertex2f(xmax, ymax); // Up Right
	glTexCoord2d(0, 1); glVertex2f(0, ymax); // Up Left
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
// Draws the game logo
void gameLogo()
{
	glPushMatrix();
	glLoadIdentity();

	// Loading Logo Texture
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, logoTexture);
	// center (xmax/2, ymax/2)
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex2f((xmax / 2) - 300, ymax - 250); // Down Left
	glTexCoord2d(1, 0); glVertex2f((xmax / 2) + 330, ymax - 250); // Down Right
	glTexCoord2d(1, 1); glVertex2f((xmax / 2) + 330, ymax - 10); // Up Right
	glTexCoord2d(0, 1); glVertex2f((xmax / 2) - 300, ymax - 10); // Up Left
	glEnd();

	glPopMatrix();
	glDisable(GL_BLEND);
}

// Draws the new game button
void newGameButton()
{
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(0.35 * xmax, 0.50 * ymax, 0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, newGameButtonTexture);

	glBegin(GL_QUADS);

	glTexCoord2d(0, 0); glVertex2f(-90, -140); // Down Left
	glTexCoord2d(1, 0); glVertex2f(490, -140); // Down Right
	glTexCoord2d(1, 1); glVertex2f(490, 190); // Up Right
	glTexCoord2d(0, 1); glVertex2f(-90, 190); // Up Left
	glEnd();
	glDisable(GL_BLEND);
	glPopMatrix();
}

// Draws the credits Button
void creditsButton()
{
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(0.35 * xmax, 0.25 * ymax, 0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, creditsButtonTexture);
	glBegin(GL_QUADS);

	glTexCoord2d(0, 0); glVertex2f(-90, -140); // Down Left
	glTexCoord2d(1, 0); glVertex2f(490, -140); // Down Right
	glTexCoord2d(1, 1); glVertex2f(490, 190); // Up Right
	glTexCoord2d(0, 1); glVertex2f(-90, 190); // Up Left
	glEnd();
	glDisable(GL_BLEND);
	glPopMatrix();
}

// Calculating Brick Positions and Calling Block to draw them
void brick()
{
	float xf = xmax / column;
	float yf = (0.50 * ymax) / row;
	for (int i = 0; i < column; i++)
	{
		for (int j = 0; j < row; j++)
		{
			if (bricks[i][j] == 1)
			{
				glPushMatrix();
				glLoadIdentity();
				// multiplies the current modelview matrix by a translation matrix
				glTranslatef((i * xf) + (0.25 * xf), (0.40 * ymax) + (j * yf) + (0.25 * yf), 0);

				// xf / 2 and yf / 2 are the bottom left coordinate of each block
				block(xf / 2, yf / 2);
				glPopMatrix();
			}
		}
	}
}

// renders a bitmap character
void writestring(void* font, char* s, float x, float y)
{
	glColor3f(1.0f, 1.0f, 1.0f); // white color
	glRasterPos2f(x, y);
	for (int i = 0; i < strlen(s); i++)
		glutBitmapCharacter(font, s[i]);
}

//renders a bitmap character
void writestring(void* font, const char* s, float x, float y)
{
	glColor3f(1.0f, 1.0f, 1.0f); // white color
	glRasterPos2f(x, y);
	for (int i = 0; i < strlen(s); i++)
		glutBitmapCharacter(font, s[i]);
}

//Display the starting screen
void menudisplay()
{
	// ModelView is the matrix that represents your camera (position, pointing, and up vector).
	// ProjectionView is the matrix that represents your camera's lens (aperture, far-field, near-field, etc).
	glMatrixMode(GL_MODELVIEW);

	menuDisplayBackground(); // draws menu Display background

	gameLogo(); // draws game logo

	newGameButton(); // draws "New Game" button

	creditsButton(); // draws "Credits" button
	

	// frame limiting
	glutPostRedisplay();
	Sleep(5);

	glFlush();
}

//Display the game window
void gamedisplay()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	GameDisplayTexture();

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(posXP, posYP, posZP); // position of the paddle (0,10,0)
	paddle();
	glPopMatrix();

	glPushMatrix();
	if (ballRel) {
		glTranslatef(posXB, posYB, posZB);
	}
	else {
		glTranslatef(posXP + (padsize / 2), posYP + 30.0, posZP);
	}
	ball();
	glPopMatrix();

	brick();

	char scorestring[10] = "Score : ";
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, scorestring, xmax * 0.75, ymax * 0.95);

	_itoa_s(score, scorestring, 10);//To convert score to string
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, scorestring, xmax * 0.85, ymax * 0.95);

	if (pause)
	{
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, " Press P to continue ... ", xmax * 0.25, ymax * 0.95);
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, " GAME PAUSED ", xmax * 0.45, ymax * 0.15);
	}
	else
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, " Press P to pause ... ", xmax * 0.25, ymax * 0.95);

	glutPostRedisplay();
	glFlush();
}

//Display the score page after playing
void scoredisplay()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.749f, 1.0f);
	glVertex2f(0, 0);
	glColor3f(0.780f, 0.082f, 0.522f);
	glVertex2f(0, ymax);
	glColor3f(0.0f, 0.545f, 1.0f);
	glVertex2f(xmax, ymax);
	glColor3f(0.780f, 0.082f, 0.522f);
	glVertex2f(xmax, 0);
	glEnd();

	glEnd();
	glPopMatrix();
	glPushMatrix();
	glLoadIdentity();
	glPushMatrix();
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "----------------------------------------------------------------------------------------", 0.05 * xmax, 0.80 * ymax);
	bool flag = false;
	for (int i = 0; i < column; i++) {
		for (int j = 0; j < row; j++) {
			if (bricks[i][j] == 1)
				flag = true;
		}
	}
	if (flag)
	{
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, "GAMEOVER BETTER LUCK NEXT TIME ;)", 0.35 * xmax, 0.75 * ymax);
		GameOver->play2D("Assets/sounds/GameOver.mp3", false);
	}
	else
	{
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, "VICTORY IS YOURS", 0.40 * xmax, 0.75 * ymax);
		gamePlaySound->stopAllSounds();
		victory->play2D("Assets/sounds/Victory.mp3", false);
	}
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "----------------------------------------------------------------------------------------", 0.05 * xmax, 0.70 * ymax);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "YOU SCORE IS :", 0.35 * xmax, 0.50 * ymax);
	char scorestr[10];
	_itoa_s(score, scorestr, 10);//To convert score to string
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, scorestr, 0.55 * xmax, 0.50 * ymax);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, " Press Enter to continue ... ", xmax * 0.35, ymax * 0.30);
	glPopMatrix();
	glFlush();
}

//Credits display
void creditsdisplay()
{
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glLoadIdentity();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.749f, 1.0f);
	glVertex2f(0, 0);
	glColor3f(0.780f, 0.082f, 0.522f);
	glVertex2f(0, ymax);
	glColor3f(0.0f, 0.545f, 1.0f);
	glVertex2f(xmax, ymax);
	glColor3f(0.780f, 0.082f, 0.522f);
	glVertex2f(xmax, 0);
	glEnd();

	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glPushMatrix();
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "----------------------------------------------------------------------------------------", 0.05 * xmax, (0.85 * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Developed by: ", 0.40 * xmax, (0.80 * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "----------------------------------------------------------------------------------------", 0.05 * xmax, (0.75 * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Ibrahim Ahmed", 0.35 * xmax, (0.70 * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Ahmed Mamdouh El Darandaly", 0.35 * xmax, (0.65 * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Mohamed Ismail", 0.35 * xmax, (0.60 * ymax) + scrolly);
	glPopMatrix();
	glutPostRedisplay();
	Sleep(1);
	glFlush();
}

// Brick detection in Gameplay
void detectblock()
{
	//To detect if there are still any bricks
	float xf = xmax / column;
	float yf = (0.50 * ymax) / row;
	bool flag = false;
	for (int i = 0; i < column; i++)
	{
		for (int j = 0; j < row; j++)
		{
			if (bricks[i][j] == 1)
				flag = true;
		}
	}

	if (!flag)
	{
		currentWindow = 3;
		ballRel = !ballRel;
		windows[2] = glutCreateWindow("Score");
		//glutFullScreen();
		glutSetWindow(windows[2]);
		glutDestroyWindow(windows[1]);
		init(xmax, ymax);
		glutKeyboardFunc(keyboardfunction);
		glutDisplayFunc(scoredisplay);
		glutMainLoop();
	}

	for (int i = 0; i < column; i++)
	{
		for (int j = 0; j < row; j++)
		{
			float minx = (i * xf) + (0.25 * xf);
			float maxx = (i * xf) + (0.25 * xf) + (xf / 2);
			float miny = (0.40 * ymax) + (j * yf) + (0.25 * yf);
			float maxy = (0.40 * ymax) + (j * yf) + (0.25 * yf) + (yf / 2);
			if (bricks[i][j] == 1)
			{
				if ((posXB + 10.0) >= minx && (posXB - 10.0) <= maxx && (posYB + 10.0) >= miny && (posYB - 10.0) <= maxy)
				{
					bricks[i][j] = 0;
					hitPaddle->play2D("Assets/sounds/bleep.wav", false);
					score += 100;
					if (posYB < miny || posYB > maxy) {
						if (posYB < miny)
							incy = false;
						else if (posYB > maxy)
							incy = true;
						if (posXB <= ((maxx - minx) / 2)) {
							incx = false;
						}
						else {
							incx = true;
						}
					}
					else if (posXB < minx || posYB > maxx)
					{
						if (posXB < minx)
							incx = false;
						else if (posXB > maxx)
							incx = true;
						if ((posYB + 10.0) <= ((maxy - miny) / 2))
						{
							incy = false;
						}
						else 
						{
							incy = true;
						}
					}
				}
			}
		}
	}
}

void idlefunction()
{
	if (currentWindow == 2 && !pause) {
		if (ballRel)
		{
			if (posXB > 10.0 && posXB < (xmax - 10.0) && posYB >(posYP + 20.0) && posYB < (ymax - 10.0))
			{
				detectblock();
				if (incx)
					posXB += 10.0 * dx;
				else
					posXB -= 10.0 * dx;
				if (incy)
					posYB += 10.0 * dy;
				else
					posYB -= 10.0 * dy;
				Sleep(1);
			}

			if (posXB < 10) {
				incx = true;
				posXB += 10.0 * dx;
			}

			if (posXB > (xmax - 10.0)) {
				incx = false;
				posXB -= 10.0 * dx;
			}

			if (posYB > ((ymax * 0.95) - 10.0)) {
				incy = false;
				posYB -= 10.0 * dy;
			}

			if (posYB <= (posYP + 20.0))
			{
				if ((posXB + 10.0) > posXP && (posXB - 10.0) < (posXP + padsize))
				{
					hitblock->play2D("Assets/sounds/bleep.mp3", false);
					incy = true;
					if (posXB == (posXP + (padsize / 2)))
					{
						dx = 0;
					}
					else if (posXB < (posXP + (padsize / 2))) {
						dx = (((posXP + (padsize / 2)) - posXB) / (padsize / 2)) + 0.25;
						incx = false;
						posXB -= 10.0 * dx;
					}
					else if (posXB > (posXP + (padsize / 2))) {
						dx = ((posXB - (posXP + (padsize / 2))) / (padsize / 2)) + 0.25;
						incx = true;
						posXB += 10.0 * dx;
					}
					dy = fabs(1 - dx);
					posYB += 10.0 * dy;
				}
				else
				{
					currentWindow = 3;
					ballRel = !ballRel;
					gamePlaySound->stopAllSounds();
					windows[2] = glutCreateWindow("Score");
					//glutFullScreen();
					glutSetWindow(windows[2]);
					glutDestroyWindow(windows[1]);
					init(xmax, ymax);
					glutKeyboardFunc(keyboardfunction);
					glutDisplayFunc(scoredisplay);
					glutMainLoop();
				}
			}
		}
		glutPostRedisplay();
	}

	if (currentWindow == 4)
	{
		scrolly++;
		if (scrolly > (0.35 * ymax))
		{
			currentWindow = 1;
			glutSetWindow(windows[0]);
			creditsSound->stopAllSounds();
			glutDestroyWindow(windows[3]);
		}
	}
}

// when in gameplay and games isn't paused but ballRel = false
void passivefunction(int x, int y)
{
	if (currentWindow == 2 && !pause)
	{
		if (x < (xmax - (padsize / 2)) && x >(padsize / 2)) //Paddle Size and Curved Surface
			posXP = x - (padsize / 2);
		else if (x >= (xmax - (padsize / 2)))
			posXP = xmax - padsize;
		else
			posXP = 0.0;
		// The next iteration through glutMainLoop, the window's display callback will be called to redisplay the window's normal plane.
		// glutPostRedisplay marks the current window as needing to be redisplayed.
		glutPostRedisplay();
	}
}

// The Mouse only has functions in currentWindow = 1 (menuDisplay) and currentWindow = 2 (gameDisplay)
void mousefunction(int button, int state, int x, int y)
{
	srand((unsigned int)time(NULL)); // To get more random results from rand() as it uses the same "random" seed
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (currentWindow == 1) // menu display
		{
			// if (x > left-side && x < right-side && y > bottom-side && y < top-side) of new game button
			if ((x > (0.35 * xmax) && x < ((0.35 * xmax) + 400.0)) && ((ymax - y) > (0.50 * ymax) && (ymax - y) < ((0.50 * ymax) + 100.0)))
			{
				gamePlaySound->play2D("Assets/sounds/game.mp3", true);
				// prepare gameDisplay and radmomize brick positions 
				for (int i = 0; i < column; i++)
				{
					for (int j = 0; j < row; j++)
					{
						bricks[i][j] = (rand() % 2);
					}
				}

				posXP = 0;  // x-postion of paddle
				posYP = 10; // y-postion of paddle 
				posZP = 0;	// z-postion of paddle

				posXB = 0;  // x-postion of ball
				posYB = 0;  // y-postion of ball
				posZB = 0;  // z-postion of ball

				dx = 0.0;	// the ball's x-component direction vector (initial ball direction is zero in the x-component)
				dy = 1.0;	// the ball's y-component direction vector (initial ball direction is directly upwards)

				incx = false; // x should't be incremented at first because we established that initial ball direction is zero in the x-component
				incy = true; // y should be incremented at first because we established that initial ball direction is directly upwards

				ballRel = false; // ball hasn't been released yet
				score = 0; // score is 0 because no blocks were destroyed yet

				windows[1] = glutCreateWindow("Game"); // create the gamedisplay

				glutSetWindow(windows[1]); // change OpenGL context so now OpenGL contexts will render in the new window
				//glutFullScreen();
				currentWindow = 2; // changes currentWindow for future reference
				init(xmax, ymax); // re-initializes the openGL context

				glutDisplayFunc(gamedisplay);
				gameDisplayTexture = loadTexture("Assets/Textures/gameDisplayTexture.jpg");
				blockTexture = loadTexture("Assets/Textures/blockTexture.jpg");
				glutIdleFunc(idlefunction);
				glutPassiveMotionFunc(passivefunction);
				glutKeyboardFunc(keyboardfunction);
				glutMouseFunc(mousefunction);

				glutMainLoop();
			}

			// if (x > left-side && x < right-side && y > bottom-side && y < top-side) of credits button
			else if ((x > (0.35 * xmax) && x < ((0.35 * xmax) + 400.0)) && ((ymax - y) > (0.25 * ymax) && (ymax - y) < ((0.25 * ymax) + 100.0)))
			{
				creditsSound->play2D("Assets/sounds/credits.mp3", true);
				scrolly = -1 * (ymax * 0.85); // ymax = 
				windows[3] = glutCreateWindow("Credits");
				glutSetWindow(windows[3]); // Change OpenGL context
				//glutFullScreen();
				currentWindow = 4;
				init(xmax, ymax);
				glutIdleFunc(idlefunction);
				glutDisplayFunc(creditsdisplay);
				glutKeyboardFunc(keyboardfunction);
				glutMainLoop();
			}
		}

		if (currentWindow == 2 && !pause)
		{
			if (!ballRel)
			{
				ballRel = true;
				posXB = posXP + (padsize / 2); // to center ball on paddle
				posYB = posYP + 35.0; // to center ball on top of paddle
				posZB = posZP; // 0
			}
		}
	}
}

//Keyboard function to pause or resume the game
void keyboardfunction(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '\r': // enter : return to menu display
		if (currentWindow == 3)
		{
			currentWindow = 1;
			score = 0;
			glutDestroyWindow(windows[2]);
			gamePlaySound->stopAllSounds();
			creditsSound->stopAllSounds();
			GameOver->stopAllSounds();
			victory->stopAllSounds();
		}
		break;
	case 'p': // pause or unpause at any point during gameplay
		pause = !pause;
		gamePlaySound->setAllSoundsPaused(pause);
		creditsSound->setAllSoundsPaused(pause);
	case 'P':
		gamePlaySound->setAllSoundsPaused(pause);
		creditsSound->setAllSoundsPaused(pause);
		break;
	case 'q': // exit game at any point
	case 'Q':
		exit(0);
	default:; // Do nothing
	}
}

void init(int w, int h)
{
	// sets the view port of the render scene to the window
	glViewport(0, 0, w, h);

	// glClearColor specifies the red, green, blue, and alpha 
	// assigns the white color to the display window's Background
	// Changes GL_COLOR_BUFFER_BIT to white (if arguments are 1.0,1.0,1.0,0) instead of default black
	// The color won't display until glClear(GL_COLOR_BUFFER_BIT) is called
	// glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClearColor(0.0f, 0.717f, 0.921f, 0.0f); // change default color to Cyan Color

	// glClear takes a single argument that is the bitwise OR of several values indicating which buffer is to be cleared.
	// by what assigned values
	// values used by glClear to clear the color buffers.Values specified by glClearColor are clamped to the range 0 1.
	// Although the glClearColor() command assigns a color to the display window
	// It does'nt put the display window on screen 
	// To get the assigned window color displayed, we need to invoke glClear(GL_COLOR_BUFFER_BIT); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Projection Matrix mode normalizes the coordinates on the screen
	// when using the projection matrix mode, The window size doesn't matter 
	// The center is (0,0) leftmost point (-1,0) and righmost point (1,0)
	// projecting the image on the screen
	// converts our matrix positions to normalized values
	// The projection matrix is used to create your viewing volume.
	glMatrixMode(GL_PROJECTION);

	// glLoadIdentity() is typically called immediately after a matrix mode change so you are starting "fresh" if you will.
	// not needed the first time but good practice
	glLoadIdentity();

	// glOrtho � multiply the current matrix with an orthographic matrix
	// default glOrtho is glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	// change coordinate system to (0,0) in bottom-left corner and cover the whole rendering area
	// gluOrtho2D(-xmax / 2, xmax / 2, -ymax / 2, ymax / 2); this zooms in 

	gluOrtho2D(0.0, xmax, 0.0, ymax);

}


void light(void)
{

	//glColor3f(1, 1, 1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	//glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5);
	//glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);
	//glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.2);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, -90);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
	//glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);


}
//Main function to call other functions

int main(int argc, char** argv)
{
	// Window Size
	xmax = 1366;
	ymax = 768;

	glutInit(&argc, argv); // Initializes GLUT Functions 

	// sets the window modes like buffering and coloring
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH); // with single buffer, gflush() must be used at the end
	glutInitWindowSize(xmax, ymax); // sets window size

	// The screen's origin point is (0,0) at the top left
	// The window's position relative to the sceen
	glutInitWindowPosition(75, 0);

	// int glutCreateWindow(char *name);
	// Creates a Window in the background with the previous specified features with the given caption on the title bar
	// Implicitly, the current (con	text) window is set to the newly created window.
	// Each created window has a unique associated OpenGL context. 
	// State changes to a window's associated OpenGL context can be done immediately after the window is created.
	// the window's display state is not actually acted upon until glutMainLoop is entered. This means until glutMainLoop is called, 
	// rendering to a created window is ineffective because the window can not yet be displayed.
	// The value returned is a unique small integer identifier for the window. 
	// The range of allocated identifiers starts at one. This window identifier can be used when calling glutSetWindow.
	windows[1] = glutCreateWindow("Menu");

	//glutFullScreen();
	glutDisplayFunc(menudisplay); // assigns the picture created by menudisplay function to the created window
	glutReshapeFunc(init); // Initializes default OpenGL context features 

	glutMouseFunc(mousefunction);
	glutKeyboardFunc(keyboardfunction);

	//glEnable(GL_LIGHTING);
	//glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.0);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, ambientLight);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	//glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,60.0f);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	//glMateriali(GL_FRONT, GL_SHININESS, 50);

	//light();

	glEnable(GL_TEXTURE_2D);

	menuDisplayTexture = loadTexture("Assets/Textures/menuDisplayTexture.jpg");
	logoTexture = loadTexture("Assets/Textures/logoTexture.jpg");
	newGameButtonTexture = loadTexture("Assets/Textures/newGameButtonTexture.png");
	creditsButtonTexture = loadTexture("Assets/Textures/creditsButtonTexture.png");
	glutMainLoop(); // Displays the created window on screen
}