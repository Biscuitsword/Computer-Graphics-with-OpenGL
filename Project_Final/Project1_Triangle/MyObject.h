#pragma once
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "stb_image.h"
using namespace std;

static GLfloat MAGIC = 0.66;//����Բ ħ��

//���ߺ���  ��yΪ��ת������
void doRotate(GLfloat points[][3], int degree, GLfloat ret[][3]) {
	float c = 3.1415926 / 180.0f;
	for (int i = 0; i < 4; i++) {
		//cout << "Rotating" << endl;
		//cout << points[i][0] <<endl;
		GLfloat nX = cos(c * degree) * points[i][0] + sin(c * degree) * points[i][2];
		GLfloat nY = points[i][1];
		GLfloat nZ = -sin(c * degree) * points[i][0] + cos(c * degree) * points[i][2];
		ret[i][0] = nX;
		ret[i][1] = nY;
		ret[i][2] = nZ;
		//cout << "new Point: " << 
	}
}

//���ߺ���	����ƽ���������
float getDistance(float x1, float y1, float x2, float y2) {

	float dis = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
	return sqrt(dis);
}

class BerFace {
public:
	BerFace(GLint numP, GLfloat controlP[4][3], int R = 4) {
		Rings = R;
		numOfPoints = numP;
		memcpy(controlPoints, controlP, sizeof(controlP));
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 3; j++) {
				controlPoints[i][j] = controlP[i][j];
				//cout << controlPoints[i][j] << " ";
			}
			//cout << endl;
		}
		cout << "Init BerFace End" << endl;
	}
	~BerFace() {

	}

	//���Ƶ����
	GLint numOfPoints;
	//���������Ƶ� 
	GLfloat controlPoints[4][3];
	//������������Ƶ�
	GLfloat texturePoints[2][2][2]{
		{{0.0, 0.0}, {0.0, 1.0}},
		{{0.5, 0.0}, {0.5, 1.0}}
	};
	//ģ�;�ϸ��
	int Rings = 4;


	//����������ά����
	//RingsΪԲ�ܱ���
	//Texture���ⲿ��ǰ����
	void drawFace(unsigned int tex, bool isSquare = false) {
		//ƽ��Ϊ���
		//double sideDelta = 360 / Sides;
		double ringDelta = 360 / Rings;

		//��ǰת���Ƕ�
		double theta = 0;
		double cosTheta = 1.0;
		double sinTheta = 0.0;

		double phi, sinPhi, cosPhi;
		double dist;

		glColor3f(1.00f, 0.0f, 0.0f);

		if (isSquare) MAGIC = 0;

		//for (int i = 0; i < Rings; i++)
		for (int i = 0; i < Rings; i++)
		{
			//�������ƴ�Բ��ÿһ��
			double theta1 = theta + ringDelta;
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, tex);
			//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
			drawBerPiece(theta, theta1);
			glDisable(GL_TEXTURE_2D);
			theta = theta1;

			//break;
		}

		if (isSquare) MAGIC = 0.66;
	}

	//���Ʊ༭�����ߣ�������������
	void drawLine() {
		glDisable(GL_LIGHTING);
		glColor3f(0.0, 0.0, 0.0);
		//���ñ��������ߣ����������ʵֻ��Ҫ����һ�Σ����Է���SetupRC������
		glMap1f(GL_MAP1_VERTEX_3, //���ɵ���������
			0.0f, //uֵ���½�
			100.0f, //uֵ���Ͻ�
			3, //�����������еļ����x,y,z���Լ����3
			numOfPoints, //u�����ϵĽף������Ƶ�ĸ���
			&controlPoints[0][0] //ָ����Ƶ����ݵ�ָ�� 
		);
		//�����ڻ��ƶ���֮ǰ����
		glEnable(GL_MAP1_VERTEX_3);
		//ʹ�û��ߵķ�ʽ�����ӵ�
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= 100; i++)
		{
			glEvalCoord1f((GLfloat)i);

		}
		glEnd();

		drawPoints();
	}

	//����������ҿ��Ƶ㣬 ���ص����
	int getPoint(float px, float py) {
		cout << "Looking for point in face" << endl;
		int p = -1;
		for (int i = 0; i < numOfPoints; i++) {
			if (getDistance(controlPoints[i][0], controlPoints[i][1], px, py) < 1.0f) {
				cout << "Get in Face: Clicking point " << i << endl;
				p = i;
				break;
			}
		}
		cout << "Face return the index: " << p << endl;
		return p;
	}

	//��ָ�����Ƶ��ƶ���Ŀ��λ��
	void editPoint(int index, float tx, float ty) {
		if (index >= numOfPoints) {
			cout << "WTF?" << endl;
			return;
		}
		controlPoints[index][0] = tx;
		controlPoints[index][1] = ty;
	}
