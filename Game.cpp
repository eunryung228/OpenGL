//	Game Class Using OpenGL & Glut (Implementation Main), ver. 0.6
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr

#pragma warning(disable: 4996)

#include "Game.h"
#include "Bmp.h"
#include "LodePng.h"
#include <ctime>

CGame::CGame(int nW, int nH, int nPosX, int nPosY)
{
	m_nW = nW;
	m_nH = nH;

	m_nPosX = nPosX;
	m_nPosY = nPosY;

	m_fNowHP = 0.5f;
	m_nObjectHP = 0;

	m_bPause = false;

	m_bCreated = false;

	if (nInstanceCnt == 0) pInstance = this;

	nInstanceCnt++;
}

CGame::~CGame()
{
	if (m_bCreated)
		ShutdownApp();

	nInstanceCnt--;
}

void CGame::Create(int arg, char **argc, float *fBgColor, double(*dLookAt)[3], bool bVSync)
{
	glutInitWindowSize(m_nW, m_nH);
	glutInitWindowPosition(m_nPosX, m_nPosY);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInit(&arg, argc);
	glutCreateWindow("LeeLion A+ Project");

	glewInit();

	m_nFrame = 0;
	m_nBaseTime = glutGet(GLUT_ELAPSED_TIME);
	m_nPreviousTime = m_nBaseTime;

	glutDisplayFunc(RenderSceneStatic);
	glutReshapeFunc(ResizeStatic);
	glutKeyboardFunc(KeyDownStatic);
	glutSpecialFunc(SpecialInputStatic);
	glutMouseFunc(MouseStatic);

	atexit(ShutdownAppStatic);

	m_bVSync = bVSync;
	SetVSync(m_bVSync);

	m_dDistace = 12.0;
	m_dAngleZ = 0.0;
	m_fRotationAngle = 0.0;

	int i;
	int r, c;
	for (i = 0; i < 4; i++)
		m_fBgColor[i] = fBgColor[i];

	for (r = 0; r < 3; r++)
		for (c = 0; c < 3; c++)
			m_dLookAt[r][c] = dLookAt[r][c];

	m_dFps = 0.;

	m_Hud.SetPosition(260, 250);
	m_Hud.SetColor(1.0f, 0.0f, 0.0f);


	m_bFullscreen = false;

	if (InitializeApp() == true)
	{
		m_bCreated = true;
		glutMainLoop();
	}
}

void CGame::DrawObject()
{
	glLoadIdentity();
	m_Sphere.Render(m_fRotationAngle);

	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();
	glTranslatef(0.0f, -3.0f, -7.0f);
	glScalef(8.0f, 0.5f, 120.0f);
	DrawCube();
	glPopMatrix();
}

void CGame::RenderItems()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_isItem[i] == true)
		{
			m_Items[i].DrawItem(m_Texture[m_Items[i].m_index], 12);
			if (m_Items[i].m_fPosition[1] < -1.2)
				m_isItem[i] = false;
		}
	}
}

void CGame::MakeItems(int iNum)
{
	if (m_isItem[iNum] == false)
	{
		int randTex = rand() % 6 + 1; // 1~6까지
		m_Items[iNum].SetIndex(randTex);
		m_Items[iNum].SetHP();
		m_Items[iNum].SetPosition(0, 0, -2);
		m_Items[iNum].SetVelocity(rand() / (float)RAND_MAX*0.002 - 0.001, -0.001, 0);
		m_isItem[iNum] = true;
	}
}

void CGame::CheckCollide()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_isItem[i] == true)
		{
			if (m_Items[i].m_fVelocity[0] < 0)
			{
				if (pow(m_Sphere.m_fPosition[0] - m_Items[i].m_fPosition[0], 2) + pow(m_Sphere.m_fPosition[1] - m_Items[i].m_fPosition[1], 2) <= 2)
				{
					printf("충돌");
					m_nObjectHP += m_Items[i].m_hp;
					if (m_Items[i].m_index >= 1 && m_Items[i].m_index <= 3)
						PlayWave(&m_Wave[2]);
					else
						PlayWave(&m_Wave[1]);
					m_isItem[i] = false;
				}
			}
			else
			{
				if (pow(m_Sphere.m_fPosition[0] - m_Items[i].m_fPosition[0], 2) + pow(m_Sphere.m_fPosition[1] - m_Items[i].m_fPosition[1], 2) <= pow(1.15f, 2))
				{
					printf("충돌");
					m_nObjectHP += m_Items[i].m_hp;
					if (m_Items[i].m_index >= 1 && m_Items[i].m_index <= 3)
						PlayWave(&m_Wave[2]);
					else
						PlayWave(&m_Wave[1]);
					m_isItem[i] = false;
				}
			}
		}
	}
}

