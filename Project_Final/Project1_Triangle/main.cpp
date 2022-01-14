#include "MyObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>

#include <commdlg.h>
#include <windows.h>
#include <ShlObj.h>
#include <stdio.h>
#include <atlstr.h> 
using namespace std;

#define pickBufferSize 32

//���ڲ���
GLfloat window_height = 600.0f;
GLfloat window_width = 600.0f;
//UI������
GLfloat ui_height = 100.0f;
GLfloat ui_width = 600.0f;

//ģ��·��
string filePath = "./Objects/Cube.obj";

//ʵ���ƶ����۲�ģ���������
static float c = 3.1415926 / 180.0f;
static float r = 40.0f;
static int degreeX = 90;
static int degreeY = 0;
static int oldPosY = -1;
static int oldPosX = -1;
//���λ��
static int cameraX = 0;
static int cameraY = 10;
static int cameraZ = 40;

//��������
static float angle = 0.0;//��y����ת��
static float spanSpeed = 1.0f;	//��ת����
static float ratio;//���ڱ���
static float x = 0.0, y = 1.75f, z = 5.0f;//���λ��
static float lx = 0.0f, ly = 0.0f, lz = -1.0f;//�������������

//2D�༭ģʽ��������
static float r_2D = 20.0f;

//�л��༭ģʽ�͹���ģʽ
enum SceneMode
{
	EDIT,
	VIEW
};
SceneMode sceneMode = VIEW;

//�ο����������
GLfloat axisPoints[3][2][3] = {
	{{0.0f, 0.0f, 0.0f},
	{100.0f, 0.0f, 0.0f}},

	{{0.0f, 0.0f, 0.0f},
	{0.0f, 100.0f, 0.0f}},

	{{0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 100.0f}} 
};
//��������
GLfloat axisSize = 2;

//���������Ƶ� 
GLint numOfPoints = 4; 
static GLfloat controlPoints[4][3] = 
{{0.0f, 10.0f, 0.0f},
{8.0f, 10.0f, 0.0f},
{ 6.0f, 4.0f, 0.0f },
{ 4.0f, 0.0f, 0.0f }};
//������������Ƶ�
static GLfloat texturePoints[2][2][2] =
{
{{0.0, 0.0}, {0.0, 1.0}},
{{0.5, 0.0}, {0.5, 1.0}}
};
//��������
static GLfloat position[3] =
{
	10.0, 0.0, 0.0
};

vector<MyObject> myObjects;
int edit_object = 0;

//���ڱ��ƶ��ĵ�
int editPoint = -1;
bool editingPoint = false;
//��ͼ����
unsigned int texture;
bool useSkybox01 = true;
unsigned int SkyboxTexture01[6];
unsigned int SkyboxTexture02[6];
unsigned int leafTexture;
unsigned int flowerTexture;

//����ϵͳ����/�ر�
bool enablePaticle = true;

//��Դ����/�ر�
bool enableLight = true;
//��Դ����
static float light_r = 100.0f;
static int light_degreeX = 90;
static int light_degreeY = 0;
static float light_red = 0.0f;
static float light_green = 0.0f;
static float light_blue = 0.0f;
GLfloat sun_light_position[] = { light_r*cos(c*light_degreeY)*cos(c*light_degreeX), light_r*sin(c*light_degreeY), light_r*cos(c*light_degreeY)*sin(c*light_degreeX), 1.0f }; //��Դ��λ��
GLfloat sun_light_ambient[] = { light_red, light_green, light_blue, 1.0f }; //RGBAģʽ�Ļ����⣬Ϊ0
GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //RGBAģʽ��������⣬ȫ�׹�
GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  //RGBAģʽ�µľ���� ��ȫ�׹�


void drawScene(GLenum mode);
//������ͼ
void loadTexture(const char* filepath);
//����Demo
void TextureDemoInit(void);
void drawTextureDemo(void);
//��պ�
void initSkybox(void);
void drawSkybox(void);

/* ����������� */
const unsigned int MAP_WIDTH = 512;
const unsigned int CELL_WIDTH = 16;
const unsigned int MAP = MAP_WIDTH * CELL_WIDTH / 2;


//void initUI() {
//	pBtn = new Button;
//	pBtn->m_bPressed = false;
//	pBtn->m_fPosX = 0;
//	pBtn->m_fPosY = 0;
//	pBtn->m_fWidth = 60;
//	pBtn->m_fHeight = 40;
//}

//���ù�Դ
void setLightRes() {
	//sun_light_position[] = { light_r*cos(c*light_degreeY)*cos(c*light_degreeX), light_r*sin(c*light_degreeY), light_r*cos(c*light_degreeY)*sin(c*light_degreeX), 1.0f }; //��Դ��λ��
	sun_light_position[0] = light_r * cos(c*light_degreeY)*cos(c*light_degreeX);
	sun_light_position[1] = light_r * sin(c*light_degreeY);
	sun_light_position[2] = light_r * cos(c*light_degreeY)*sin(c*light_degreeX);

	sun_light_ambient[0] = light_red;
	sun_light_ambient[1] = light_green;
	sun_light_ambient[2] = light_blue;

	glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

	glEnable(GL_LIGHTING); //���ù�Դ
	glEnable(GL_LIGHT0);   //ʹ��ָ���ƹ�
	glEnable(GL_COLOR_MATERIAL);	//ʹ��ɫֱ�������ڲ�����

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); //���������

}

