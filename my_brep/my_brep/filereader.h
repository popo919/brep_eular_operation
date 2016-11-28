#ifndef _filereader_h_
#define _filereader_h_
#include "Brep.h"
//#include <iostream>
#include <fstream>

class FileReader
{
private :

	float len;
	Vec3f dir;

public :
	//PARSE FUNCTION
	Face* readFromFile(char *path, Solid *& s)
	{
		char *buffer = new char[100];
		Loop *loop;
		Face *f;
		Vertex *first_v, *v;
		int num;

		ifstream in("face.txt"); 
		in.getline (buffer,100); 

		while (!in.eof() )  
        {  
			if(!strcmp(buffer, ""))
				in.getline (buffer,100); 

			//parse outer loop
			if(!strcmp(buffer, "outer"))
			{
				in>>num;
				for(int i=0; i<num; ++i)
				{
					Vec3f cord;
					float x, y, z;
					in>>x>>y>>z;
					cord.Set(x, y, z);

					if(0 == i)
					{
					s = mvsf(cord);
					loop = s->getFirstFace()->getOuterLoop();
					first_v = loop->getFirstVertex();
					v = first_v;
					}
					else
					{
					HalfEdge *hf = mev(v, loop, cord);
					v = hf->getEndV();
					}
				}

				f = mef(v, first_v, loop);
				s->addFace(f);

				in.getline (buffer,100); 
				in.getline (buffer,100); 
			}

			Loop *inner_loop;
			Face *inside_f;
			
			//parse inner loop
			if(!strcmp(buffer, "inner"))
			{
				in>>num;
				for(int i=0; i<num; ++i)
				{
					Vec3f cord;
					float x, y, z;
					in>>x>>y>>z;
					cord.Set(x, y, z);

					if(0 == i)
					{
					HalfEdge *bridge = mev(f->getOuterLoop()->getFirstEdge()->getStartV(), 
										   f->getOuterLoop(), cord);
					inner_loop = kemr(f->getOuterLoop(), bridge);
					first_v = bridge->getEndV(), v = first_v;
					}
					else
					{
					HalfEdge *hf = mev(v, inner_loop, cord);
					v = hf->getEndV();
					}
				}

				inside_f = mef(v, first_v, inner_loop);
				s->addFace(inside_f);

				in.getline (buffer,100); 
				in.getline (buffer,100);
			}
			
			//parse dir
			if(!strcmp(buffer, "dir"))
			{
				float x, y, z;
				in>>x>>y>>z;
				dir.Set(x, y, z);

				in.getline (buffer,100); 
				in.getline (buffer,100);
			}
			
			//parse length
			if(!strcmp(buffer, "len"))
			{
				in>>len;
				in.getline (buffer,100);
			}
        } 

		return f;
	}


	//GETTERS
	Vec3f getDir()
	{
		return this->dir;
	}

	float getLen()
	{
		return this->len;
	}
};

#endif