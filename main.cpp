#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "texture.h"
#include "../GL/glew.h"
#include "../GL/glut.h""
#include "../shader_lib/shader.h"
#include "glm/glm.h"
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
extern "C"
{
	#include "glm_helper.h"
}

/*you may need to do something here
you may use the following struct type to perform your single VBO method,
or you can define/declare multiple VBOs for VAO method.
Please feel free to modify it*/
struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};
typedef struct Vertex Vertex;

//no need to modify the following function declarations and gloabal variables
void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void camera_light_ball_move();
GLuint loadTexture(char* name, GLfloat width, GLfloat height);

namespace
{
	char *obj_file_dir = "../Resources/Ball.obj";
	char *bunny_file_dir = "../Resources/bunny.obj";
	char *teapot_file_dir = "../Resources/teapot.obj";
	
	GLfloat light_rad = 0.05;//radius of the light bulb
	float eyet = 0.0;//theta in degree
	float eyep = 90.0;//phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;
	int mousex = 0;
	int mousey = 0;
}

// You can modify the moving/rotating speed if it's too fast/slow for you
const float speed = 0.03; // camera / light / ball moving speed
const float rotation_speed = 0.05; // ball rotating speed

//you may need to use some of the following variables in your program 

// No need for model texture, 'cause glmModel() has already loaded it for you.
// To use the texture, check glmModel documentation.
GLuint mainTextureID; // TA has already loaded this texture for you
GLuint noiseTextureID; // TA has already loaded this texture for you
GLuint rampTextureID; // TA has already loaded this texture for you

GLMmodel *model, *bunnyModel, *teapotModel; //TA has already loaded the model for you(!but you still need to convert it to VBO(s)!)

float eyex = 0.0;
float eyey = 0.64;
float eyez = 3.0;

GLfloat light_pos[] = { 1.1, 1.0, 1.3 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };

#define deltaTime (10) // in ms (1e-3 second)

GLuint vboID, fboID[3];
GLuint program;
GLuint renderTexture[3];
int windowWidth = 512, windowHeight = 512;
bool isScreenShoot = false;

Texture texture;
float textureWidth = 512, textureHeight = 512;
float avgWidth = 20, avgHeight = 20;

vector<TextureRectangle> rectangles;

void Tick(int id)
{
	double d = deltaTime / 1000.0;

	glutPostRedisplay();
	glutTimerFunc(deltaTime, Tick, 0); // 100ms for passTime step size
}

void InitParameter(std::string filename)
{
    srand(time(NULL));
    std::ifstream fin;
    fin.open(filename);

    if (!fin) {
        std::cout << "File Not Exist.\n";
        return;
    }

    std::string input;
    while (fin >> input) {
        if (input == "TEXTURE_SIZE") {
            fin >> textureWidth >> textureHeight;
        }
        else if (input == "AVERAGE_SIZE") {
            fin >> avgWidth >> avgHeight;
        }
        else {
            std::cout << "Wrong Format.\n";
            return;
        }
    }

    texture = Texture(textureWidth, textureHeight);
    texture.BuildBasicJointPattern(avgWidth, avgHeight);

    rectangles = texture.GetRectangles();
}


int main(int argc, char *argv[])
{
    InitParameter("config.txt");
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// remember to replace "YourStudentID" with your own student ID
	glutCreateWindow("CG_HW3_0656651");
	glutReshapeWindow(windowWidth, windowHeight);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutTimerFunc(deltaTime, Tick, 0); //pass Timer function

	glutMainLoop();

	glmDelete(model);
	return 0;
}


void init(void)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);

    // FBO
    glGenFramebuffers(3, fboID);
    glGenTextures(3, renderTexture);

    // Bind Textures

    for (int i = 0; i < 3; i++) {
        
        glBindTexture(GL_TEXTURE_2D, renderTexture[i]);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
            512, 512, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        //    512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, fboID[i]);
        //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture[i], 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderTexture[i], 0);
        glBindTexture(GL_TEXTURE_2D, NULL);
        glBindFramebuffer(GL_FRAMEBUFFER, NULL);
    }
    
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("WRONG!!!!\n");
    }

}


void screenshot(char filename[160], int x, int y)
{   
    // get the image data
    long imageSize = x * y * 3;
    unsigned char *data = new unsigned char[imageSize];
    glReadPixels(0, 0, x, y, GL_BGR, GL_UNSIGNED_BYTE, data);
    // split x and y sizes into bytes
    int xa = x % 256;
    int xb = (x - xa) / 256; int ya = y % 256;
    int yb = (y - ya) / 256;//assemble the header
    unsigned char header[18] = { 0,0,2,0,0,0,0,0,0,0,0,0,(char)xa,(char)xb,(char)ya,(char)yb,24,0 };
    // write header and data to file
    std::fstream File(filename, std::ios::out | std::ios::binary);
    File.write(reinterpret_cast<char *>(header), sizeof(char) * 18);
    File.write(reinterpret_cast<char *>(data), sizeof(char)*imageSize);
    File.close();

    delete[] data;
    data = NULL;
}

