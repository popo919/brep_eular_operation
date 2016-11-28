#ifndef _brep_h_
#define _brep_h_
#include <vector>
#include "vectors.h"
#include <gl\glew.h>
#include <glfw3.h>
#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Edge;
class Face;
class Solid;
class Loop;
class HalfEdge;

class Vertex
{
public :
	Vec3f cordinate;
	HalfEdge *hf;

public :
	int id;

	//CONSTRUCTORS & DESTRUCTORS
	Vertex(){}

	Vertex(Vec3f v)
	{
		this->cordinate = v;
	}

	//GETTERS
	Vec3f getCordinate()
	{
		return this->cordinate;
	}

	//OPERATIONS
	void setHalfEdge(HalfEdge *e)
	{
		this->hf = e;
	}
};

class HalfEdge
{
private :
	Vertex *fromVertex, *toVertex;
	HalfEdge *next, *prev, *partner;
	Edge * e;
	Loop *loop;
public :
	int id;

	//CONSTRUCTORS & DESTRUCTORS
	HalfEdge(){}

	HalfEdge(Vertex *f, Vertex *t)
	{
		this->fromVertex = f;
		this->toVertex = t;
	}

	~HalfEdge()
	{
		delete fromVertex;
		delete toVertex;
	}

	//SETTERS
	void setEdge(Edge *e)
	{
		this->e = e;
	}

	void setStartV(Vertex *v)
	{
		this->fromVertex = v;
	}

	void setEndV(Vertex *v)
	{
		this->toVertex = v;
	}

	void setNextHF(HalfEdge *hf)
	{
		this->next = hf;
	}

	void setPrevHF(HalfEdge *hf)
	{
		this->prev = hf;
	}

	void setLoop(Loop *l)
	{
		this->loop = l;
	}

	//GETTERS
	HalfEdge *getNext()
	{
		return this->next;
	}

	HalfEdge *getPrev()
	{
		return this->prev;
	}

	HalfEdge *&getPartner()
	{
		return this->partner;
	}

	Vertex *getStartV()
	{
		return this->fromVertex;
	}

	Vertex *getEndV()
	{
		return this->toVertex;
	}

	Vec3f getVector()
	{
		return this->toVertex->cordinate - this->fromVertex->cordinate;
	}

	Loop *getLoop()
	{
		return this->loop;
	}

	//HELPER FUNCTIONS
	void printEdge()
	{
		printf("from %.2f %.2f %.2f\n", this->fromVertex->cordinate[0], 
			this->fromVertex->cordinate[1], this->fromVertex->cordinate[2]);

		printf("to %.2f %.2f %.2f\n", this->toVertex->cordinate[0], 
			this->toVertex->cordinate[1], this->toVertex->cordinate[2]);
	}
};

class Edge
{
private :
	HalfEdge *h1, *h2;

public :
	int id;

	//CONSTRUCTORS & DESTRUCTORS
	Edge(HalfEdge *h1, HalfEdge *h2)
	{
		this->h1 = h1;
		this->h2 = h2;
		this->h1->setEdge(this);
		this->h2->setEdge(this);
	}

	~Edge()
	{
		delete h1;
		delete h2;
	}

	//GETTERS
	HalfEdge *getHe1()
	{
		return this->h1;
	}

	HalfEdge *getHe2()
	{
		return this->h2;
	}
};

class Loop
{
private :
	Loop *next;
	HalfEdge *firstEdge;
	Face *face;
	Vertex *firstVertex;
	int start_idx;
	int vertex_num;
	bool is_inner;

public :
	//CONSTRUCTORS & DESTRUCTORS

	Loop(Face *f)
	{
		this->face = f;
		next = NULL;
		firstEdge = NULL;
		firstVertex = NULL;
		is_inner = false;
	}

	//SETTER
	void setFirstVertex(Vertex *v)
	{
		this->firstVertex = v;
	}

	void setFirstEdge(HalfEdge *hf)
	{
		this->firstEdge = hf;
	}

	void setStartIdx(int s)
	{
		this->start_idx = s;
	}

	void setVertexNum(int n)
	{
		this->vertex_num = n;
	}

	void setFace(Face *f)
	{
		this->face = f;
	}