//���ò���
void SetMaterial() {
	//����
	GLfloat mat_ambient[] = { 0.2,0.2,0.2,1.0 };
	GLfloat mat_diffuse[] = { 0.8,0.3,0.3,1.0 };
	GLfloat mat_specular[] = { 1.0,0.5,0.5,1.0 };
	GLfloat mat_shiness[] = { 50.0 };
	/*���������������*/
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shiness);
}

//���껻��
//������Ļ����õ��ӵ�ռ�����
void Get3Dpos(float x, float y, float &px , float &py , float &pz) {
	//cout << "input pos: " << x << ", " << y << endl;
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble object_x, object_y, object_z;
	float mouse_x = x;
	float mouse_y = y;
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)mouse_x;
	winY = (float)viewport[3] - (float)mouse_y - 1.0f;
	glReadBuffer(GL_BACK);
	glReadPixels(mouse_x, float(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	gluUnProject((GLdouble)winX, (GLdouble)winY, (GLdouble)winZ, modelview, projection, viewport, &object_x, &object_y, &object_z);
	px = object_x;
	py = object_y;
	pz = object_z;
}


//��ʼ��
void init() {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("ObjLoader");
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	//�������ù�Դ
	setLightRes();
	glEnable(GL_DEPTH_TEST);
	//ͼ���ܰѹⷴ�䵽��������ʹ�������ܹ����չ��գ�
	glEnable(GL_AUTO_NORMAL);

	//MyObject myObj(ModelType::TEA_CUP);
	//loadTexture("./Textures/02.jpg");
	//myObj.addTexture(texture);

	//myObjects.push_back(myObj);
	//myObj.moveObject(10, 0, 0);
	//myObjects.push_back(myObj);

	//TextureDemoInit();
	initSkybox();
	//ui��ʼ��
	//initUI();

}

//����������
void drawAxis() {
	glLineWidth(axisSize);
	glColor3f(0.0, 1.0, 1.0);
	glBegin(GL_LINES);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 2; j++)
			glVertex3fv(axisPoints[i][j]);
	glEnd();

}

void display()
{
	switch (sceneMode)
	{
	case EDIT:
		//glViewport(0, 0, window_width, window_height);

		glClearColor(1.0, 1.0, 1.0, 1.0);  //�ı�Ĭ�ϱ�������ɫΪǳ��ɫ��Ҫд��glLoadIdentity()ǰ
		glColor3f(1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
			gluLookAt(0, 0, r_2D, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f); //�������λ��
			drawAxis();
			//drawBer2D();
			myObjects[edit_object].drawLines();
			
		glPopMatrix();
		//glutSwapBuffers();
		break;
	case VIEW:
		drawScene(GL_RENDER);
		//glutSwapBuffers();


		break;
	default:
		break;
	}
	glutSwapBuffers();

	
}

void reshape(int width, int height)
{
	cout << "DO reshape" << endl;
	glViewport(0, 0, width, height);
	window_width = width;
	window_height = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//�޸���Ⱦ����
	gluPerspective(60.0f, (GLdouble)width / (GLdouble)height, 1.0f, 10000.0f);
	glMatrixMode(GL_MODELVIEW);
}

//��������¼�
void keyBoardInput(unsigned char key, int x, int y) {

	switch (key)
	{
	case 9:
		//Tab�¼�
		if (myObjects.size() == 0) break;
		if (sceneMode == VIEW) sceneMode = EDIT;
		else sceneMode = VIEW;
		break;
	case 'x': {
		int mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT) {
			cout << "Pressing alt + X" << endl;
			myObjects[edit_object].moveObject(-1.0, 0.0, 0.0);
		}
		else {
			cout << "Pressing X" << endl;
			myObjects[edit_object].moveObject(1.0, 0.0, 0.0);
		}
		break;
	}
	case 'y': {
		int mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT) {
			cout << "Pressing alt + Y" << endl;
			myObjects[edit_object].moveObject(0.0, -1.0, 0.0);
		}
		else {
			cout << "Pressing Y" << endl;
			myObjects[edit_object].moveObject(0.0, 1.0, 0.0);
		}
		break;
	}
	case 'z': {
		int mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT) {
			cout << "Pressing alt + Z" << endl;
			myObjects[edit_object].moveObject(0.0, 0.0, -1.0);
		}
		else {
			cout << "Pressing Z" << endl;
			myObjects[edit_object].moveObject(0.0, 0.0, 1.0);
		}
		break;
	}
	case 'q': {
		cout << "Pressing Q" << endl;
		light_degreeX += 5;
		//cout << "Sunlight Position" << endl;
		//for (int i = 0; i < 3; i++) {
		//	cout << sun_light_position[i] << ", ";
		//}cout << endl;
		break;
	}
	case 'e': {
		cout << "Pressing E" << endl;
		light_degreeX -= 5;
		//cout << "Sunlight Position" << endl;
		//for (int i = 0; i < 3; i++) {
		//	cout << sun_light_position[i] << ", ";
		//}cout << endl;
		break;
	}
	//��Դ��ɫ����
	case 'r': {
		int mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT) {
			cout << "light red -" << endl;
			light_red -= 0.1;
		}
		else {
			light_red += 0.1;
			cout << "light red +" << endl;
		}
		if (light_red < 0) light_red = 0;
		if (light_red > 1.0f) light_red = 1.0f;
		break;
	}
	case 'g': {
		int mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT) {
			light_green -= 0.1;
		}
		else {
			light_green += 0.1;
		}
		if (light_green < 0) light_green = 0;
		if (light_green > 1.0f) light_green = 1.0f;
		break;
	}
	case 'b': {
		int mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT) {
			light_blue -= 0.1;
		}
		else {
			light_blue += 0.1;
		}
		if (light_blue < 0) light_blue = 0;
		if (light_blue > 1.0f) light_blue = 1.0f;
		break;
	}
	case 's': {
		int mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT) {
			myObjects[edit_object].scale -= 0.1;
			if (myObjects[edit_object].scale < 0.2) myObjects[edit_object].scale = 0.2;
		}
		else {
			myObjects[edit_object].scale += 0.1;
		}
		
		break;
	}
	case 'd': {
		vector<MyObject>::iterator  it = myObjects.begin() + edit_object;
		myObjects.erase(it);
		edit_object = 0;
		break;
	}
	//�����ð���
	case 'p': {
		MAGIC += 0.01;
		cout << MAGIC << endl;
		break;
	}
	default:
		break;
	}
}

