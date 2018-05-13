#include "headers/halfedge.h"

HalfEdge::HalfEdge() {
    cost = 0;
    calculated = false;
}

double HalfEdge::calculateCost() {
    double area1 = face->area;
    double area2 = twin->face->area;

    QVector3D n1 = face->n;
    QVector3D n2 = twin->face->n;

    cost = (area1 + area2) * (1.0f - (n1.x()*n2.x() + n1.y()*n2.y() + n1.z()*n2.z()));
    twin->cost = cost;
    return cost;
}

double HalfEdge::getArea(){
    return face->area;
}

QVector3D HalfEdge::getNormalField(){
    return face->area * face->n;
}