void drawRectangles()
{
    //cout << rectangles.size() << endl;
    //for (int i = 0; i < rectangles.size(); i++) {
    //    TextureRectangle rect = rectangles[i];

    //    glColor3f(0, 1, 0);
    //    glBegin(GL_QUADS);
    //    glVertex2f(rect.minX, rect.minY);
    //    glVertex2f(rect.minX, rect.maxY);
    //    glVertex2f(rect.maxX, rect.maxY);
    //    glVertex2f(rect.maxX, rect.minY);
    //    glEnd();
    //    cout << rect.minX << " " << rect.maxX << " " << rect.minY << " " << rect.maxY << endl;
    //}

    //for (int i = 0; i < rectangles.size(); i++) {
    //    TextureRectangle rect = rectangles[i];

    //    glColor3f(1, 0, 0);
    //    glBegin(GL_LINE_STRIP);
    //    glVertex2f(rect.minX, rect.minY);
    //    glVertex2f(rect.minX, rect.maxY);
    //    glVertex2f(rect.maxX, rect.maxY);
    //    glVertex2f(rect.maxX, rect.minY);
    //    glVertex2f(rect.minX, rect.minY);
    //    glEnd();
    //}

    /*for (int i = 0; i < texture.edges.size(); i++) {
        Edge e = texture.edges[i];

        glColor3f(1, 0, 0);
        glBegin(GL_LINE_STRIP);
        glVertex2f(texture.points[e.uIndex].x, texture.points[e.uIndex].y);
        glVertex2f(texture.points[e.vIndex].x, texture.points[e.vIndex].y);
        glEnd();

    }*/

    for (int i = 0; i < texture.polygons.size(); i++) {
        TexturePolygon &poly = texture.polygons[i];
        for (int j = 0; j < poly.edges.size(); j++) {
            Edge &e = texture.edges[poly.edges[j]];
            glColor3f(0, 0, 1);
            glBegin(GL_LINE_STRIP);
            glVertex2f(texture.points[e.uIndex].x, texture.points[e.uIndex].y);
            glVertex2f(texture.points[e.vIndex].x, texture.points[e.vIndex].y);
            glEnd();
        }
    }
    //system("PAUSE");
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glPointSize(10);
    //glBegin(GL_POINTS);
    //glVertex2f(0.0 ,0.0);
    //glEnd();

    //glBegin(GL_LINES);
    //glVertex2f(0.0, 0.0);
    //glVertex2f(5.0, 5.0);
    //glEnd();

    drawRectangles();

    if (isScreenShoot) {
        isScreenShoot = false;
        screenshot("test.tga", 512, 512);
        std::cout << "Save.\n";
    }
	glutSwapBuffers();
	camera_light_ball_move();
}

//please implement mode toggle(switch mode between phongShading/Dissolving/Ramp) in case 'b'(lowercase)
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	//ESC
		break;
	}
	case 'b'://toggle mode
	{
		//you may need to do somting here
		break;
	}
	case 'd':
	{

		break;
	}
	case 'a':
	{

		break;
	}
	case 'w':
	{

		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		lright = true;
		break;
	}
	case 'f':
	{
		lleft = true;
		break;
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos[1] = eyey + sin(eyet*M_PI / 180);
		light_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://move ball to front of camera
	{
		ball_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) * 3;
		ball_pos[1] = eyey + sin(eyet*M_PI / 180) * 5;
		ball_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180) * 3;
		break;
	}
	case 'c'://reset all pose
	{
		light_pos[0] = 1.1;
		light_pos[1] = 1.0;
		light_pos[2] = 1.3;
		ball_pos[0] = 0;
		ball_pos[1] = 0;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 0.0;
		eyey = 0.64;
		eyez = 3.0;
		eyet = 0;
		eyep = 90;
		break;
	}
	default:
	{
		break;
	}
	}
}

//no need to modify the following functions
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
    gluOrtho2D(0.0, textureWidth, 0.0, textureHeight);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	if (mleft)
	{
		eyep -= (x-mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}

void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz=0;
	if(lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		light_pos[1] += dy*sin(eyet*M_PI / 180);
		light_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (lup)
			light_pos[1] += speed;
		else if(ldown)
			light_pos[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		ball_pos[1] += dy*sin(eyet*M_PI / 180);
		ball_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if(bx||by||bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}

void keyboardup(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':
	{
		
		break;
	}
	case 'a':
	{
		break;
	}
	case 'w':
	{
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}
    case '1':
    {
        isScreenShoot = true;
        break;
    }
	default:
	{
		break;
	}
	}
}

void idle(void)
{
	glutPostRedisplay();
}

GLuint loadTexture(char* name, GLfloat width, GLfloat height)
{
	return glmLoadTexture(name, false, true, true, true, &width, &height);
}