private:
	//���Ʊ༭�ÿ��Ƶ�
	void drawPoints() {
		glPointSize(5.0f);
		glBegin(GL_POINTS); for (int i = 0; i < numOfPoints; ++i)
		{
			glVertex3fv(controlPoints[i]);
		}
		glEnd();
	}

	//���������е�һƬ
	void drawBerPiece(float from, float to) {
		//cout << "draw from " << from << " to " << to << endl;
		//GLfloat MAGIC = 0.5522847498;
		
		

		GLfloat fromPoints[4][3];
		GLfloat fromPoints_1[4][3];	//��ħ�����ɵ��м���Ƶ�
		GLfloat toPoints_1[4][3];//��ħ�����ɵ��м���Ƶ�
		GLfloat toPoints[4][3];
		//GLfloat objPoints[2][4][3];
		GLfloat objPoints[4][4][3];
		GLfloat texPoints[2][2][2];

		doRotate(controlPoints, from, fromPoints);
		doRotate(controlPoints, to, toPoints);
		

		memcpy(fromPoints_1, fromPoints, sizeof(fromPoints));
		memcpy(toPoints_1, toPoints, sizeof(toPoints));

		if (from == 0) {
			for (int i = 0; i < 4; i++) {
				fromPoints_1[i][2] -= abs(MAGIC * fromPoints_1[i][0]);
				toPoints_1[i][0] += abs(MAGIC * fromPoints_1[i][2]);
			}
		}
		if (from == 90) {
			for (int i = 0; i < 4; i++) {
				fromPoints_1[i][0] -= abs(MAGIC * fromPoints_1[i][2]);
				toPoints_1[i][2] -= abs(MAGIC * fromPoints_1[i][0]);
			}
		}
		if (from == 180) {
			for (int i = 0; i < 4; i++) {
				fromPoints_1[i][2] += abs(MAGIC * fromPoints_1[i][0]);
				toPoints_1[i][0] -= abs(MAGIC * fromPoints_1[i][2]);
			}
		}
		if (from == 270) {
			for (int i = 0; i < 4; i++) {
				fromPoints_1[i][0] += abs(MAGIC * fromPoints_1[i][2]);
				toPoints_1[i][2] += abs(MAGIC * fromPoints_1[i][0]);
			}
		}
		
		memcpy(objPoints[0], fromPoints, sizeof(fromPoints));
		memcpy(objPoints[1], fromPoints_1, sizeof(fromPoints_1));
		memcpy(objPoints[2], toPoints_1, sizeof(toPoints_1));
		memcpy(objPoints[3], toPoints, sizeof(toPoints));

		GLfloat texfrom = from / 360;
		GLfloat texto = to / 360;
		//cout << "texfrom :" << texfrom << " , texto: " << texto << endl;
		memcpy(texPoints, texturePoints, sizeof(texturePoints));
		texPoints[0][0][0] = texfrom;
		texPoints[1][0][0] = texto;
		texPoints[0][1][0] = texfrom;
		texPoints[1][1][0] = texto;

		glMatrixMode(GL_MODELVIEW);
		//glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);
		//����ӳ�䷽ʽ��ֻ��Ҫ����һ�ο�����SetupRC�е��á�  
		glMap2f(GL_MAP2_VERTEX_3, //���ɵ���������  
			0.0f, // u���½� 
			1.0f, //u���Ͻ�  
			3, //�����е�ļ��  
			4, //u�����ϵĽ�  
			0.0f, //v���½�  
			1.0f, //v���Ͻ�  
			12, // ���Ƶ�֮��ļ��  
			4, // v�����ϵĽ�  
			&objPoints[0][0][0]); //���Ƶ����� 
		glMap2f(GL_MAP2_TEXTURE_COORD_2, 0.0f, 1.0f, 2, 2,
			0.0f, 1.0f, 4, 2, &texPoints[0][0][0]);//��������
		//������ֵ��  
		glEnable(GL_MAP2_VERTEX_3);
		glEnable(GL_MAP2_TEXTURE_COORD_2);
		glEnable(GL_AUTO_NORMAL);//�Զ����ɷ���

		//��0��10ӳ��һ������10���������  
		glMapGrid2f(20, 0.0f, 1.0f, 20, 0.0f, 1.0f);

		glEnable(GL_TEXTURE_2D);
		// ��������ʹ��FILL�ܹ��Զ����÷��ߣ�
		glEvalMesh2(GL_FILL, 0, 20, 0, 20);
		//glEnable(GL_MAP2_TEXTURE_COORD_2);
		//glEvalMesh2(GL_LINE, 0, 10, 0, 10);
		//glPopMatrix();
		//glDisable(GL_TEXTURE_2D);
	}
};