	//GETTERS

	bool &isInner()
	{
		return this->is_inner;
	}

	int computeVertexNum()
	{
		int num = 0;

		if(NULL != this->firstEdge)
		{
			HalfEdge *hf = firstEdge;
			num++;
			hf = hf->getNext();

			while(hf != this->firstEdge)
			{
				num++;
				hf = hf->getNext();
			}
		}
		
		this->vertex_num = num;

		return num;
	}

	int getStartIdx()
	{
		return this->start_idx;
	}

	int getVertexNum()
	{
		return this->vertex_num;
	}
		
	HalfEdge *getFirstEdge()
	{
		return this->firstEdge;
	}

	Loop *&getNext()
	{
		return this->next;
	}

	Face *getFace()
	{
		return this->face;
	}

	Vertex *getFirstVertex()
	{
		return this->firstVertex;
	}

	//HELPER FUNCTIONS

	void drawLoop()
	{
		glDrawArrays(GL_POLYGON, this->start_idx, this->vertex_num);
	}

	void printStartIdx()
	{
		printf("%d\n", this->start_idx);
	}

	void printLoop()
	{
		if(NULL != this->firstEdge)
		{
			HalfEdge *hf = firstEdge;
			Vertex *v = hf->getStartV();
			printf("%.2f %.2f %.2f\n", v->cordinate[0], v->cordinate[1], v->cordinate[2]);

			hf = hf->getNext();
			while(hf != this->firstEdge)
			{
				v = hf->getStartV();
				printf("%.2f %.2f %.2f\n", v->cordinate[0], v->cordinate[1], v->cordinate[2]);
				hf = hf->getNext();
			}
			printf("\n");
		}
	}
};

class Face
{
private :
	Loop *outerLoop;
	Solid *solid;
	Face *next;
	bool innerLoopExist;

public :
	//CONSTRUCTORS & DESTRUCTORS
	Face(Solid *s)
	{
		this->solid = s;
		this->outerLoop = NULL;
		this->next = NULL;
		this->innerLoopExist = false;
	}

	//SETTERS
	void setOuterLoop(Loop *l)
	{
		this->outerLoop = l;
		l->getNext() = NULL;
		l->isInner() = false;
	}

	void setNext(Face *f)
	{
		this->next = f;
	}

	void setSolid(Solid *s)
	{
		this->solid = s;
	}

	//GETTERS

	int computeVertexNum()
	{
		int num = 0;

		Loop *l = this->outerLoop;
		while(NULL != l)
		{
			num += l->computeVertexNum();
			l = l->getNext();
		}

		return num;
	}

	bool isInnerLoopExist()
	{
		return this->innerLoopExist;
	}

	Loop *getOuterLoop()
	{
		return this->outerLoop;
	}

	Solid *getSolid()
	{
		return this->solid;
	}

	Face *getNext()
	{
		return this->next;
	}

	void addInnerLoop(Loop *l)
	{
		l->setFace(this);
		if(NULL == this->outerLoop)
			return ;

		this->innerLoopExist = true;
		Loop *lp = this->outerLoop;
		while(NULL != lp->getNext())
			lp = lp->getNext();
		
		lp->getNext() = l;
		l->getNext() = NULL;
		l->isInner() = true;
	}

	//HELPER FUNCTIONS

	void drawFace()
	{

		Loop *l = this->outerLoop, *out_loop = l;
		
		l = l->getNext();

		while(NULL != l)
		{
			glStencilFunc(GL_ALWAYS, 1, 0xFF);//always pass stencil test
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//replace by '1' when pass
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);//do not write to framebuffer
			glDepthMask(GL_FALSE);//do not record the depth of the inner loop
			glStencilMask(0xFF);

			l->drawLoop();
			l = l->getNext();

		}

		glStencilFunc(GL_EQUAL, 0, 0xFF);//write when equal to '0'
		glDepthMask(GL_TRUE);//record depth
		glStencilOp(GL_ZERO, GL_KEEP, GL_KEEP);//make sure next face will not be affected 
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		out_loop->drawLoop();

	}

	void printStartIdx()
	{
		Loop *l = this->outerLoop;
		while(NULL != l)
		{
			l->printStartIdx();
			l = l->getNext();
		}
		printf("\n");
	}

	void printFace()
	{
		Loop *l = this->outerLoop;

		while(NULL != l)
		{
			l->printLoop();
			l = l->getNext();
		}
	}
};

