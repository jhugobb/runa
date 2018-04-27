#ifndef VERTEX_H
#define VERTEX_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>

class Vertex
{
public:
    Vertex(double x, double y, double z, double nx, double ny, double nz);
    QVector3D coords;
    QVector3D normal;
    double cost;
    QVector<Face> adjacent_faces;
    addFace(Face f);
    calculateCost();
    bool equals(Vertex v);

};

#endif