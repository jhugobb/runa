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
        bool calculated;
        double cost;
        double calculateCost();
        double getArea();
        QVector3D getNormalField();
};

#endif // HALFEDGE_H