//ʰȡ������
void processPicks(GLint nPicks, GLuint pickBuffer[]) {
	GLint j, k;
	GLuint objID, *ptr;
	printf(" Number of objects picked = %d \n", nPicks);
	printf(" \n");
	ptr = pickBuffer;

	for (j = 0; j < nPicks; j++) {
		objID = *ptr;

		printf(" Stack position = %d \n", objID);
		ptr++;

		printf(" Min depth = %g, ", float(*ptr / 0x7fffffff));
		ptr++;

		printf(" Max depth = %g, ", float(*ptr / 0x7fffffff));
		ptr++;

		int target = *ptr;
		//�л����ڱ༭��Ʒ
		edit_object = target;

		printf(" Stack IDs are: \n");
		for (k = 0; k < objID; k++) {
			printf(" %d ", *ptr);
			ptr++;
		}
		printf("\n\n");
	}
}

//������¼�
void moseClick(int button, int state, int x, int y)
{
	switch (sceneMode)
	{
	case EDIT:
		if (state == GLUT_DOWN) {
			cout << "Click in EDIT mode" << endl;
			float px, py, pz;
			//ת������ϵ
			Get3Dpos(x, y, px, py, pz);

			editingPoint = myObjects[edit_object].getPoint(px, py);
			//for (int i = 0; i < numOfPoints; i++) {
			//	if (getDistance(controlPoints[i][0], controlPoints[i][1], px, py) < 1.0f) {
			//		cout << "Clicking point " << i << endl;
			//		editPoint = i;
			//		break;
			//	}
			//}
		}
		if (state == GLUT_UP) {
			//if (editPoint == -1) break;
			if (!editingPoint) break;
			float px, py, pz;
			//ת������ϵ
			Get3Dpos(x, y, px, py, pz);
			cout << "End move at " << px << ", "<< py << endl;
			//�Զ�����
			if (abs(px) < 0.6) px = 0;
			//controlPoints[editPoint][0] = px;
			//controlPoints[editPoint][1] = py;
			//editPoint = -1;
			myObjects[edit_object].editLine(px, py);
			myObjects[edit_object].editEnd();
			editingPoint = false;
		}
		break;
	case VIEW:
		cout << "Click in VIEW mode" << endl;
		if (state == GLUT_DOWN) {
			oldPosX = x; oldPosY = y;
		}

		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

			GLuint pickBuffer[pickBufferSize];
			GLint nPicks, vpArray[4];

			glSelectBuffer(pickBufferSize, pickBuffer);
			glRenderMode(GL_SELECT);
			glInitNames();

			//Save current
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();			

			glGetIntegerv(GL_VIEWPORT, vpArray);
			gluPickMatrix(GLdouble(x), GLdouble(vpArray[3] - y), 5.0, 5.0, vpArray);
			gluPerspective(60.0f, (GLdouble)window_width / (GLdouble)window_height, 1.0f, 10000.0f);
			//gluOrtho2D(-300.0, 300.0, -300.0, 300.0);
			drawScene(GL_SELECT);

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glFlush();

			nPicks = glRenderMode(GL_RENDER);

			processPicks(nPicks, pickBuffer);
			
			glutPostRedisplay();
		}

		break;
	default:
		break;
	}
	
}
//��������
void mouseWheel(int button, int dir, int x, int y) 
{
	switch (sceneMode)
	{
	case EDIT:
		if (dir > 0)
		{
			if (r_2D > 0.05f)
				r_2D -= 0.05f;
		}
		else
		{
			r_2D += 0.05f;
		}
		break;
	case VIEW: {
		int mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT) {
			//������Դ
			if (dir > 0)
			{
				cout << "Wheeling light" << endl;
				light_degreeY += 5;
				if (degreeY > 89) degreeY = 89;
				if (degreeY < -89) degreeY = -89;
			}
			else {
				cout << "Wheeling light" << endl;
				light_degreeY -= 5;
				if (degreeY > 89) degreeY = 89;
				if (degreeY < -89) degreeY = -89;
			}
		}
		else {
			//�����ӽ�
			if (dir > 0)
			{
				if (r > 0.5f)
					r -= 0.5f;
			}
			else
			{
				r += 0.5f;
			}
		}

		
		break;
	}
	default:
		break;
	
	}
	return;
}

void changeViewPoint(int x, int y)
{
	switch (sceneMode)
	{
	case EDIT:
		//if (editPoint == -1) break;
		if (!editingPoint) break;
		float px, py, pz;
		//ת������ϵ
		Get3Dpos(x, y, px, py, pz);
		//cout << "End move at " << px << ", " << py << endl;
		//controlPoints[editPoint][0] = px;
		//controlPoints[editPoint][1] = py;
		myObjects[edit_object].editLine(px, py);
		break;
	case VIEW: {
		//cout << "Changing in VIEW mode" << endl;
		int tempX = x - oldPosX;
		int tempY = y - oldPosY;
		degreeX += tempX;
		degreeY += tempY;
		if (degreeY > 89.9) degreeY = 89.9;
		if (degreeY < -89.9) degreeY = -89.9;
		oldPosX = x;
		oldPosY = y;
	}
	default:
		break;
	}
}

