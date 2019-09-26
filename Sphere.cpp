
#pragma warning(disable: 4996)

#include "Game.h"
#include "Sphere.h"
#include "Bmp.h"
#include "LodePng.h"


CSphere::CSphere() {
	m_fPosition[0] = 0.0f;
	m_fPosition[1] = 0.0f;
	m_fPosition[2] = 0.0f;

	m_fColor[0] = 0.0f;
	m_fColor[1] = 0.0f;
	m_fColor[2] = 0.0f;

	m_fVelocity[0] = 0.0f;
	m_fVelocity[1] = 0.0f;
	m_fVelocity[2] = 0.0f;

	m_nSlice = m_nStack = 20;

	m_Quad = gluNewQuadric();
}

CSphere::CSphere(float *fPosition, float *fColor, float *fVelocity)
{
	int i;
	for(i = 0 ; i < 3 ; i++)
		m_fPosition[i] = fPosition[i];

	for(i = 0 ; i < 3 ; i++)
		m_fColor[i] = fColor[i];

	for(i = 0 ; i < 3 ; i++)
		m_fVelocity[i] = fVelocity[i];

	m_nSlice = m_nStack = 20;

	m_Quad = gluNewQuadric();
}

CSphere::~CSphere()
{
	gluDeleteQuadric(m_Quad);
}

void CSphere::SetPosition(float fX, float fY, float fZ)
{
	m_fPosition[0] = fX;
	m_fPosition[1] = fY;
	m_fPosition[2] = fZ;
}

void CSphere::SetColor(float fR, float fG, float fB)
{
	m_fColor[0] = fR;
	m_fColor[1] = fG;
	m_fColor[2] = fB;
}

void CSphere::SetVelocity(float fVx, float fVy, float fVz)
{
	m_fVelocity[0] = fVx;
	m_fVelocity[1] = fVy;
	m_fVelocity[2] = fVz;
}

void CSphere::SetRadius(double radius)
{
	m_dRadius = radius;
}

void CSphere::Move(float fElapsedTime)
{
	m_fPosition[0] += fElapsedTime*m_fVelocity[0];
	m_fPosition[1] += fElapsedTime*m_fVelocity[1];
	m_fPosition[2] += fElapsedTime*m_fVelocity[2];
}

void CSphere::Render(float rot_angle)
{
	glColor3f(m_fColor[0], m_fColor[1], m_fColor[2]);
	glTranslatef(m_fPosition[0], m_fPosition[1], m_fPosition[2]);
	glPushMatrix();
	
	float ambient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float diffuse[4] = { 1.0f, 0.6f, 0.6f, 1.0f };
	float specular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float emission[4] = { 0.15f, 0.0f, 0.0f, 0.0f };
	float shininess = 128.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	
	unsigned char *Image;
	int nW, nH;
	Image = ReadBmp("ryan.bmp", &nW, &nH);

	glGenTextures(1, m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, nW, nH, GL_BGR, GL_UNSIGNED_BYTE, Image);
	delete[] Image;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_Texture[0]);
	gluQuadricTexture(m_Quad, GL_TRUE);

	glPopMatrix();

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glTranslatef(0.5, 0.5, 0.5);
	glRotatef(rot_angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.5, -0.5, -0.5);
	gluSphere(m_Quad, m_dRadius, m_nSlice, m_nStack);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glLoadIdentity();

	float ambient_r[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float diffuse_r[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float specular_r[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emission_r[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_r);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_r);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_r);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission_r);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
}
