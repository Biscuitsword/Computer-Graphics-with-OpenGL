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

//窗口参数
GLfloat window_height = 600.0f;
GLfloat window_width = 600.0f;
//UI栏参数
GLfloat ui_height = 100.0f;
GLfloat ui_width = 600.0f;

//模型路径
string filePath = "./Objects/Cube.obj";

//实现移动鼠标观察模型所需变量
static float c = 3.1415926 / 180.0f;
static float r = 40.0f;
static int degreeX = 90;
static int degreeY = 0;
static int oldPosY = -1;
static int oldPosX = -1;
//相机位置
static int cameraX = 0;
static int cameraY = 10;
static int cameraZ = 40;

//场景漫游
static float angle = 0.0;//绕y轴旋转角
static float spanSpeed = 1.0f;	//旋转速率
static float ratio;//窗口比例
static float x = 0.0, y = 1.75f, z = 5.0f;//相机位置
static float lx = 0.0f, ly = 0.0f, lz = -1.0f;//相机面向（向量）

//2D编辑模式场景参数
static float r_2D = 20.0f;

//切换编辑模式和观赏模式
enum SceneMode
{
	EDIT,
	VIEW
};
SceneMode sceneMode = VIEW;

//参考坐标轴绘制
GLfloat axisPoints[3][2][3] = {
	{{0.0f, 0.0f, 0.0f},
	{100.0f, 0.0f, 0.0f}},

	{{0.0f, 0.0f, 0.0f},
	{0.0f, 100.0f, 0.0f}},

	{{0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 100.0f}} 
};
//坐标轴宽度
GLfloat axisSize = 2;

//贝塞尔控制点 
GLint numOfPoints = 4; 
static GLfloat controlPoints[4][3] = 
{{0.0f, 10.0f, 0.0f},
{8.0f, 10.0f, 0.0f},
{ 6.0f, 4.0f, 0.0f },
{ 4.0f, 0.0f, 0.0f }};
//贝塞尔纹理控制点
static GLfloat texturePoints[2][2][2] =
{
{{0.0, 0.0}, {0.0, 1.0}},
{{0.5, 0.0}, {0.5, 1.0}}
};
//物体坐标
static GLfloat position[3] =
{
	10.0, 0.0, 0.0
};

vector<MyObject> myObjects;
int edit_object = 0;

//正在被移动的点
int editPoint = -1;
bool editingPoint = false;
//贴图纹理
unsigned int texture;
bool useSkybox01 = true;
unsigned int SkyboxTexture01[6];
unsigned int SkyboxTexture02[6];
unsigned int leafTexture;
unsigned int flowerTexture;

//粒子系统开启/关闭
bool enablePaticle = true;

//光源开启/关闭
bool enableLight = true;
//光源参数
static float light_r = 100.0f;
static int light_degreeX = 90;
static int light_degreeY = 0;
static float light_red = 0.0f;
static float light_green = 0.0f;
static float light_blue = 0.0f;
GLfloat sun_light_position[] = { light_r*cos(c*light_degreeY)*cos(c*light_degreeX), light_r*sin(c*light_degreeY), light_r*cos(c*light_degreeY)*sin(c*light_degreeX), 1.0f }; //光源的位置
GLfloat sun_light_ambient[] = { light_red, light_green, light_blue, 1.0f }; //RGBA模式的环境光，为0
GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //RGBA模式的漫反射光，全白光
GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  //RGBA模式下的镜面光 ，全白光


void drawScene(GLenum mode);
//加载贴图
void loadTexture(const char* filepath);
//纹理Demo
void TextureDemoInit(void);
void drawTextureDemo(void);
//天空盒
void initSkybox(void);
void drawSkybox(void);

/* 定义地面网格 */
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

//安置光源
void setLightRes() {
	//sun_light_position[] = { light_r*cos(c*light_degreeY)*cos(c*light_degreeX), light_r*sin(c*light_degreeY), light_r*cos(c*light_degreeY)*sin(c*light_degreeX), 1.0f }; //光源的位置
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

	glEnable(GL_LIGHTING); //启用光源
	glEnable(GL_LIGHT0);   //使用指定灯光
	glEnable(GL_COLOR_MATERIAL);	//使颜色直接作用在材质上

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); //正反面计算

}