void myIdle()
{
	glutPostRedisplay();
}

//#pragma region Demo for Berz
////��������
////���Ƶ�  
//GLint nNumPoints = 2;
//GLfloat ctrlPoints[4][4][3] =
//{
//{{ -1.5, -1.5, 4.0}, { -0.5, -1.5, 2.0},
//{0.5, -1.5, -1.0}, {1.5, -1.5, 2.0}},
//{{ -1.5, -0.5, 1.0}, { -0.5, -0.5, 3.0},
//{0.5, -0.5, 0.0}, {1.5, -0.5, -1.0}},
//{{ -1.5, 0.5, 4.0}, { -0.5, 0.5, 0.0},
//{0.5, 0.5, 3.0}, {1.5, 0.5, 4.0}},
//{{ -1.5, 1.5, -2.0}, { -0.5, 1.5, -2.0},
//{0.5, 1.5, 0.0}, {1.5, 1.5, -1.0}}
//};
//GLfloat texpts[2][2][2] =
//{
//{{0.0, 0.0}, {0.0, 1.0}},
//{{1.0, 0.0}, {1.0, 1.0}}
//};// ������Ƶ�
//
//void DrawPoints(void){ 
//	int i, j;
//	glColor3f(1.0f, 0.0f, 0.0f); //�ѵ�Ŵ�һ�㣬���ø����  
//	glPointSize(15.0f);
//	glBegin(GL_POINTS);
//	for (i = 0; i < nNumPoints; i++)
//		for (j = 0; j < 5; j++)
//			glVertex3fv(ctrlPoints[i][j]);
//	glEnd();
//}
//void drawBer(void)
//{
//	// Clear the window with current clearing color 
//	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	// ����ģ����ͼ����  
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//
//	glColor3f(1.0f, 1.0f, 1.0f); //����ӳ�䷽ʽ��ֻ��Ҫ����һ�ο�����SetupRC�е��á�  
//	//glMap2f(GL_MAP2_VERTEX_3, //���ɵ���������  
//	//	0.0f, // u���½� 
//	//	10.0f, //u���Ͻ�  
//	//	3, //�����е�ļ��  
//	//	4, //u�����ϵĽ�  
//	//	0.0f, //v���½�  
//	//	10.0f, //v���Ͻ�  
//	//	12, // ���Ƶ�֮��ļ��  
//	//	2, // v�����ϵĽ�  
//	//	&ctrlPoints[0][0][0]); //���Ƶ����� 
//	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
//		0, 1, 12, 4, &ctrlPoints[0][0][0]);//��������
//	glMap2f(GL_MAP2_TEXTURE_COORD_2, 
//		0, 
//		1, 
//		2, 
//		2,
//		0, 
//		1, 
//		4, 
//		2, 
//		&texpts[0][0][0]);//��������
//	//������ֵ��  
//	glEnable(GL_MAP2_TEXTURE_COORD_2);
//	glEnable(GL_MAP2_VERTEX_3);
//	glEnable(GL_AUTO_NORMAL);//�Զ����ɷ���
//
//
//
//	//��0��10ӳ��һ������10���������  
//	//glMapGrid2f(10, 0.0f, 10.0f, 10, 0.0f, 10.0f);
//	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
//	// ��������  
//
//	glEvalMesh2(GL_FILL, 0, 10, 0, 10);
//	
//	glDisable(GL_TEXTURE_2D);
//	//�����Ƶ�  
//	//glTexCoord2f(0.0, 0.0);glTexCoord2f(1.0, 0.0);glTexCoord2f(0.0, 1.0);glTexCoord2f(1.0, 1.0);
//
//	DrawPoints();
//
//	glPopMatrix();
//	//glutSwapBuffers();
//}
//
//#pragma endregion

#pragma region Demo for Texture
GLfloat ctrlpoints[4][4][3] =
{
{{ -1.5, -1.5, 4.0}, { -0.5, -1.5, 2.0},
{0.5, -1.5, -1.0}, {1.5, -1.5, 2.0}},
{{ -1.5, -0.5, 1.0}, { -0.5, -0.5, 3.0},
{0.5, -0.5, 0.0}, {1.5, -0.5, -1.0}},
{{ -1.5, 0.5, 4.0}, { -0.5, 0.5, 0.0},
{0.5, 0.5, 3.0}, {1.5, 0.5, 4.0}},
{{ -1.5, 1.5, -2.0}, { -0.5, 1.5, -2.0},
{0.5, 1.5, 0.0}, {1.5, 1.5, -1.0}}
};
//add up���ƶ��������

//add down ������
GLfloat texpts[2][2][2] =
{
{{0.0, 0.2}, {0.0, 0.5}},
{{1.0, 0.2}, {1.0, 0.5}}
};
#define imageWidth 64
#define imageHeight 64
GLubyte image[3 * imageWidth*imageHeight];
//��������
void makeImage(void)
{
	int i, j;
	float ti, tj;

	for (i = 0; i < imageWidth; i++) {
		ti = 2.0*3.14159265*i / imageWidth;
		for (j = 0; j < imageHeight; j++) {
			tj = 2.0*3.14159265*j / imageHeight;

			image[3 * (imageHeight*i + j)] = (GLubyte)127 * (1.0 + sin(ti));
			image[3 * (imageHeight*i + j) + 1] = (GLubyte)127 * (1.0 + cos(2 * tj));
			image[3 * (imageHeight*i + j) + 2] = (GLubyte)127 * (1.0 + cos(ti + tj));
		}
	}
}
void TextureDemoInit()
{
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
		0, 1, 12, 4, &ctrlpoints[0][0][0]);//��������
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2,
		0, 1, 4, 2, &texpts[0][0][0]);//��������
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_AUTO_NORMAL);//�Զ����ɷ���

	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat position[] = { 0.0, 0.0, 2.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };

	//glEnable(GL_LIGHTING);//���ú���ӹ���û���κ�Ч��
	//glEnable(GL_LIGHT0);

	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	//glLightfv(GL_LIGHT0, GL_POSITION, position);

	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	//makeImage();

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0,
	//	GL_RGB, GL_UNSIGNED_BYTE, image);
	
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	//glEvalMesh2(GL_FILL, 0, 1, 0, 1);
}

void drawTextureDemo() {
	//glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0, 1.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glFlush();
}
#pragma endregion

#pragma region SkyBox

void initSkybox() {
	
	//const char filename[128];
	const char *filepath = "./Textures/skybox01/";
	const char *bmpName[] = { "back","front","bottom","top","left","right" };
	for (int i = 0; i < 6; i++) {
		cout << "Loading skyBox , before: " << SkyboxTexture01[i] << endl;
		glGenTextures(1, &SkyboxTexture01[i]);
		glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[i]);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
		//�趨�Ŵ����С�Ĺ��˷�ʽ�����Թ���
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		string fp = filepath;
		string name = bmpName[i];
		string fn = fp + name + ".jpg";
		const char *filename = fn.data();

		int width, height, nrChannels;
		cout << "trying to read " << filename << endl;
		unsigned char *JPGdata = stbi_load( filename, &width, &height, &nrChannels, 0);
		if (JPGdata)
		{
			std::cout << "Load texture succeed" << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, JPGdata);
			//glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(JPGdata);

		cout << "after: " << SkyboxTexture01[i] << endl;
	}
	//loadTexture(filename);
	filepath = "./Textures/skybox02/";
	for (int i = 0; i < 6; i++) {
		cout << "Loading skyBox , before: " << SkyboxTexture02[i] << endl;
		glGenTextures(1, &SkyboxTexture02[i]);
		glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[i]);
		// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
		//�趨�Ŵ����С�Ĺ��˷�ʽ�����Թ���
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		string fp = filepath;
		string name = bmpName[i];
		string fn = fp + name + ".jpg";
		const char *filename = fn.data();

		int width, height, nrChannels;
		cout << "trying to read " << filename << endl;
		unsigned char *JPGdata = stbi_load(filename, &width, &height, &nrChannels, 0);
		if (JPGdata)
		{
			std::cout << "Load texture succeed" << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, JPGdata);
			//glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(JPGdata);

		cout << "after: " << SkyboxTexture02[i] << endl;
	}
	
}

void drawSkybox() {
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glColor3f(1.0, 1.0, 1.0);

	int x = 0, y = 0, z = 0;

	/** ��ó����й���״̬ */
	GLboolean lp;
	glGetBooleanv(GL_LIGHTING, &lp);

	GLfloat box_width = 1.0f;
	GLfloat box_height = 0.5f;
	GLfloat box_length = 1.0f;
	/** ������պг� �� �� */
	GLfloat width = MAP * box_width / 8;
	GLfloat height = MAP * box_height / 8;
	GLfloat length = MAP * box_length / 8;

	/** ������պ�����λ�� */
	x = r * cos(c*degreeY)*cos(c*degreeX);
	y = r * sin(c*degreeY);
	z = r * cos(c*degreeY)*sin(c*degreeX);

	glDisable(GL_LIGHTING);            /* �رչ��� */

	glPushMatrix();

	glTranslatef(-x, -y, -z);
	glEnable(GL_TEXTURE_2D);
	/** ���Ʊ��� */
	if(useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[0]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[0]);

	glBegin(GL_QUADS);

	/** ָ����������Ͷ������� */
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width / 2, y - height / 2, z - length / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width / 2, y + height / 2, z - length / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x - width / 2, y + height / 2, z - length / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x - width / 2, y - height / 2, z - length / 2);

	glEnd();

	/** ����ǰ�� */
	if (useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[1]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[1]);

	glBegin(GL_QUADS);

	/** ָ����������Ͷ������� */
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x - width / 2, y - height / 2, z + length / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x - width / 2, y + height / 2, z + length / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width / 2, y + height / 2, z + length / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width / 2, y - height / 2, z + length / 2);

	glEnd();

	/** ���Ƶ��� */
	if (useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[2]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[2]);

	glBegin(GL_QUADS);

	/** ָ����������Ͷ������� */
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x - width / 2, y - height / 2, z - length / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x - width / 2, y - height / 2, z + length / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width / 2, y - height / 2, z + length / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width / 2, y - height / 2, z - length / 2);

	glEnd();

	/** ���ƶ��� */
	if (useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[3]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[3]);

	glBegin(GL_QUADS);

	/** ָ����������Ͷ������� */
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width / 2, y + height / 2, z - length / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width / 2, y + height / 2, z + length / 2 );
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x - width / 2, y + height / 2, z + length / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x - width / 2, y + height / 2, z - length / 2);

	glEnd();

	/** �������� */
	if (useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[5]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[5]);

	glBegin(GL_QUADS);

	/** ָ����������Ͷ������� */
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x - width / 2, y + height / 2, z - length / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x - width / 2, y + height / 2, z + length / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x - width / 2, y - height / 2, z + length / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x - width / 2, y - height / 2, z - length / 2);

	glEnd();

	/** �������� */
	if (useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[4]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[4]);

	glBegin(GL_QUADS);

	/** ָ����������Ͷ������� */
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width / 2, y - height / 2, z - length / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width / 2, y - height / 2, z + length / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width / 2, y + height / 2, z + length / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width / 2, y + height / 2, z - length / 2);
	glEnd();

	glPopMatrix();                 /** ���ƽ��� */

	if (lp)                         /** �ָ�����״̬ */
		glEnable(GL_LIGHTING);

	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);

}