void CGame::DrawHP()
{
	float hpChange = m_nObjectHP / 10 * 0.02f;
	m_nObjectHP = 0;
	m_fNowHP += hpChange;

	if (m_fNowHP >= 0.5f)
		m_fNowHP = 0.5f;

	glLoadIdentity();
	glTranslatef(0, 2.2f, -4.3f);
	glScalef(6.1f, 1.0f, 1.0f);
	glPushMatrix();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(-0.5f, 0.15f);
	glVertex2f(-0.5f, -0.15f);
	glVertex2f(m_fNowHP, -0.15f);
	glVertex2f(m_fNowHP, 0.15f);
	glEnd();

	glBegin(GL_LINE_LOOP); // HP 주변 라인
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.5f, 0.15f);
	glVertex2f(-0.5f, -0.15f);
	glVertex2f(0.5f, -0.15f);
	glVertex2f(0.5f, 0.15f);
	glEnd();

	glPopMatrix();
}

void CGame::RenderScene()
{
	m_nCurrentTime = glutGet(GLUT_ELAPSED_TIME);
	int nElapsedTime = m_nCurrentTime - m_nPreviousTime;

	m_nPreviousTime = m_nCurrentTime;
	m_nElapsedTime += nElapsedTime;
	m_nItemMakeTime += nElapsedTime;

	m_fNowHP -= 0.0000001*m_nElapsedTime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	gluLookAt(0, 10, 8, 0, 0, -5, 0, 1, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, m_LightViewMatrix);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_LightProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_LightViewMatrix);

	glViewport(0, 0, m_nShadowMapSize, m_nShadowMapSize);

	glCullFace(GL_FRONT);

	glColorMask(0, 0, 0, 0); // 빛의 위치에서 보는 걸 안 보여주기 위해 0, 0, 0, 0으로 display를 하지 않게 설정

	glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_nShadowMapSize, m_nShadowMapSize);

	glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);

	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glViewport(0, 0, m_nWindowW, m_nWindowH);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_POSITION, m_LightPosition);

	glLightfv(GL_LIGHT0, GL_AMBIENT, Gray);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Gray);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Black);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	DrawObject();
	if (m_nItemMakeTime >= 0 && m_nItemMakeTime <= 3500)
	{
		MakeItems(0);
	}
	else if (m_nItemMakeTime > 3500 && m_nItemMakeTime <= 7000)
	{
		MakeItems(1);
	}
	else
	{
		if (m_nItemMakeTime > 10500)
			m_nItemMakeTime = 0;
		MakeItems(2);
	}
	RenderItems();
	DrawHP();
	
	CheckCollide();
	

	glLightfv(GL_LIGHT0, GL_DIFFUSE, White);
	glLightfv(GL_LIGHT0, GL_SPECULAR, White);

	GLfloat BiasMatrix[16] = { 0.5f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.5f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.5f, 0.0f,
								0.5f, 0.5f, 0.5f, 1.0f };	//bias from [-1, 1] to [0, 1]

	GLfloat Temp[16];
	GLfloat TextureMatrix[16];// = BiasMatrix*LightProjectionMatrix*LightViewMatrix;

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, &TextureMatrix[0]);
	glEnable(GL_TEXTURE_GEN_S);

	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, &TextureMatrix[4]);
	glEnable(GL_TEXTURE_GEN_T);

	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, &TextureMatrix[8]);
	glEnable(GL_TEXTURE_GEN_R);

	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, &TextureMatrix[12]);
	glEnable(GL_TEXTURE_GEN_Q);

	glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

	glAlphaFunc(GL_GEQUAL, 0.99f);
	glEnable(GL_ALPHA_TEST);

	RenderItems();
	m_fRotationAngle += 0.2f;
	DrawObject(); // 색상 반영, depth를 따져서 그림자가 없는 부분은 밝게 Display
	DrawHP();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);

	if (m_fNowHP <= -0.5f)
	{
		PlayWave(&m_Wave[3]);
		m_fNowHP = -0.5f;
		char GameoverText[100];
		sprintf(GameoverText, "GameOver!");
		m_Hud.SetText(GameoverText);
		m_Hud.DisplayHud(this);
	}
	else
	{
		glutPostRedisplay();
	}
	glutSwapBuffers();
}

void CGame::KeyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;

	case 'v':
		m_bVSync = !m_bVSync;
		SetVSync(m_bVSync);
		break;

	case 'p':
		m_bPause = !m_bPause;
		break;
	}
}