class Solid
{
public :
	Face *firstFace;
	int face_num;

public :
	
	Solid()
	{
		face_num = 0;
	}

	void setFace(Face *f)
	{
		firstFace = f;
		++face_num;
	}

	void addFace(Face *f)
	{
		++face_num;
		f->setSolid(this);

		if(firstFace == NULL)
			firstFace = f;

		else
		{
			Face *iter = firstFace;
			while(iter->getNext() != NULL)
				iter = iter->getNext();
			iter->setNext(f);
		}
	}

	bool deleteFace(Face *f)
	{
		Face *tmp = this->firstFace;

		if(f == tmp)
		{
			this->firstFace = tmp->getNext();
			delete tmp;
			--this->face_num;
			return true;
		}
		else
		{
			Face *pre = tmp;
			tmp = tmp->getNext();
			while(tmp != f)
			{
				tmp = tmp->getNext();
				pre = pre->getNext();
			}

			if(NULL != tmp)//found
			{
				pre->setNext(tmp->getNext());
				delete tmp;
				--this->face_num;
				return true;
			}

			return false;//not found
		}
	}

	//GETTERS
	Face *getFirstFace()
	{
		return this->firstFace;
	}

	int getFaceNum()
	{
		return this->face_num;
	}

	//HELPER FUNCTIONS
	void printStartIdx()
	{
		Face *f = this->firstFace;
		f->printStartIdx();
		f = f->getNext();

		for(int i=0; i<face_num - 1; ++i)
		{
			f->printStartIdx();
			f = f->getNext();
		}
	}

	void printfFaces()
	{
		Face *f = this->firstFace;
		f->printFace();
		f = f->getNext();

		for(int i=0; i<face_num - 1; ++i)
		{
			f->printFace();
			f = f->getNext();
		}
	}

	int computeVertexNum()
	{
		int num = 0;

		Face *f = this->firstFace;
		num += f->computeVertexNum();
		f = f->getNext();

		for(int i=0; i<face_num - 1; ++i)
		{
			num += f->computeVertexNum();
			f = f->getNext();
		}

		return num;
	}

	int printfVerticesIntoBuffer(GLfloat *&vertices, GLfloat *&normals)
	{
		int v_num = this->computeVertexNum();

		vertices = new GLfloat[v_num * 3];
		normals = new GLfloat[v_num * 3];
		int p = 0;

		Face *f = this->firstFace;
		for(int i=0; i<face_num; ++i)
		{
			Loop *l = f->getOuterLoop();

			while(NULL != l)
			{
				l->setStartIdx(p/3);

				HalfEdge *hf = l->getFirstEdge(), *firstEdge = l->getFirstEdge(), *next_hf = hf->getNext();

				Vec3f normal; 
				hf->getVector().Cross3(normal, hf->getVector(), next_hf->getVector());
				normal.Normalize();

				Vertex *v = hf->getStartV();
				normals[p] = normal[0];
				vertices[p] = v->cordinate[0];
				p++;
				normals[p] = normal[1];
				vertices[p] = v->cordinate[1];
				p++;
				normals[p] = normal[2];
				vertices[p] = v->cordinate[2];
				p++;

				hf = hf->getNext();
				while(firstEdge != hf)
				{
					v = hf->getStartV();
					normals[p] = normal[0];
					vertices[p] = v->cordinate[0];
					p++;
					normals[p] = normal[1];
					vertices[p] = v->cordinate[1];
					p++;
					normals[p] = normal[2];
					vertices[p] = v->cordinate[2];
					p++;

					hf = hf->getNext();
				}

				l->setVertexNum(p/3 - l->getStartIdx());
				l->printLoop();
				l = l->getNext();
			}

			f = f->getNext();
		}

		return p / 3;
	}

	void drawFaces()
	{
		Face *f = this->firstFace;
		f->drawFace();
		f = f->getNext();

		for(int i=0; i<face_num - 1; ++i)
		{
			f->drawFace();
			f = f->getNext();
		}
	}
};

#endif