enum ModelType
{
	SANCAI_CUP,
	TEA_CUP,
	TABLE,
	CHAIR,
	TRAY
};

class MyObject {
public:
	MyObject(ModelType modelT) {
		cout << "in MyObject" << endl;
		modelType = modelT;

		switch (modelType)
		{
		case SANCAI_CUP: {
			cout << "Add a sancai cup" << endl;
			numOfFaces = 3;
			GLfloat Points_1[4][3] = {
				{9.0f, 9.6f, 0.0f},
				{6.69f, 8.98f, 0.0f},
				{ 7.84f, -1.1f, 0.0f },
				{ 0.0f, 0.0f, 0.0f } };
			GLfloat Points_2[4][3] = {
				{7.95f, 9.33f, 0.0f},
				{6.81f, 10.9f, 0.0f},
				{ 3.72f, 11.9f, 0.0f },
				{ 1.14f, 11.85f, 0.0f } };
			GLfloat Points_3[4][3] = {
				{1.14f, 11.85f, 0.0f},
				{0.63f, 12.59f, 0.0f},
				{ 3.03f, 14.19f, 0.0f },
				{ 0.0f, 13.62f, 0.0f } };
			BerFace B1(4, Points_1);
			BerFace B2(4, Points_2);
			BerFace B3(4, Points_3);
			Faces.push_back(B1);
			Faces.push_back(B2);
			Faces.push_back(B3);

			cout << "My Object Init Finish" << endl;
			break;
		}
		case TEA_CUP: {
			cout << "Add a tea cup" <<endl;
			numOfFaces = 2;
			GLfloat Points_1[4][3] ={ 
				{0.0f, 0.0f, 0.0f},
				{4.96f, -0.26f, 0.0f},
				{ 5.38f, 1.0f, 0.0f },
				{ 5.38f, 2.69f, 0.0f } };
			GLfloat Points_2[4][3] = {
				{5.38f, 2.69f, 0.0f },
				{4.96f, -0.46f, 0.0f},
				{ 1.11f, 0.73f, 0.0f },
				{ 0.0f, 0.46f, 0.0f } };
			BerFace B1(4, Points_1);
			BerFace B2(4, Points_2);
			Faces.push_back(B1);
			Faces.push_back(B2);

			cout << "My Object Init Finish" << endl;

			break;
		}
		case TABLE: {
			cout << "Add a Table" << endl;
			numOfFaces = 3;
			GLfloat Points_1[4][3] = {
				{0.0f, 15.0f, 0.0f},
				{4.0f, 15.0f, 0.0f},
				{ 8.0f, 15.0f, 0.0f },
				{ 12.0f, 15.0f, 0.0f } };
			GLfloat Points_2[4][3] = {
				{12.0f, 15.0f, 0.0f},
				{12.5f, 8.0f, 0.0f},
				{ 3.0f, 15.5f, 0.0f },
				{ 3.0f, 10.0f, 0.0f } };
			GLfloat Points_3[4][3] = {
				{3.0f, 10.0f, 0.0f},
				{6.0f, 0.5f, 0.0f},
				{ 1.0f, 5.0f, 0.0f },
				{ 4.0f, 0.0f, 0.0f } };
			BerFace B1(4, Points_1);
			BerFace B2(4, Points_2);
			BerFace B3(4, Points_3);
			Faces.push_back(B1);
			Faces.push_back(B2);
			Faces.push_back(B3);
			cout << "My Object Init Finish" << endl;
			break;
		}
		case CHAIR: {
			cout << "Add a Chair" << endl;
			numOfFaces = 2;
			GLfloat Points_1[4][3] = {
				{0.0f, 8.0f, 0.0f},
				{5.8f, 8.04f, 0.0f},
				{ 3.3f, 8.23f, 0.0f },
				{ 4.2f, 5.4f, 0.0f } };
			GLfloat Points_2[4][3] = {
				{4.2f, 5.4f, 0.0f},
				{5.4f, -1.1f, 0.0f},
				{ 5.85f, 0.3f, 0.0f },
				{ 0.0f, 0.0f, 0.0f } };
			BerFace B1(4, Points_1);
			BerFace B2(4, Points_2);
			Faces.push_back(B1);
			Faces.push_back(B2);

			cout << "My Object Init Finish" << endl;
			break;
		}
		case TRAY:{
			cout << "Add a Chair" << endl;
			numOfFaces = 3;
			GLfloat Points_1[4][3] = {
				{0.0f, 0.9f, 0.0f},
				{4.34f, 1.15f, 0.0f},
				{ 3.61f, 0.62f, 0.0f },
				{ 3.96f, 1.54f, 0.0f } };
			GLfloat Points_2[4][3] = {
				{3.96f, 1.54f, 0.0f},
				{5.15f, 1.42f, 0.0f},
				{ 4.54f, 2.38f, 0.0f },
				{ 4.69f, 0.0f, 0.0f } };
			GLfloat Points_3[4][3] = {
				{4.69f, 0.0f, 0.0f},
				{3.5f, 0.0f, 0.0f},
				{ 2.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f } };
			BerFace B1(4, Points_1, 4);
			BerFace B2(4, Points_2, 4);
			BerFace B3(4, Points_3, 4);
			Faces.push_back(B1);
			Faces.push_back(B2);
			Faces.push_back(B3);

			cout << "My Object Init Finish" << endl;
			break;
		}
		default:
			break;
		}
	}
	MyObject(string filepath) {
		importObj(filepath);
	}
	~MyObject() {

	}

