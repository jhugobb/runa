#include "headers/halfedge.h"
#include <iostream>

HalfEdge::HalfEdge() {
    cost = 0;
    calculated = false;
}

double HalfEdge::calculateCost() {
    double area1 = face->area;
    double area2 = twin->face->area;

    QVector3D n1 = face->n;
    QVector3D n2 = twin->face->n;
    cost = (area1 + area2) * (1.0 - 0.91 * QVector3D::dotProduct(n1, n2));
    twin->cost = cost;
    return cost;
}

double HalfEdge::getArea(){
    return face->area;
}

QVector3D HalfEdge::getNormalField(){
    return face->area * face->n;
}

bool HalfEdge::isElegible() {
    Vertex *origin = twin->next;
    Vertex *dest = next;
    QVector<Vertex *> neighbors1, neighbors2;
    neighbors1 = origin->getNeighborhood();
    neighbors2 = dest->getNeighborhood();
    assert(neighbors1.size() != 0);
    assert(neighbors2.size() != 0);
    int count = 0;
    for (Vertex *v : neighbors1) {
        if (neighbors2.contains(v)) {
            count++;
        }
    }
    return count == 2;
}