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
        Vertex(QVector3D coords);
        QVector3D coords;
        QVector3D normal;
        double cost;
        QVector<Face *> adjacent_faces;
        QVector<QPair<Face *, Face *>> twin_faces;
        void addFace(Face *f);
        void addNormal(QVector3D n);
        int calculateCost(int count);
        bool operator==(const Vertex &v2) const;
        Vertex *getOptimalEdge();
        void getLinearPair(QPair<Eigen::Matrix3d, Eigen::Vector3d>);
        QVector<Face *> replaceWith(QVector3D newCoords, QVector<Face *> result, Vertex *old);
        void recalculateCost(int count);
};

#endif