	ModelType modelType;

	//��������
	GLint numOfFaces;
	//ÿ����
	vector<BerFace> Faces;
	//��Ӧ��������
	vector <unsigned int> textures;
	//��������
	GLfloat position[3] = { 0.0, 0.0, 0.0 };

	GLfloat scale = 1.0f;

	//��������
	void drawObject() {
		glTranslatef(position[0], position[1], position[2]);
		glScalef(scale, scale, scale);

		bool isSquare = (modelType == ModelType::TRAY);

		for (int i = 0; i < numOfFaces; i++) {
			if(i < textures.size())
				Faces[i].drawFace(textures[i],isSquare);
			else
				Faces[i].drawFace(0, isSquare);
		}
	}

	//���Ʊ༭��ͼ,����������
	void drawLines() {
		for (int i = 0; i < numOfFaces; i++) {
			if (i >= Faces.size()) break;
			Faces[i].drawLine();
		}
		//���Ʊ�����
		glColor3f(0.9, 0.9, 0.9);

		glDisable(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, SkyboxTexture[0]);
		glBegin(GL_QUAD_STRIP);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-100.0f, -100.0f, 0.0f);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(100.0f, -100.0f, 0.0f);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-100.0f, 100.0f, 0.0f);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(100.0f, 100.0f, 0.0f);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
	}


	//�ƶ�����
	void moveObject(GLfloat dx, GLfloat dy, GLfloat dz) {
		position[0] += dx;
		position[1] += dy;
		position[2] += dz;
	}

	//��������
	void addTexture(unsigned int tex) {
		cout << "Add Texture to OBJ�� " << tex << endl;
		textures.push_back(tex);
	}

	//���ұ��༭��
	bool getPoint(float px, float py) {
		cout << "Looking for point at " << px << ", " << py << endl;
		bool find = false;
		for (int i = 0; i < numOfFaces; i++) {
			int ret = Faces[i].getPoint(px, py);
			if (ret != -1) {
				cout << "OBJ get answer: " << ret << endl;
				edit_f = i;
				edit_p = ret;
				find = true;
				break;
			}
		}
		cout << "find it ? " << find << endl;
		return find;
	}

	//�༭����
	void editLine(float tx, float ty) {
		if (edit_f != -1 && edit_p != -1) {
			Faces[edit_f].editPoint(edit_p, tx, ty);
		}
	}
	
	//�����༭
	void editEnd() {
		edit_f = -1;
		edit_p = -1;
	}

	void exportObj(string filepath) {
		cout << "try export " << filepath << endl;
		ofstream outfile(filepath, ios::trunc | ios::out);
		if (!outfile.is_open())
		{
			cout << "can not open this file" << endl;
			return;
		}

		outfile << modelType <<  ",\n";
		outfile << numOfFaces << ",\n";
		outfile << position[0] << "," << position[1] << "," << position[2]<< ",\n";

		////���Ƶ����
		//GLint numOfPoints;
		////���������Ƶ� 
		//GLfloat controlPoints[4][3];
		////������������Ƶ�
		//GLfloat texturePoints[2][2][2]{
		//	{{0.0, 0.0}, {0.0, 1.0}},
		//	{{0.5, 0.0}, {0.5, 1.0}}
		//};
		////ģ�;�ϸ��
		//int Rings = 90;

		for (int i = 0; i < numOfFaces; i++) {
			outfile << Faces[i].Rings << ",\n";
			outfile << Faces[i].numOfPoints << ",\n";
			for (int j = 0; j < Faces[i].numOfPoints; j++) {
				outfile << Faces[i].controlPoints[j][0] << "," << Faces[i].controlPoints[j][1] << "," << Faces[i].controlPoints[j][2] << ",\n";
			}
		}



		outfile.close();
		cout << "export finish" << endl;
	}

	void importObj(string filepath) {
		cout << "try import " << filepath << endl;
		ifstream infile(filepath, ios::in);
		if (!infile.is_open())
		{
			cout << "can not open this file" << endl;
			return;
		}
		int count = 0;
		string line;
		while (count < 3)
		{
			getline(infile, line);
			stringstream ss(line);

			int pi = 0;
			
			while (getline(ss, line, ','))
			{
				//cout << line << ", ";
				if (count == 0) modelType = (ModelType)atoi(line.c_str());
				if (count == 1) numOfFaces = atoi(line.c_str());
				if (count == 2) {
					position[pi] = atof(line.c_str());
					pi++;
				}
				else {
					pi = 0;
				}
			}

			count++;
		}
		cout << "step1: " << modelType << ", " << numOfFaces << endl << position[0] << ", " << position[1] << ", " << position[2] << endl;


		//����ÿһ����
		count = 0;
		int f_Rings, f_NumOfPoints;
		GLfloat f_Points[4][3];
		for (int i = 0; i < numOfFaces; i++) {
			while (count < 2 ) {
				getline(infile, line);
				stringstream ss(line);
				getline(ss, line, ',');
				cout << "reading line: " << line << endl;
				if (count == 0) f_Rings = atoi(line.c_str()); 
				if (count == 1) f_NumOfPoints = atoi(line.c_str());
				count++;
			}
			//�������
			for (int j = 0; j < f_NumOfPoints; j++) {
				getline(infile, line);
				stringstream ss(line);
				for (int k = 0; k < 3; k++) {
					
					getline(ss, line, ',');
					f_Points[j][k] = atof(line.c_str());
				}
				
			}
			//������
			BerFace B(f_NumOfPoints,f_Points,f_Rings);
			Faces.push_back(B);

			cout << "step2: " << B.Rings << ", " << B.numOfPoints << endl;
			for (int a = 0; a < 4; a++) {
				for (int b = 0; b < 3; b++) {
					cout << B.controlPoints[a][b] << ", ";
				}cout << endl;
			}

			count = 0;
		}

		//for (int i = 0; i < 3; i++) {
		//	cout << position[i] << " ";
		//}
		cout << "import finish " << endl;
	}

private:
	int edit_f = -1; //���ڱ��༭�������
	int edit_p = -1; //���ڱ��༭�ĵ����
	
	
};