#ifndef VERTEX_H
#define VERTEX_H

#include <QMap>
#include <QVector2D>
#include <QVector3D>
#include <QVector>
#include <eigen3/Eigen/Dense>

class Face;
class HalfEdge;

class Vertex {
   public:
    Vertex(QVector3D coordiates, QVector3D n);
    Vertex(QVector3D coords);
    QVector3D coords;
    QVector3D normal;
    double cost;
    HalfEdge *edge;
    void addNormal(QVector3D n);
    void calculateCost(double tolerance);
    bool operator==(const Vertex &v2) const;
    Vertex *getOptimalEdge();
    QPair<Eigen::Matrix3d, Eigen::Vector3d> getLinearPair(QPair<Eigen::Matrix3d, Eigen::Vector3d>);
    QVector<Face *> replaceWith(QVector3D newCoords, QVector<Face *> result, Vertex *old, HalfEdge *he);
    QVector<Vertex *> recalculateCost(double tolerance, QVector<Vertex *> result);
    QVector<Vertex *> getChanged();
    void calculateNormal();
    HalfEdge *getEdge(Vertex *v);
    QVector<Vertex *> getNeighborhood();
};

#endif