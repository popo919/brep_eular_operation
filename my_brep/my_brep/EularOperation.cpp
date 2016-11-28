#include "EularOperation.h"

Solid *mvsf(Vec3f v)//specify the cordinate of the first vertex
{
	Solid *solid = new Solid();

	Face *face = new Face(solid);
	solid->setFace(face);

	Loop *loop = new Loop(face);
	face->setOuterLoop(loop);

	Vertex *vertex = new Vertex(v);
	loop->setFirstVertex(vertex);

	return solid;
}



HalfEdge *mev(Vertex *v1, Loop *l, Vec3f cord)
{
	Vertex *v, *v2 = new Vertex(cord);
	HalfEdge *he1 , *he2, *he3;
	Edge *eg;

	he1 = new HalfEdge(v1, v2);
	he2 = new HalfEdge(v2, v1);

	eg = new Edge(he1, he2);
	he1->setEdge(eg);
	he2->setEdge(eg);

	he1->setNextHF(he2);
	he2->setPrevHF(he1);

	he1->setLoop(l);
	he2->setLoop(l);

	if(l->getFirstEdge() == NULL)
	{
		he2->setNextHF(he1);
		he1->setPrevHF(he2);
		l->setFirstEdge(he1);
	}
	else
	{

		for(he3 = l->getFirstEdge(); he3->getEndV() != v1; 
			he3 = he3->getNext());
		
		he2->setNextHF(he3->getNext());
		he3->getNext()->setPrevHF(he2);
		he3->setNextHF(he1);
		he1->setPrevHF(he3);
	}

	he1->getPartner() = he2;
	he2->getPartner() = he1;
	return he1;
}


Face *mef(Vertex *from, Vertex *to, Loop *lp)
{

	HalfEdge *in_from, *out_from, *in_to, *out_to;
	HalfEdge *he1, *he2;
	Edge *e;
	Face *face = new Face(lp->getFace()->getSolid());
	Loop *loop;
	bool first_in_from = true, first_in_to = true;

	HalfEdge *firstEdge = lp->getFirstEdge();
	bool flag = true;

	for(HalfEdge *iter = firstEdge; iter != firstEdge || flag; 
		iter = iter->getNext())
	{
		if(flag) flag = false;

		if(iter->getEndV() == to)
		{
			if(first_in_to)
			{
			in_to = iter;
			first_in_to = false;
			}
			out_to = iter->getNext();
		}
		if(iter->getEndV() == from)
		{
			if(first_in_from)
			{
			in_from = iter;
			first_in_from = false;
			}
			out_from = iter->getNext();
		}
	}


	he1 = new HalfEdge(from, to);
	he2 = new HalfEdge(to, from);

	he1->getPartner() = he2;
	he2->getPartner() = he1;

	he1->setNextHF(out_to);
	he1->setPrevHF(in_from);
	in_from->setNextHF(he1);
	out_to->setPrevHF(he1);

	he2->setNextHF(out_from);
	he2->setPrevHF(in_to);
	in_to->setNextHF(he2);
	out_from->setPrevHF(he2);

	e = new Edge(he1, he2);
	loop = new Loop(face);
	face->setOuterLoop(loop);

	lp->setFirstEdge(he2);
	he2->setLoop(lp);
	loop->setFirstEdge(he1);
	he1->setLoop(loop);

	he1->setEdge(e);
	he2->setEdge(e);

	he1->getPartner() = he2;
	he2->getPartner() = he1;

	return face;
}

Loop* kemr(Loop *l, HalfEdge *he1)
{

	HalfEdge *he2 = he1->getPartner();

	he1->getPrev()->setNextHF(he2->getNext());
	he2->getNext()->setPrevHF(he1->getPrev());

	he2->getPrev()->setNextHF(he1->getNext());
	he1->getNext()->setPrevHF(he2->getPrev());

	Loop *loop = new Loop(l->getFace());
	loop->setFirstVertex(he1->getEndV());
	if(he1->getNext() != he2)
		loop->setFirstEdge(he1->getNext());

	//l->setFirstEdge(he1->getPrev());
	l->getFace()->addInnerLoop(loop);

	return loop;
}

void kfmrh(Face *f1, Face *f2)
{
	Loop *lp = f2->getOuterLoop();

	f1->addInnerLoop(lp);

	f1->getSolid()->deleteFace(f2);
}

void sweep(Face *f, float len, Vec3f dir)
{
	Loop *loop = f->getOuterLoop()->getFirstEdge()->getPartner()->getLoop();
	Solid *solid = f->getSolid();

	/*---------------------------------------------------------------*/
	//generate hole side faces

	for(Loop *l = f->getOuterLoop(); NULL != l; l = l->getNext())
	{
		Loop *lp = l->getFirstEdge()->getPartner()->getLoop();
		HalfEdge *hf = lp->getFirstEdge();

		int num = lp->computeVertexNum();
		HalfEdge** edges = new HalfEdge*[num];

		for(int i=0; i<num; ++i)//side edges
		{
			Vertex *fromV = hf->getStartV();
			hf = hf->getNext();
			Vec3f to_cord = fromV->cordinate + len * dir;
			edges[i] = mev(fromV, lp, to_cord);
		}

		for(int i=0; i<num; ++i)//side faces
		{
			Vertex* toV = edges[i]->getEndV();
			Vertex* fromV = edges[(i+1)%num]->getEndV();
			Face* f = mef(fromV, toV, lp);
			solid->addFace(f);
		}

		//call kfmrh when current loop is inner loop
		if(l != f->getOuterLoop())
		{
			Face *inside_f = l->getFirstEdge()->getPartner()->getLoop()->getFace();
			kfmrh(loop->getFace(), inside_f);
		}
	}
}