//配置材质
void SetMaterial() {
	//材质
	GLfloat mat_ambient[] = { 0.2,0.2,0.2,1.0 };
	GLfloat mat_diffuse[] = { 0.8,0.3,0.3,1.0 };
	GLfloat mat_specular[] = { 1.0,0.5,0.5,1.0 };
	GLfloat mat_shiness[] = { 50.0 };
	/*定义物体正面材质*/
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shiness);
}

//坐标换算
//根据屏幕坐标得到视点空间坐标
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


//初始化
void init() {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("ObjLoader");
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	//调用配置光源
	setLightRes();
	glEnable(GL_DEPTH_TEST);
	//图形能把光反射到各个方向（使得曲面能够接收光照）
	glEnable(GL_AUTO_NORMAL);

	//MyObject myObj(ModelType::TEA_CUP);
	//loadTexture("./Textures/02.jpg");
	//myObj.addTexture(texture);

	//myObjects.push_back(myObj);
	//myObj.moveObject(10, 0, 0);
	//myObjects.push_back(myObj);

	//TextureDemoInit();
	initSkybox();
	//ui初始化
	//initUI();

}

//绘制坐标轴
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

		glClearColor(1.0, 1.0, 1.0, 1.0);  //改变默认背景有颜色为浅黄色，要写在glLoadIdentity()前
		glColor3f(1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
			gluLookAt(0, 0, r_2D, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f); //更新相机位置
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
	//修改渲染距离
	gluPerspective(60.0f, (GLdouble)width / (GLdouble)height, 1.0f, 10000.0f);
	glMatrixMode(GL_MODELVIEW);
}

//处理键盘事件
void keyBoardInput(unsigned char key, int x, int y) {

	switch (key)
	{
	case 9:
		//Tab事件
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
	//光源颜色调整
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
	//调试用按键
	case 'p': {
		MAGIC += 0.01;
		cout << MAGIC << endl;
		break;
	}
	default:
		break;
	}
}

//拾取矩阵处理
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
		//切换正在编辑物品
		edit_object = target;

		printf(" Stack IDs are: \n");
		for (k = 0; k < objID; k++) {
			printf(" %d ", *ptr);
			ptr++;
		}
		printf("\n\n");
	}
}

