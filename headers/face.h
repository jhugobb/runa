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
        Vertex *v1, *v2, *v3;
        QVector3D n;
        double area;
        bool operator==(const Face &f2) const;
        bool areTwins(Face *f, Vertex *v);
        double normalFieldDeviation();
        Vertex* vertexOf(Face *f, Vertex *v);
        void replace(Vertex *old, Vertex *actual);
        void recalculate();
};

#endif // FACE_H
