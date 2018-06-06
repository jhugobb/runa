#ifndef FACE_H
#define FACE_H

#include <QString>
#include <QStringList>
#include <QVector2D>
#include <QVector3D>
#include <QVector>

#include "halfedge.h"
#include "vertex.h"

class Face {
   public:
    Face();
    Face(Vertex *v1, Vertex *v2, Vertex *v3);
    Vertex *v1, *v2, *v3;
    QVector3D n;
    double area;
    HalfEdge *edge;
    bool operator==(const Face &f2) const;
    bool areTwins(Face *f, Vertex *v);
    Vertex *vertexOf(Face *f, Vertex *v);
    void replace(Vertex *old, Vertex *actual);
    void recalculate();
    bool isDegenerate();
    void changeEdges();
    bool reorientCoherently();
};

#endif  // FACE_H
