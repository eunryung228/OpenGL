//	Load Obj Files (Implementation)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr

#include "Game.h"
#include "LinkedList.h"

#pragma warning(disable: 4996)

bool LoadObj(char *FileName, GLfloat **Vertex, int *pVertexCnt, unsigned int **TriangleIndex, int *pTraingleCnt)
{
	CIpList<GLfloat> VertexList;
	CIpList<int> TriangleList;

	FILE *fp;
	int read;
	GLfloat x, y, z;
	char ch;

	fp = fopen(FileName, "r");
	if(!fp) return false;

	while (!(feof(fp)))
	{
		read = fscanf(fp, "%c %f %f %f", &ch, &x, &y, &z);
		if (read == 4 && ch == 'v')
		{
			VertexList.InsertTail(x);
			VertexList.InsertTail(y);
			VertexList.InsertTail(z);
		}
		else if (read == 4 && ch == 'f')
		{
			TriangleList.InsertTail(x);
			TriangleList.InsertTail(y);
			TriangleList.InsertTail(z);
		}
	}

	fclose(fp);

	int i;

	*pVertexCnt = VertexList.GetSize();
	*Vertex = new GLfloat[*pVertexCnt];

	i = 0;
	CNode4IpList<GLfloat>* pHeadV = VertexList.GetHead();
	while(pHeadV)
		(*Vertex)[i++] = VertexList.GetDataForward(&pHeadV);


	*pTraingleCnt = TriangleList.GetSize();
	*TriangleIndex = new unsigned int[*pTraingleCnt];

	i = 0;
	CNode4IpList<int>* pHeadT = TriangleList.GetHead();
	while(pHeadT)
		(*TriangleIndex)[i++] = TriangleList.GetDataForward(&pHeadT) - 1;

	return true;
}