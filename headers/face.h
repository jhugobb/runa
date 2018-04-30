#ifndef FACE_H
#define FACE_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include "vertex.h"

class Face {
    public:
        Face();
        Face(Vertex* v1, Vertex* v2, Vertex* v3);
        Vertex* v1, *v2, *v3;
        QVector3D *n;
        double area;
        bool areTwins(Face f, Vertex* v);
        double normalFieldDeviation();
        Vertex* vertexOf(Face f, Vertex* v);
};

#endif // FACE_H
