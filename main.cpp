/*
 * Solar.c
 *
 * Program to demonstrate how to use a local
 * coordinate method to position parts of a
 * model in relation to other model parts.
 *
 * Draws a simple solar system, with a sun, planet and moon.
 * Based on sample code from the OpenGL programming guide
 *		by Woo, Neider, Davis.  Addison-Wesley.
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 * USAGE:
 *    Press "r" key to toggle (off and on) running the animation
 *    Press "s" key to single-step animation
 *    The up arrow key and down array key control the
 *			time step used in the animation rate.  Each key
 *			press multiplies or divides the times by a factor
 *			of two (2).
 *	  Press ESCAPE to exit.
 *
 */

#include "Solar.hpp"
#include <stdlib.h>
#include <cmath>
#include <GL/glut.h> // OpenGL Graphics Utility Library
#include "getBMP.h"
#include <iostream>

static GLenum spinMode = GL_TRUE;
static GLenum singleStep = GL_FALSE;

// Year
const GLfloat MERCURY_YEAR = 88.0;
const GLfloat VENUS_YEAR = 225.0;
const GLfloat EARTH_YEAR = 365.0;
const GLfloat MARS_YEAR = 687.0;
const GLfloat JUPITER_YEAR = 4332.0;
const GLfloat SATURN_YEAR = 29.5 * 365.0;
const GLfloat URANUS_YEAR = 84.0 * 365.0;
const GLfloat NEPTUNE_YEAR = 165.0 * 365.0;

// Day
const GLfloat MERCURY_DAY = 58.7;
const GLfloat VENUS_DAY = 243.0;
const GLfloat EARTH_DAY = 1.0;
const GLfloat MARS_DAY = 24.6 / 24;
const GLfloat JUPITER_DAY = 9.83 / 24;
const GLfloat SATURN_DAY = 10.23 / 24;
const GLfloat URANUS_DAY = 17.23 / 24;
const GLfloat NEPTUNE_DAY = 16.1 / 24;

// These three variables control the animation's state and speed.
static float HourOfDay = 0.0;
static float DayOfYear = 0.0;
static float AnimateIncrement = 24.0; // Time step for animation (hours)

GLfloat WHITE[] = {1, 1, 1};
GLfloat RED[] = {1, 0, 0};
GLfloat GREEN[] = {0, 1, 0};
GLfloat MAGENTA[] = {1, 0, 1};

bool ambientEnabled = true;
bool diffuseEnabled = true;
bool specularEnabled = true;


void SetupLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightOff[] = {0.0f, 0.0f, 0.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientEnabled ? lightAmbient : lightOff);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseEnabled ? lightDiffuse : lightOff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularEnabled ? lightSpecular : lightOff);

    GLfloat lightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f}; // Point light at Sun
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}



// glutKeyboardFunc is called below to set this function to handle
//		all normal key presses.
static void KeyPressFunc(unsigned char Key, int x, int y)
{
	switch (Key)
	{
	case 'R':
	case 'r':
		Key_r();
		break;
	case 's':
	case 'S':
		Key_s();
		break;
	case 'a':
	case 'A':
      ambientEnabled = !ambientEnabled;
	  std::cout << "ambientEnabled: " << ambientEnabled << std::endl;
      break;
    case '1':
      specularEnabled = !specularEnabled;
	  std::cout << "specularenabled: " << specularEnabled << std::endl;
      break;
    case 'd': 
	case 'D':
      diffuseEnabled = !diffuseEnabled;
	  std::cout << "diffuseEnabled: " << diffuseEnabled << std::endl;
      break;
	case 27: // Escape key
		exit(1);
	}
	glutPostRedisplay();
}

// glutSpecialFunc is called below to set this function to handle
//		all special key presses.  See glut.h for the names of
//		special keys.
static void SpecialKeyFunc(int Key, int x, int y)
{
	switch (Key)
	{
	case GLUT_KEY_UP:
		Key_up();
		break;
	case GLUT_KEY_DOWN:
		Key_down();
		break;
	}
}

static void Key_r(void)
{
	if (singleStep)
	{ // If ending single step mode
		singleStep = GL_FALSE;
		spinMode = GL_TRUE; // Restart animation
	}
	else
	{
		spinMode = !spinMode; // Toggle animation on and off.
	}
}

static void Key_s(void)
{
	singleStep = GL_TRUE;
	spinMode = GL_TRUE;
}

static void Key_up(void)
{
	AnimateIncrement *= 2.0; // Double the animation time step
}

static void Key_down(void)
{
	AnimateIncrement /= 2.0; // Halve the animation time step
}

/*
 * Animate() handles the animation and the redrawing of the
 *		graphics window contents.
 */