#pragma endregion

// �ļ�����ѡ���ļ�
bool openFile(string &filepath) {
	cout << "Start Opening" << endl;
	OPENFILENAME ofn;
	char szFile[300];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = (LPCSTR) "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		cout << (string)ofn.lpstrFile << endl;
		filepath = (string)ofn.lpstrFile;
		return true;
	}
	else
	{
		printf("user cancelled\n");
		return false;
	}
}

bool openPath(string &filepath) {
	TCHAR szBuffer[MAX_PATH] = { 0 };
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szBuffer;
	bi.lpszTitle = _T("������ѡ�ļ���Ŀ¼:");
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (NULL == idl)
	{
		return false;
	}
	SHGetPathFromIDList(idl, szBuffer);
	cout << (string)szBuffer << endl;
	filepath = (string)szBuffer;
	return true;
}

#pragma region Menu
//Model
#define OP_SANCAI_CUP 0
#define OP_TEA_CUP 1
#define OP_TABLE 2
#define OP_CHAIR 3
#define OP_TRAY 4
//Texture preset
#define OP_T_STONE_1 5
#define OP_T_STONE_2 6
#define OP_T_CHINA_1 7
#define OP_T_CHINA_2 8
#define OP_T_WOOD_1 9
#define OP_T_WOOD_2 10

#define OP_T_IMPORT 11

#define OP_P_ENABLE 12

#define OP_SKYBOX_1 13
#define OP_SKYBOX_2 14

#define OP_EXPORT 15
#define OP_IMPORT 16


float red, green, blue;
void subMenuEvents(int option) {
	//option�����Ǵ��ݹ�����value��ֵ��
	switch (option) {
	case OP_SANCAI_CUP: {
		MyObject obj(ModelType::SANCAI_CUP);
		//loadTexture("./Textures/02.jpg");
		//obj.addTexture(texture);
		myObjects.push_back(obj);
		break;
	}
	case OP_TEA_CUP: {
		MyObject obj(ModelType::TEA_CUP);
		//loadTexture("./Textures/02.jpg");
		//obj.addTexture(texture);
		myObjects.push_back(obj);
		break;
	}
	case OP_TABLE: {
		MyObject obj(ModelType::TABLE);
		//loadTexture("./Textures/02.jpg");
		//obj.addTexture(texture);
		myObjects.push_back(obj);
		break;
	}
	case OP_CHAIR: {
		MyObject obj(ModelType::CHAIR);
		//loadTexture("./Textures/02.jpg");
		//obj.addTexture(texture);
		myObjects.push_back(obj);
		break;
	}
	case OP_TRAY: {
		MyObject obj(ModelType::TRAY);
		//loadTexture("./Textures/02.jpg");
		//obj.addTexture(texture);
		myObjects.push_back(obj);
		break;
	}

	case OP_T_STONE_1: {
		loadTexture("./Textures/Stone01.jpg");
		myObjects[edit_object].addTexture(texture);
		break;
	}
	case OP_T_STONE_2: {
		loadTexture("./Textures/Stone02.jpg");
		myObjects[edit_object].addTexture(texture);
		break;
	}
	case OP_T_CHINA_1: {
		loadTexture("./Textures/China01.jpg");
		myObjects[edit_object].addTexture(texture);
		break;
	}
	case OP_T_CHINA_2: {
		loadTexture("./Textures/China02.jpg");
		myObjects[edit_object].addTexture(texture);
		break;
	}
	case OP_T_WOOD_1: {
		loadTexture("./Textures/Wood01.jpg");
		myObjects[edit_object].addTexture(texture);
		break;
	}
	case OP_T_WOOD_2: {
		loadTexture("./Textures/Wood02.jpg");
		myObjects[edit_object].addTexture(texture);
		break;
	}
	case OP_T_IMPORT: {
		string filepath;
		bool ret = openFile(filepath);
		if (ret) {
			loadTexture(filepath.c_str());
			myObjects[edit_object].addTexture(texture);
		}
		
		break;
	}
	case OP_P_ENABLE: {
		enablePaticle = (!enablePaticle);
		break;
	}
	case OP_SKYBOX_1: {
		useSkybox01 = true;
		break;
	}
	case OP_SKYBOX_2: {
		useSkybox01 = false;
		break;
	}
	default:
		break;
	}
}

void mainMenuEvents(int option) {
	switch (option) {
	case OP_EXPORT: {
		string filepath;
		bool ret = openPath(filepath);
		filepath += "\\obj.txt";
		if (ret) {
			myObjects[edit_object].exportObj(filepath);
		}
		break;
	}
	case OP_IMPORT: {
		string filepath;
		bool ret = openFile(filepath);
		if (ret) {
			/*myObjects[edit_object].importObj(filepath);*/
			MyObject Obj(filepath);
			myObjects.push_back(Obj);
		}
		break;
	}
	default:
		break;
	}
}

