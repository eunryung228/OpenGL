#include "Item.h"
#include "Bmp.h"
#include "LodePng.h"

void Item::SetPosition(float x, float y, float z)
{
	m_fPosition[0] = x;
	m_fPosition[1] = y;
	m_fPosition[2] = z;
}

void Item::SetVelocity(float vx, float vy, float vz)
{
	m_fVelocity[0] = vx;
	m_fVelocity[1] = vy;
	m_fVelocity[2] = vz;
}

void Item::SetIndex(int index)
{
	m_index = index;
}

void Item::SetHP()
{
	switch (m_index)
	{
	case 1:
		m_hp = -50;
		break;
	case 2:
		m_hp = -30;
		break;
	case 3:
		m_hp = -20;
		break;
	case 4:
		m_hp = 40;
		break;
	case 5:
		m_hp = 25;
		break;
	case 6:
		m_hp = 15;
		break;
	default:
		break;
	}
}

void Item::Move(float fElapsedTime)
{
	m_fPosition[0] += fElapsedTime * m_fVelocity[0];
	m_fPosition[1] += fElapsedTime * m_fVelocity[1];
	m_fPosition[2] += fElapsedTime * m_fVelocity[2];
}

void Item::DrawItem(GLuint& tex, float time)
{
	glLoadIdentity();
	GLfloat pos[4][3] = { { -0.2f, 0.2f, 0.2f }, { 0.2f, 0.2f, 0.2f }, { 0.2f, 0.2f, -0.2f }, { -0.2f, 0.2f, -0.2f } };
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(m_fPosition[0], m_fPosition[1], m_fPosition[2]);
	glRotatef(120, 1, 1, 1);
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(pos[1]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(pos[2]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(pos[3]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(pos[0]);

	glEnd();

	Move(time);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}