static void Animate(void)
{

	SetupLighting();
	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (spinMode)
	{
		// Update the animation state
		HourOfDay += AnimateIncrement;
		DayOfYear += AnimateIncrement / 24.0;

		//HourOfDay = HourOfDay - ((int)(HourOfDay / 24)) * 24;
		//DayOfYear = DayOfYear - ((int)(DayOfYear / 365)) * 365;
	}

	// Clear the current matrix (Modelview)
	glLoadIdentity();

	// Back off eight units to be able to view from the origin.
	glTranslatef(0.0, 0.0, -8.0);

	// Rotate the plane of the elliptic (rotate the model's plane about the x axis by fifteen degrees)
	glRotatef(15.0, 1.0, 0.0, 0.0);

	// Draw the sun as a yellow, wireframe sphere
	//glColor3f(.0, 1.0, 0.0);
	imageFile* sunTexture = getBMP("images/sun.bmp");
    glEnable(GL_TEXTURE_2D);

    GLuint sunTextureID;
    glGenTextures(1, &sunTextureID);
    glBindTexture(GL_TEXTURE_2D, sunTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sunTexture->width, sunTexture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, sunTexture->data);

    GLUquadric* sunQuad = gluNewQuadric();
    gluQuadricTexture(sunQuad, GL_TRUE);
    gluSphere(sunQuad, 0.4, 20, 20);

    gluDeleteQuadric(sunQuad);
    glDeleteTextures(1, &sunTextureID);
    glDisable(GL_TEXTURE_2D);

    delete[] sunTexture->data;
    delete sunTexture;

	struct Planet
	{
		float distance;
		float year;
		float day;
		float size;
		float r, g, b;
		std::string image;
	};

	Planet planets[] = {
		{0.579, MERCURY_YEAR, MERCURY_DAY, 0.1, 0.5, 0.5, 0.5, "images/mercury.bmp"},
		{1.082, VENUS_YEAR, VENUS_DAY, 0.12, 0.9, 0.6, 0.1, "images/venus.bmp"},
		{1.496, EARTH_YEAR, EARTH_DAY, 0.13, 0.2, 0.2, 1.0, "images/earth.bmp"},
		{2.28, MARS_YEAR, MARS_DAY, 0.07, 1.0, 0.0, 0.0, "images/mars.bmp"},
		{7.79, JUPITER_YEAR, JUPITER_DAY, 0.3, 1.0, 0.5, 0.0, "images/jupiter.bmp"},
		{14.27, SATURN_YEAR, SATURN_DAY, 0.25, 1.0, 1.0, 0.5, "images/saturn.bmp"},
		{28.71, URANUS_YEAR, URANUS_DAY, 0.2, 0.5, 0.5, 1.0, "images/uranus.bmp"},
		{44.97, NEPTUNE_YEAR, NEPTUNE_DAY, 0.18, 0.3, 0.3, 0.8, "images/neptune.bmp"}};

	for (auto &planet : planets)
	{
		imageFile* texture;
		texture = getBMP(planet.image);
		glPushMatrix();
		glRotatef(360.0 * DayOfYear / planet.year, 0.0, 1.0, 0.0);
		glTranslatef(planet.distance, 0.0, 0.0);				  
		glRotatef(360.0 * HourOfDay / planet.day, 0.0, 1.0, 0.0); 
		glEnable(GL_TEXTURE_2D);

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);		

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);

		GLUquadric* quad = gluNewQuadric();
		gluQuadricTexture(quad, GL_TRUE);
		gluSphere(quad, planet.size, 20, 20);

		gluDeleteQuadric(quad);
		glDeleteTextures(1, &textureID);
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();

		delete[] texture->data;
		delete texture;
		glPopMatrix();
	}

	glPushMatrix();
	glRotatef(360.0 * DayOfYear / EARTH_YEAR, 0.0, 1.0, 0.0);
	glTranslatef(1.496, 0.0, 0.0);
	glRotatef(360.0 * 12.0 * DayOfYear / 365.0, 0.0, 1.0, 0.0);
	glTranslatef(0.2, 0.0, 0.0);
	glutWireSphere(0.05, 5, 5);
	glPopMatrix();

	// Flush the pipeline, and swap the buffers
	glFlush();
	glutSwapBuffers();

	if (singleStep)
	{
		spinMode = GL_FALSE;
	}

	glutPostRedisplay(); // Request a re-draw for animation purposes
}

// Initialize OpenGL's rendering modes
void OpenGLInit(void)
{

	glShadeModel(GL_FLAT);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glLightfv(GL_LIGHT0, GL_AMBIENT, WHITE);    // Ambient light
    glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);    // Diffuse light
    glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);   // Specular light
	const GLfloat LIGHT_POSITION[] = {0.0f, 0.0f, 0.0f, 1.0f}; // Point light
    glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POSITION);
	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set up material properties for planets and other objects
    glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);  // Specular reflection
    glMaterialfv(GL_FRONT, GL_AMBIENT, WHITE);   // Ambient reflection
    glMaterialfv(GL_FRONT, GL_DIFFUSE, WHITE);   // Diffuse reflection
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);  // Shininess factor (higher = shinier)

    // Set background color and clear depth
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    glClearDepth(1.0f);

    // Enable smooth shading for better visuals
    glShadeModel(GL_SMOOTH);
}

// ResizeWindow is called when the window is resized
static void ResizeWindow(int w, int h)
{
	float aspectRatio;
	h = (h == 0) ? 1 : h;
	w = (w == 0) ? 1 : w;
	glViewport(0, 0, w, h); // View port uses whole window
	aspectRatio = (float)w / (float)h;

	// Set up the projection view matrix (not very well!)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, aspectRatio, 1.0, 30.0);

	// Select the Modelview matrix
	glMatrixMode(GL_MODELVIEW);
}



// Main routine
// Set up OpenGL, hook up callbacks, and start the main loop
int main(int argc, char **argv)
{
	// Need to double buffer for animation
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Create and position the graphics window
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(600, 360);
	glutCreateWindow("Systeme Solaire");

	// Initialize OpenGL.
	OpenGLInit();

	// Set up callback functions for key presses
	glutKeyboardFunc(KeyPressFunc);
	glutSpecialFunc(SpecialKeyFunc);


	// Set up the callback function for resizing windows
	glutReshapeFunc(ResizeWindow);

	// Callback for graphics image redrawing
	glutDisplayFunc(Animate);

	// Start the main loop.  glutMainLoop never returns.
	glutMainLoop();

	return (0); // Compiler requires this to be here. (Never reached)
}