void createGLUTMenus() {

	int mainmenu;
	int submenu_preset;
	int submenu_texture;
	int submenu_texture_presetTexture;
	int submenu_paticle;
	int submenu_skybox;
	


	//����ģ��
	//�����˵�������GLUT��processMenuEvents����˵��¼���
	submenu_preset = glutCreateMenu(subMenuEvents);
		//���˵�������Ŀ
		glutAddMenuEntry("Sancai Cup             ", OP_SANCAI_CUP);
		glutAddMenuEntry("Tea Cup", OP_TEA_CUP);
		glutAddMenuEntry("Table", OP_TABLE);
		glutAddMenuEntry("Chair", OP_CHAIR);
		glutAddMenuEntry("Tray", OP_TRAY);

	//��������
	submenu_texture_presetTexture = glutCreateMenu(subMenuEvents);
		glutAddMenuEntry("Stone 01               ", OP_T_STONE_1);
		glutAddMenuEntry("Stone 02", OP_T_STONE_2);
		glutAddMenuEntry("China 01", OP_T_CHINA_1);
		glutAddMenuEntry("China 02", OP_T_CHINA_2);
		glutAddMenuEntry("Wood 01", OP_T_WOOD_1);
		glutAddMenuEntry("Wood 02", OP_T_WOOD_2);
	//submenu_texture_importTexture = glutCreateMenu(subMenuEvents);
		

	submenu_texture = glutCreateMenu(subMenuEvents);
		glutAddSubMenu("Use Preset Texture       ", submenu_texture_presetTexture);
		glutAddMenuEntry("Import from local JPG  ", OP_T_IMPORT);

	submenu_paticle = glutCreateMenu(subMenuEvents);
		glutAddMenuEntry("Enable/Disable paticle ", OP_P_ENABLE);

	submenu_skybox = glutCreateMenu(subMenuEvents);
		glutAddMenuEntry("Use Skybox 01          ", OP_SKYBOX_1);
		glutAddMenuEntry("Use Skybox 02          ", OP_SKYBOX_2);
	

	mainmenu = glutCreateMenu(mainMenuEvents);
		glutAddSubMenu("Add Preset Model        ", submenu_preset);
		glutAddMenuEntry("Export Model          ", OP_EXPORT);
		glutAddMenuEntry("Import Model          ", OP_IMPORT);
		glutAddSubMenu("Add Texture        ", submenu_texture);
		glutAddSubMenu("Paticle        ", submenu_paticle);
		glutAddSubMenu("Skybox        ", submenu_skybox);
	//�Ѳ˵�������Ҽ�����������
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

#pragma endregion

#pragma region Paticle
typedef struct//structrue for particle
{
	bool active;//active or not
	float life;//last time
	float fade;//describe the decreasing of life
	float r, g, b;//color
	float x, y, z;//the position
	float xi, yi, zi;//what direction to move
	float xg, yg, zg;//gravity
	float width;
	float xr, yr, zr;//rotate
}particles;

#define MAX_PARTICLES 200
particles paticle[MAX_PARTICLES];

bool rainbow = true;
bool sp, rp;//space button and return button pressed or not?
float slowdown = 2.0f;
float xspeed, yspeed;
//float zoom = -40.0f;
bool gkeys[256];

//���ɰ뾶
int range = 100;

GLuint loop, col, delay;

void initPaticle() {
	for (loop = 0; loop < MAX_PARTICLES; ++loop)
	{
		paticle[loop].active = true;
		paticle[loop].life = 1.0f;
		paticle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;

		paticle[loop].r = 1;
		paticle[loop].g = 0.4;
		paticle[loop].b = 0.2;

		paticle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f;
		paticle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f;
		paticle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f;

		paticle[loop].xg = -0.2f;
		paticle[loop].yg = -0.5f;
		paticle[loop].zg = 0.0f;

		paticle[loop].width = float(rand() % 100) / 200;

		paticle[loop].xr = float(rand() % 360);
		paticle[loop].yr = float(rand() % 360);
		paticle[loop].zr = float(rand() % 360);
	}
}

void drawPaticle() {

	glDisable(GL_TEXTURE_2D);
	
	for (loop = 0; loop < MAX_PARTICLES; ++loop)
	{
		glRotatef(paticle[loop].xr, 1, 0, 0);
		glRotatef(paticle[loop].yr, 0, 1, 0);
		glRotatef(paticle[loop].zr, 0, 0, 1);
		if (paticle[loop].active)//the particle is alive
		{
			float x = paticle[loop].x;
			float y = paticle[loop].y;
			//our scene is moved into the screen
			float z = paticle[loop].z ;

			glColor4f(paticle[loop].r,
				paticle[loop].g,
				paticle[loop].r,
				paticle[loop].life);

			float width = paticle[loop].width;

			//draw particles : use triangle strip instead of quad to speed
			glBegin(GL_TRIANGLE_STRIP);
			//top right
			glTexCoord2d(1, 1);
			glVertex3f(x + 0.5f, y + 0.5f, z);
			//top left
			glTexCoord2d(0, 1);
			glVertex3f(x - width/2, y + width/2, z);
			//bottom right
			glTexCoord2d(1, 0);
			glVertex3f(x + width/2, y - width/2, z);
			//bottom left
			glTexCoord2d(0, 0);
			glVertex3f(x - 0.5f, y - 0.5f, z);
			glEnd();

			//Move On The X Axis By X Speed 
			paticle[loop].x += paticle[loop].xi / (slowdown * 1000);
			//Move On The Y Axis By Y Speed 
			paticle[loop].y += paticle[loop].yi / (slowdown * 1000);
			//Move On The Z Axis By Z Speed 
			paticle[loop].z += paticle[loop].zi / (slowdown * 1000);

			//add gravity or resistance : acceleration
			paticle[loop].xi += paticle[loop].xg;
			paticle[loop].yi += paticle[loop].yg;
			paticle[loop].zi += paticle[loop].zg;

			//decrease particles' life
			paticle[loop].life -= paticle[loop].fade;

			//if particle is dead,rejuvenate it
			if (paticle[loop].life < 0.0f)
			{
				paticle[loop].life = 1.0f;//alive again
				paticle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;

				//reset its position
				//paticle[loop].x = 0.0f;
				paticle[loop].x = float((rand() % (2 * range)) - range);
				paticle[loop].y = 0;
				//paticle[loop].z = 0.0f;
				paticle[loop].x = float((rand() % (2 * range)) - range);


				//X Axis Speed And Direction 
				paticle[loop].xi = float((rand() % 600) - 300.0f);
				// Y Axis Speed And Direction 
				paticle[loop].yi = float((rand() % 600) - 600.0f);
				// Z Axis Speed And Direction 
				paticle[loop].zi = float((rand() % 600) - 300.0f);

				paticle[loop].width = float(rand() % 100) / 200;
			}
		}
	}
	glEnable(GL_TEXTURE_2D);
}

#pragma endregion

// ��װ���������������������ṩmode��selectʹ��
void drawScene(GLenum mode) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//���ó�ģ�;���ģʽ
	glMatrixMode(GL_MODELVIEW);

	//���뵥λ������
	glLoadIdentity();
	//����������Z��ƽ��-G_fDistance (ʹ��������λ�������ǰ��)
	glTranslatef(0.0, 0.0, -3.0);
	glInitNames();
	glPushName(0);

	glPushMatrix();
		gluLookAt(r*cos(c*degreeY)*cos(c*degreeX), r*sin(c*degreeY), r*cos(c*degreeY)*sin(c*degreeX),0.0f,0.0f,0.0f,  0.0f, 1.0f, 0.0f); //�������λ��
		if (mode != GL_SELECT) drawSkybox();	//��պв�����select����
		if (mode != GL_SELECT && enablePaticle) drawPaticle();
	glPopMatrix();
	for (int i = 0; i < myObjects.size(); i++) {
		//�����е�i��Ϊ���
		if (mode == GL_SELECT) glLoadName(i);
		glPushMatrix();
			gluLookAt(r*cos(c*degreeY)*cos(c*degreeX), r*sin(c*degreeY), r*cos(c*degreeY)*sin(c*degreeX), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f); //�������λ��
			
			if (mode != GL_SELECT) setLightRes();//�ڱ任�����б任��Դ��ʹ��Դ�����һͬת��

			myObjects[i].drawObject();
		
		glPopMatrix();
	}

	////����ǰ�󻺳���
	//glutSwapBuffers();

}