//鼠标点击事件
void moseClick(int button, int state, int x, int y)
{
	switch (sceneMode)
	{
	case EDIT:
		if (state == GLUT_DOWN) {
			cout << "Click in EDIT mode" << endl;
			float px, py, pz;
			//转换坐标系
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
			//转换坐标系
			Get3Dpos(x, y, px, py, pz);
			cout << "End move at " << px << ", "<< py << endl;
			//自动吸附
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
//滚轮缩放
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
			//调整光源
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
			//调整视角
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
		//转换坐标系
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
////绘制曲面
////控制点  
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
//};// 纹理控制点
//
//void DrawPoints(void){ 
//	int i, j;
//	glColor3f(1.0f, 0.0f, 0.0f); //把点放大一点，看得更清楚  
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
//	// 保存模型视图矩阵  
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//
//	glColor3f(1.0f, 1.0f, 1.0f); //设置映射方式，只需要设置一次可以在SetupRC中调用。  
//	//glMap2f(GL_MAP2_VERTEX_3, //生成的数据类型  
//	//	0.0f, // u的下界 
//	//	10.0f, //u的上界  
//	//	3, //数据中点的间隔  
//	//	4, //u方向上的阶  
//	//	0.0f, //v的下界  
//	//	10.0f, //v的上界  
//	//	12, // 控制点之间的间隔  
//	//	2, // v方向上的阶  
//	//	&ctrlPoints[0][0][0]); //控制点数组 
//	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
//		0, 1, 12, 4, &ctrlPoints[0][0][0]);//制作曲面
//	glMap2f(GL_MAP2_TEXTURE_COORD_2, 
//		0, 
//		1, 
//		2, 
//		2,
//		0, 
//		1, 
//		4, 
//		2, 
//		&texpts[0][0][0]);//纹理曲面
//	//启用求值器  
//	glEnable(GL_MAP2_TEXTURE_COORD_2);
//	glEnable(GL_MAP2_VERTEX_3);
//	glEnable(GL_AUTO_NORMAL);//自动生成法线
//
//
//
//	//从0到10映射一个包含10个点的网格  
//	//glMapGrid2f(10, 0.0f, 10.0f, 10, 0.0f, 10.0f);
//	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
//	// 计算网格  
//
//	glEvalMesh2(GL_FILL, 0, 10, 0, 10);
//	
//	glDisable(GL_TEXTURE_2D);
//	//画控制点  
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
//add up控制顶点的坐标

//add down 纹理常量
GLfloat texpts[2][2][2] =
{
{{0.0, 0.2}, {0.0, 0.5}},
{{1.0, 0.2}, {1.0, 0.5}}
};
#define imageWidth 64
#define imageHeight 64
GLubyte image[3 * imageWidth*imageHeight];
//制作纹理
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
		0, 1, 12, 4, &ctrlpoints[0][0][0]);//制作曲面
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2,
		0, 1, 4, 2, &texpts[0][0][0]);//纹理曲面
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_AUTO_NORMAL);//自动生成法线

	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat position[] = { 0.0, 0.0, 2.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };

	//glEnable(GL_LIGHTING);//禁用和添加光照没有任何效果
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
		// 为当前绑定的纹理对象设置环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
		//设定放大和缩小的过滤方式：线性过滤
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
		// 为当前绑定的纹理对象设置环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
		//设定放大和缩小的过滤方式：线性过滤
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

	/** 获得场景中光照状态 */
	GLboolean lp;
	glGetBooleanv(GL_LIGHTING, &lp);

	GLfloat box_width = 1.0f;
	GLfloat box_height = 0.5f;
	GLfloat box_length = 1.0f;
	/** 计算天空盒长 宽 高 */
	GLfloat width = MAP * box_width / 8;
	GLfloat height = MAP * box_height / 8;
	GLfloat length = MAP * box_length / 8;

	/** 计算天空盒中心位置 */
	x = r * cos(c*degreeY)*cos(c*degreeX);
	y = r * sin(c*degreeY);
	z = r * cos(c*degreeY)*sin(c*degreeX);

	glDisable(GL_LIGHTING);            /* 关闭光照 */

	glPushMatrix();

	glTranslatef(-x, -y, -z);
	glEnable(GL_TEXTURE_2D);
	/** 绘制背面 */
	if(useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[0]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[0]);

	glBegin(GL_QUADS);

	/** 指定纹理坐标和顶点坐标 */
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width / 2, y - height / 2, z - length / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width / 2, y + height / 2, z - length / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x - width / 2, y + height / 2, z - length / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x - width / 2, y - height / 2, z - length / 2);

	glEnd();

	/** 绘制前面 */
	if (useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[1]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[1]);

	glBegin(GL_QUADS);

	/** 指定纹理坐标和顶点坐标 */
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x - width / 2, y - height / 2, z + length / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x - width / 2, y + height / 2, z + length / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width / 2, y + height / 2, z + length / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width / 2, y - height / 2, z + length / 2);

	glEnd();

	/** 绘制底面 */
	if (useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[2]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[2]);

	glBegin(GL_QUADS);

	/** 指定纹理坐标和顶点坐标 */
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x - width / 2, y - height / 2, z - length / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x - width / 2, y - height / 2, z + length / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width / 2, y - height / 2, z + length / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width / 2, y - height / 2, z - length / 2);

	glEnd();

	/** 绘制顶面 */
	if (useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[3]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[3]);

	glBegin(GL_QUADS);

	/** 指定纹理坐标和顶点坐标 */
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width / 2, y + height / 2, z - length / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width / 2, y + height / 2, z + length / 2 );
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x - width / 2, y + height / 2, z + length / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x - width / 2, y + height / 2, z - length / 2);

	glEnd();

	/** 绘制左面 */
	if (useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[5]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[5]);

	glBegin(GL_QUADS);

	/** 指定纹理坐标和顶点坐标 */
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x - width / 2, y + height / 2, z - length / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x - width / 2, y + height / 2, z + length / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x - width / 2, y - height / 2, z + length / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x - width / 2, y - height / 2, z - length / 2);

	glEnd();

	/** 绘制右面 */
	if (useSkybox01) glBindTexture(GL_TEXTURE_2D, SkyboxTexture01[4]);
	else  glBindTexture(GL_TEXTURE_2D, SkyboxTexture02[4]);

	glBegin(GL_QUADS);

	/** 指定纹理坐标和顶点坐标 */
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width / 2, y - height / 2, z - length / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width / 2, y - height / 2, z + length / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width / 2, y + height / 2, z + length / 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width / 2, y + height / 2, z - length / 2);
	glEnd();

	glPopMatrix();                 /** 绘制结束 */

	if (lp)                         /** 恢复光照状态 */
		glEnable(GL_LIGHTING);

	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);

}