void CGame::SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		break;
	case GLUT_KEY_DOWN:
		break;

	case GLUT_KEY_RIGHT:
		if (m_Sphere.m_fPosition[0] < 2.0)
		{
			m_Sphere.SetVelocity(0.02, 0.0f, 0.0f);
			m_Sphere.Move(30);
		}
		break;

	case GLUT_KEY_LEFT:
		if (m_Sphere.m_fPosition[0] > -2.0)
		{
			m_Sphere.SetVelocity(-0.02f, 0.0f, 0.0f);
			m_Sphere.Move(30);
		}
		break;

	case GLUT_KEY_F1:
		m_Sphere.SetPosition(0.0f, 0.6f, 4.5f);
		m_nDirectionZ = -1;
		m_nElapsedTime = 0;
		break;

	case GLUT_KEY_F11:
		if (!m_bFullscreen)
		{
			glutFullScreen();
			m_bFullscreen = true;
		}
		else if (m_bFullscreen)
		{
			glutReshapeWindow(m_nW, m_nH);
			glutPositionWindow(m_nPosX, m_nPosY);
			m_bFullscreen = false;
		}
		break;
	}
}

void CGame::Resize(int width, int height)
{
	m_nWindowW = width;
	m_nWindowH = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)width / (double)height, 0.1, 50.0);

	glGetFloatv(GL_PROJECTION_MATRIX, m_LightProjectionMatrix);
}

void CGame::Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{

	}
}

bool CGame::InitializeApp()
{
	if (!GLEW_ARB_depth_texture || !GLEW_ARB_shadow)	return false;

	glClearColor(m_fBgColor[0], m_fBgColor[1], m_fBgColor[2], m_fBgColor[3]);
	glEnable(GL_DEPTH_TEST);

	m_LightPosition[0] = -2;
	m_LightPosition[1] = 2;
	m_LightPosition[2] = 0.0f;
	m_LightPosition[3] = 0.0f;

	m_nElapsedTime = 0;
	m_nItemMakeTime = 0;

	m_nDirectionZ = -1;
	m_Sphere.SetPosition(0.0f, -1.9, -4.3f);
	m_Sphere.SetColor(1.0f, 1.0f, 1.0f);
	m_Sphere.SetRadius(0.5);

	m_velocity = 1.0f;
	for (int i = 0; i < 3; i++)
	{
		m_isItem[i] = false;
	}

	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	glEnable(GL_NORMALIZE);

	m_nShadowMapSize = 512;

	glGenTextures(1, &m_ShadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_nShadowMapSize, m_nShadowMapSize, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	unsigned char *Image;
	int nW, nH;
	Image = ReadBmp("ryan.bmp", &nW, &nH);
	if (!Image) return false;

	glGenTextures(7, m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, nW, nH, GL_BGR, GL_UNSIGNED_BYTE, Image);
	delete[] Image;


	unsigned int res;
	res = lodepng_decode32_file(&Image, (unsigned*)&nW, (unsigned*)&nH, "aboksung.png");
	if (res == 1) return false;

	glBindTexture(GL_TEXTURE_2D, m_Texture[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, nW, nH, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	delete[] Image;


	res = lodepng_decode32_file(&Image, (unsigned*)&nW, (unsigned*)&nH, "phone.png");
	if (res == 1) return false;

	glBindTexture(GL_TEXTURE_2D, m_Texture[2]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, nW, nH, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	delete[] Image;


	res = lodepng_decode32_file(&Image, (unsigned*)&nW, (unsigned*)&nH, "game.png");
	if (res == 1) return false;

	glBindTexture(GL_TEXTURE_2D, m_Texture[3]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, nW, nH, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	delete[] Image;


	res = lodepng_decode32_file(&Image, (unsigned*)&nW, (unsigned*)&nH, "jokbo.png");
	if (res == 1) return false;

	glBindTexture(GL_TEXTURE_2D, m_Texture[4]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, nW, nH, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	delete[] Image;

	res = lodepng_decode32_file(&Image, (unsigned*)&nW, (unsigned*)&nH, "notebook.png");
	if (res == 1) return false;

	glBindTexture(GL_TEXTURE_2D, m_Texture[5]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, nW, nH, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	delete[] Image;

	res = lodepng_decode32_file(&Image, (unsigned*)&nW, (unsigned*)&nH, "ice_coffee.png");
	if (res == 1) return false;

	glBindTexture(GL_TEXTURE_2D, m_Texture[6]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, nW, nH, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	delete[] Image;


	printf("%d\n", LoadMp3("Fantasyland.mp3", &m_Wave[0])); // 배경음
	printf("%d\n", LoadMp3("getgood.mp3", &m_Wave[1]));
	printf("%d\n", LoadWave("getbad.wav", &m_Wave[2]));
	printf("%d\n", LoadWave("gameover.wav", &m_Wave[3]));

	PlayWave(&m_Wave[0]);

	return true;
}

void CGame::ShutdownApp()
{
}