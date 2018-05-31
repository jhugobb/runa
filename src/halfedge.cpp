#include "headers/halfedge.h"
#include <iostream>

HalfEdge::HalfEdge() {
    cost = 0;
    calculated = false;
}

double HalfEdge::calculateCost(double tolerance) {
    double area1 = face->area;
    double area2 = twin->face->area;

    QVector3D n1 = face->n;
    QVector3D n2 = twin->face->n;
    Vertex *v1 = next, *v2 = twin->next;
    double sqdist = pow((v2->coords.x() - v1->coords.x()), 2) + pow((v2->coords.y() - v1->coords.y()), 2) + pow((v2->coords.z() - v1->coords.z()), 2);
    cost = (area1 + area2) * (1.0 - tolerance * QVector3D::dotProduct(n1, n2)) * sqrt(sqdist);
    twin->cost = cost;
    return cost;
}

double HalfEdge::getArea() {
    return face->area;
}

QVector3D HalfEdge::getNormalField() {
    return face->area * face->n;
}

bool HalfEdge::isElegible() {
    Vertex *origin = twin->next;
    Vertex *dest = next;
    QVector<Vertex *> neighbors1, neighbors2;
    neighbors1 = origin->getNeighborhood();
    neighbors2 = dest->getNeighborhood();
    int count = 0;
    for (Vertex *v : neighbors1) {
        if (neighbors2.contains(v)) {
            count++;
        }
    }
    return count == 2;
}