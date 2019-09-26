#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <windows.h>

class Item
{
public:
	float m_fPosition[3];
	float m_fVelocity[3];
	int m_hp;
	int m_index;

	Item() {}
	~Item() {}

	void SetPosition(float x, float y, float z);
	void SetVelocity(float vx, float vy, float vz);
	void SetIndex(int index);
	void SetHP();
	void Move(float fElapsedTime);

	void DrawItem(GLuint& tex, float time);
};