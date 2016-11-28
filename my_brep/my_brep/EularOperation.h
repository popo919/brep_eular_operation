#ifndef _eular_h_
#define _eular_h_
#include "Brep.h"


HalfEdge *mev(Vertex *v1, Loop *l, Vec3f cord);

Solid *mvsf(Vec3f v);

Face *mef(Vertex *from, Vertex *to, Loop *lp);

Loop* kemr(Loop *l, HalfEdge *he1);

//kill f2 and make f2's outer ring f1's inner ring
void kfmrh(Face *f1, Face *f2);

void sweep(Face *f, float len, Vec3f dir);

#endif