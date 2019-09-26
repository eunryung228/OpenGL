//	Game Class Using OpenGL & Glut (Header), ver. 0.6
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr

#pragma once

#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <windows.h>

#include "Hud.h"
#include "Sphere.h"
#include "Item.h"
#include "Wave.h"

class CGame
{
public:
	static int nInstanceCnt;
	static CGame *pInstance;

	static GLfloat Red[4], Green[4], Blue[4], White[4], Black[4], Gray[4];

	GLfloat m_LightPosition[4];

	GLfloat m_LightViewMatrix[16], m_LightProjectionMatrix[16];
	GLuint m_ShadowMapTexture;
	int m_nWindowW, m_nWindowH;
	int m_nShadowMapSize;

	CWave m_Wave[10];

	Item m_Items[3]; // 한 번에 최대 4개까지 생성이 가능하도록
	bool m_isItem[3];
	void RenderItems();
	void MakeItems(int iNum);
	float m_velocity;

	void CheckCollide();
	void DrawObject();
	void DrawHP();
	float m_fNowHP;
	int m_nObjectHP;

	int m_nElapsedTime;
	int m_nItemMakeTime;
	int m_nDirectionZ;

	float m_dDistace;
	float m_dAngleZ;
	float m_fRotationAngle;

	int m_nW, m_nH;
	int m_nPosX, m_nPosY;
	bool m_bCreated;

	GLuint m_Texture[7];

	float m_fBgColor[4];
	double m_dLookAt[3][3];
	
	int m_nFrame;
	int m_nCurrentTime, m_nBaseTime, m_nPreviousTime;
	bool m_bVSync, m_bFullscreen;
	double m_dFps;

	bool m_bPause;

	CHud m_Hud;
	CSphere m_Sphere;

	CGame(int nW, int nH, int nPosX, int nPosY);
	~CGame();

	static void SetVSync(bool bSync);
	static void DrawBitmapText(char *string, GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, int nW, int nH, bool bFixed2D);

	static void DrawCube();
	static void DrawCoordinates();
	
	static void RenderSceneStatic();
	static void KeyDownStatic(unsigned char key, int x, int y);
	static void SpecialInputStatic(int key, int x, int y);
	static void ResizeStatic(int width, int height);
	static void MouseStatic(int button, int state, int x, int y);
	static void ShutdownAppStatic();

	void Create(int arg, char **argc, float *fBgColor, double (*dLookAt)[3], bool bVSync);

	void RenderScene();
	void KeyDown(unsigned char key, int x, int y);
	void SpecialInput(int key, int x, int y);
	void Resize(int width, int height);
	void Mouse(int button, int state, int x, int y);

	bool InitializeApp();
	void ShutdownApp();
};