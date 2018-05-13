#ifndef HALFEDGE_H
#define HALFEDGE_H

#include <QVector>

#include "vertex.h"
#include "face.h"

class HalfEdge {
    public: 
        HalfEdge();
        Vertex *next;
        HalfEdge *twin;
        HalfEdge *nextEdge;
        Face *face;
        double cost;
        void calculateCost(double *areaSum, double *costSum, QVector3D *normalField);
};

#endif // HALFEDGE_H