void loadTexture(const char* filepath) {
	cout << "tring to access " <<  filepath << endl;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//�趨�Ŵ����С�Ĺ��˷�ʽ�����Թ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// ���ز���������
	//JPGdata = stbi_load("./Textures/02.jpg", &width, &height, &nrChannels, 0);
	int width, height, nrChannels;
	unsigned char *JPGdata = stbi_load(filepath, &width, &height, &nrChannels, 0);
	if (JPGdata)
	{
		std::cout << "Load texture succeed" << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, JPGdata);
		//glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(JPGdata);

	//glActiveTexture(GL_TEXTURE0);
}



//��������
#pragma region SceneGuide
void ChangeSize(int w, int h) {
	if (h == 0) h = 1;
	ratio = 1.0f * w / h;
	glMatrixMode(GL_PROJECTION);	//ָ����ǰ����ΪͶӰ����������һ����ͶӰ����
	glLoadIdentity();	//����ǰ��������Ϊ��λ����
	glViewport(0, 0, w, h);	//�����ӿ�Ϊ�������ڴ�С
	//���ÿ��ӿռ�
	gluPerspective(45, ratio, 1, 1000); //���ӽǴ�С�����������������룬��Զ����
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);
}
void orientMe(float ang) {
	cout << "angle: " << ang << endl;
	lx = sin(ang);
	lz = -cos(ang);
	glLoadIdentity();
	gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);
}
void moveMeFlat(int direction) {
	x = x + direction * (lx)*0.1;
	z = z + direction * (lz)*0.1;
	glLoadIdentity();
	gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);
}
void InputKey(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_LEFT:
			//cout << "Get KeyLeft" << endl;
			angle -= 0.01f * spanSpeed;
			orientMe(angle); break;
		case GLUT_KEY_RIGHT:
			angle += 0.01f * spanSpeed;
			orientMe(angle); break;
		case GLUT_KEY_UP:
			moveMeFlat(1); break;
		case GLUT_KEY_DOWN:
			moveMeFlat(-1); break;
	}
}
#pragma endregion

int main(int argc, char* argv[])
{
	//openFile();
	glutInit(&argc, argv);
	init();
	initPaticle();
	
	//glutSpecialFunc(InputKey);	//�����¼�

	//��Ⱦ����
	glutDisplayFunc(display);

	glutReshapeFunc(reshape);
	//����¼�ע��
	glutMouseFunc(moseClick);

	//�����¼�ע��
	glutMouseWheelFunc(mouseWheel);
	glutMotionFunc(changeViewPoint);
	//�����¼�ע��
	glutKeyboardFunc(keyBoardInput);

	glutIdleFunc(myIdle);

	//���ò˵�
	createGLUTMenus();

	glutMainLoop();
	return 0;
}