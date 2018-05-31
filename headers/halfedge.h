#ifndef HALFEDGE_H
#define HALFEDGE_H

#include <QVector>

#include "face.h"
#include "vertex.h"

class HalfEdge {
   public:
    HalfEdge();
    Vertex *next;
    HalfEdge *twin;
    HalfEdge *nextEdge;
    Face *face;
    bool calculated;
    double cost;
    double calculateCost(double tolerance);
    double getArea();
    QVector3D getNormalField();
    bool isElegible();
};

#endif  // HALFEDGE_H