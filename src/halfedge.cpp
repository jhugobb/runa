#include "headers/halfedge.h"

HalfEdge::HalfEdge() {
    cost = 0;
}

void HalfEdge::calculateCost(double *areaSum, double *costSum, QVector3D *normalField) {
    double area1 = face->area;
    double area2 = twin->face->area;

    (*areaSum) += area1;
    QVector3D n1 = face->n;
    *normalField = *normalField + (area1 *n1);
    QVector3D n2 = twin->face->n;

    cost = (area1 + area2) * (QVector3D::dotProduct(n1, n2));
    twin->cost = cost;
    *costSum += cost;
}