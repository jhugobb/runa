#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <iostream>
#include <limits>

#include "headers/vertex.h"
#include "headers/face.h"

using namespace std;

Vertex::Vertex(QVector3D coordinates, QVector3D n) {
    coords = QVector3D(coordinates);
    normal = QVector3D(n);
    cost = 0;
}

Vertex::Vertex(QVector3D coordinates) {
    coords = QVector3D(coordinates);
    normal = QVector3D();
    cost = 0;
}

void Vertex::addNormal(QVector3D n) {
    normal = QVector3D(n);
} 

int Vertex::calculateCost(int count) {
    double costSum = 0, areaSum = 0;
    QVector3D normalField = QVector3D(0,0,0);
    HalfEdge *curr = edge;
    do {
        areaSum += curr->getArea();
        normalField = normalField + curr->getNormalField();
        if (!false) {
            costSum += curr->calculateCost();
            curr->calculated = true;
            curr->twin->calculated = true;
        } else costSum += curr->cost;

        curr = curr->twin->nextEdge;
    } while (curr != edge);
    cost = costSum + ((areaSum) - normalField.length());
    return count;
}

bool Vertex::operator==(const Vertex &v2) const {
    return ((coords == v2.coords) && (normal == v2.normal));
}

Vertex *Vertex::getOptimalEdge() {
    Vertex* result = (Vertex *) NULL;
    double maxCost = std::numeric_limits<double>::max();
    HalfEdge *best = edge;
    do {
        if (best->cost < maxCost && best->isElegible()) {
            result = best->next;
            maxCost = best->cost;
        }
        best = best->twin->nextEdge;
    } while (best != edge);
    return result;
}

void Vertex::getLinearPair(QPair<Eigen::Matrix3d, Eigen::Vector3d> result) {
    Eigen::Matrix3d resMatrix; //A
    resMatrix << 0, 0, 0, 0, 0, 0, 0, 0, 0;
    Eigen::Vector3d resVector(0,0,0); //b

    Eigen::Vector3d point(coords.x(), coords.y(), coords.z());
    HalfEdge *curr = edge;
    Face *f;
    do {
        f = curr->face;
        Eigen::Vector3d norm(f->n.x(), f->n.y(), f->n.z());
        Eigen::Matrix3d aj = (norm * norm.transpose());
        resMatrix = resMatrix + aj;
        resVector = resVector + (aj * point);
        curr = curr->twin->nextEdge;
    } while (curr != edge) ;

    result.first = resMatrix;
    result.second = resVector;
}

QVector<Face*> Vertex::replaceWith(QVector3D newCoords, QVector<Face*> result, Vertex *actual, HalfEdge *he) {
    actual->coords = newCoords;
    if ((actual->edge->face == he->twin->face) || (actual->edge->face == he->face))
        actual->edge = he->twin->nextEdge->nextEdge->twin->nextEdge->nextEdge->twin;
    HalfEdge *curr = he->twin;
    do {
        if (curr->twin->next != actual) curr->next = actual;
        curr->face->replace(this, actual);
        if (curr->face->isDegenerate()){
            curr->face->changeEdges();
            result.append(curr->face);
        }
        curr->calculated = false;
        curr->twin->calculated = false;
        curr = curr->nextEdge->twin;
    } while (curr != he->twin);

    he->nextEdge->nextEdge->twin->twin = he->nextEdge->twin;
    he->nextEdge->twin->twin = he->nextEdge->nextEdge->twin;

    he->twin->nextEdge->twin->twin = he->twin->nextEdge->nextEdge->twin;
    he->twin->nextEdge->nextEdge->twin->twin = he->twin->nextEdge->twin;
    return result;
}

QVector<Vertex *> Vertex::recalculateCost(int count, QVector<Vertex *> result) {
    for (Vertex *v : result) {
        v->calculateCost(count);
    }
    return result;
}

QVector<Vertex *> Vertex::getChanged() {
    QVector<Vertex *> result = *(new QVector<Vertex *>());
    HalfEdge *curr = edge;
    do {
        if (!result.contains(curr->next)) {
            result.append(curr->next);
        }
        curr = curr->twin->nextEdge;
    } while (curr != edge);
    return result;
}

void Vertex::calculateNormal() {
    QVector3D n;
    double count = 0;
    HalfEdge *curr = edge;
    Face *f;
    do {
        f = curr->face;
        count++;
        normal = normal + f->area * (f->n);
        curr = curr->twin->nextEdge;
    } while (curr != edge);
    normal = normal/count;
    normal = n;
    normal.normalize();
}

HalfEdge *Vertex::getEdge(Vertex *v) {
    HalfEdge *curr = edge;
    do {
        if (curr->next == v) return curr;
        curr = curr->twin->nextEdge;
    } while (curr != edge);
    assert(false);
    return NULL;
}

QVector<Vertex *> Vertex::getNeighborhood() {
    QVector<Vertex *> result;
    HalfEdge *curr = edge;
    do {
        result.append(curr->next); 
        curr = curr->twin->nextEdge;
    } while (curr != edge);
    return result;
}