#pragma endregion

// 文件窗口选择文件
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
	bi.lpszTitle = _T("从下面选文件夹目录:");
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
	//option，就是传递过来的value的值。
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
	


	//创建模型
	//创建菜单并告诉GLUT，processMenuEvents处理菜单事件。
	submenu_preset = glutCreateMenu(subMenuEvents);
		//给菜单增加条目
		glutAddMenuEntry("Sancai Cup             ", OP_SANCAI_CUP);
		glutAddMenuEntry("Tea Cup", OP_TEA_CUP);
		glutAddMenuEntry("Table", OP_TABLE);
		glutAddMenuEntry("Chair", OP_CHAIR);
		glutAddMenuEntry("Tray", OP_TRAY);

	//创建纹理
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
	//把菜单和鼠标右键关联起来。
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

//生成半径
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

// 封装函数：绘制整个场景，提供mode供select使用
void drawScene(GLenum mode) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//设置成模型矩阵模式
	glMatrixMode(GL_MODELVIEW);

	//载入单位化矩阵
	glLoadIdentity();
	//坐标中心向Z轴平移-G_fDistance (使坐标中心位于摄像机前方)
	glTranslatef(0.0, 0.0, -3.0);
	glInitNames();
	glPushName(0);

	glPushMatrix();
		gluLookAt(r*cos(c*degreeY)*cos(c*degreeX), r*sin(c*degreeY), r*cos(c*degreeY)*sin(c*degreeX),0.0f,0.0f,0.0f,  0.0f, 1.0f, 0.0f); //更新相机位置
		if (mode != GL_SELECT) drawSkybox();	//天空盒不参与select计算
		if (mode != GL_SELECT && enablePaticle) drawPaticle();
	glPopMatrix();
	for (int i = 0; i < myObjects.size(); i++) {
		//命名中的i即为序号
		if (mode == GL_SELECT) glLoadName(i);
		glPushMatrix();
			gluLookAt(r*cos(c*degreeY)*cos(c*degreeX), r*sin(c*degreeY), r*cos(c*degreeY)*sin(c*degreeX), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f); //更新相机位置
			
			if (mode != GL_SELECT) setLightRes();//在变换过程中变换光源，使光源和相机一同转动

			myObjects[i].drawObject();
		
		glPopMatrix();
	}

	////交换前后缓冲区
	//glutSwapBuffers();

}

void loadTexture(const char* filepath) {
	cout << "tring to access " <<  filepath << endl;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//设定放大和缩小的过滤方式：线性过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 加载并生成纹理
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



//场景漫游
#pragma region SceneGuide
void ChangeSize(int w, int h) {
	if (h == 0) h = 1;
	ratio = 1.0f * w / h;
	glMatrixMode(GL_PROJECTION);	//指定当前矩阵为投影矩阵，声明下一步是投影操作
	glLoadIdentity();	//将当前矩阵设置为单位矩阵
	glViewport(0, 0, w, h);	//设置视口为整个窗口大小
	//设置可视空间
	gluPerspective(45, ratio, 1, 1000); //可视角大小，物体比例，最近距离，最远距离
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
	
	//glutSpecialFunc(InputKey);	//键盘事件

	//渲染函数
	glutDisplayFunc(display);

	glutReshapeFunc(reshape);
	//点击事件注册
	glutMouseFunc(moseClick);

	//滚轮事件注册
	glutMouseWheelFunc(mouseWheel);
	glutMotionFunc(changeViewPoint);
	//键盘事件注册
	glutKeyboardFunc(keyBoardInput);

	glutIdleFunc(myIdle);

	//启用菜单
	createGLUTMenus();

	glutMainLoop();
	return 0;
}