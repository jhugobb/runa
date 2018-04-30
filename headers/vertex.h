#ifndef VERTEX_H
#define VERTEX_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <eigen3/Eigen/Dense>

class Face;

class Vertex {
    public:
        Vertex();
        Vertex(QVector3D coordiates, QVector3D n);
        Vertex(double x, double y, double z, double nx, double ny, double nz);
        QVector3D *coords;
        QVector3D *normal;
        double cost;
        QVector<Face*> *adjacent_faces;
        QVector<QPair<Face, Face>> twin_faces;
        void addFace(Face* f);
        int calculateCost(int count);
        bool equals(Vertex* v);
        Vertex* getOptimalEdge();
        void getLinearPair(QPair<Eigen::Matrix3d, Eigen::Vector3d>);

};

#endif