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
    adjacent_faces = *(new QVector<Face*>());
    twin_faces = *(new QMap<Vertex*, QPair<Face *, Face *>>());
    cost = 0;
}

Vertex::Vertex(QVector3D coordinates) {
    coords = QVector3D(coordinates);
    normal = QVector3D();
    adjacent_faces = *(new QVector<Face*>());
    twin_faces = *(new QMap<Vertex*, QPair<Face *, Face *>>());
    cost = 0;
}

void Vertex::addFace(Face *f) {
    if (!adjacent_faces.contains(f)) {
        adjacent_faces.append(f);
    }
}

void Vertex::addNormal(QVector3D n) {
    normal = QVector3D(n);
} 

int Vertex::calculateCost(int count) {
    int size = adjacent_faces.size();
    twin_faces.clear();
    int checked[size];
    int found;
    for (int i = 0; i<size; i++) {
        checked[i] = 0;
    }
    double areaSum = 0;
    QVector3D normal_field = QVector3D();
    int i, j;
    for (i = 0; i < size; i++) {
        Face *f = adjacent_faces.at(i);
        areaSum += f->area;
        normal_field += f->area * (f->n);

        if (checked[i] < 2) {
            QPair<Face *, Face *> twins1, twins2;

            twins1.first = f;
            twins2.first = f;
            
            found = 0;

            for (j = i+1; j < size && (found < 2); j++) {
                if((checked[j] < 2) && f->areTwins(adjacent_faces.at(j), this)) {
                    if (found == 0)
                        twins1.second = adjacent_faces.at(j);
                    if (found == 1)
                        twins2.second = adjacent_faces.at(j);
                    found++;
                    checked[j]++;
                }
            }
            if (found == 1) {
                twin_faces.insert(twins1.first->vertexOf(twins1.second, this), twins1);
            } else if (found == 2) {
                twin_faces.insert(twins1.first->vertexOf(twins1.second, this), twins1);
                twin_faces.insert(twins2.first->vertexOf(twins2.second, this), twins2);
            }
            checked[i]+=found;
            
        }   
    }
    // TODO: Remove this testing
    bool t = false;

    QMapIterator<Vertex*, QPair<Face *, Face *>> iter(twin_faces);
    while (iter.hasNext()) {
        t = true;
        iter.next();
        QPair<Face *, Face *> tf = iter.value();
        Face *f1 = tf.first, *f2 = tf.second;
        double c = (f1->area + f2->area) * (1.0f - (f1->n.x()*f2->n.x() + f1->n.y()*f2->n.y() + f1->n.z()*f2->n.z())); // a . b == a^T * b
        cost+=c;
    }
    // TODO: Remove this testing
    if (!t) {
        count++;
    }
    cost += (areaSum - normal_field.length());
    // if (cost != 0) {
    //     cout << "here is the cost " << cost << endl;
    // }
    return count;
}

bool Vertex::operator==(const Vertex &v2) const {
    return ((coords == v2.coords) && (normal == v2.normal));
}

Vertex *Vertex::getOptimalEdge() {
    Vertex* result;
    double maxCost = std::numeric_limits<double>::max();
    double cost;
    cout << "Size of twin faces: " << twin_faces.size() << endl;

    QMapIterator<Vertex*, QPair<Face *, Face *>> i(twin_faces);
    while (i.hasNext()) {
        i.next();
        QPair<Face *, Face *> tf = i.value();
        Face *f1 = tf.first, *f2 = tf.second;
        cost = f1->normalFieldDeviation();
        cost += f2->normalFieldDeviation();
        if (cost < maxCost) {
            maxCost = cost;
            result = i.key();
            assert(result != NULL);
        }
    }
    return result;
}

void Vertex::getLinearPair(QPair<Eigen::Matrix3d, Eigen::Vector3d> result) {
    Eigen::Matrix3d resMatrix; //A
    resMatrix << 0, 0, 0, 0, 0, 0, 0, 0, 0;
    Eigen::Vector3d resVector(0,0,0); //b

    Eigen::Vector3d point(coords.x(), coords.y(), coords.z());
    
    for (Face *f : adjacent_faces) {
        Eigen::Vector3d norm(f->n.x(), f->n.y(), f->n.z());
        Eigen::Matrix3d aj = (norm * norm.transpose());
        resMatrix = resMatrix + aj;
        resVector = resVector + (aj * point);
    }
    result.first = resMatrix;
    result.second = resVector;
}

QVector<Face*> Vertex::replaceWith(QVector3D newCoords, QVector<Face*> result, Vertex *old) {
    coords = newCoords;
    for (Face *f : (old->adjacent_faces)) {
        f->replace(old, this);
        addFace(f);
    }
    for (Face *f : adjacent_faces) {
        f->recalculate();
        if (f->isDegenerate()) {
            adjacent_faces.removeAll(f);
            if (!result.contains(f)) {
                result.append(f);
            }
        }
    }
    assert(adjacent_faces.size() > 1);
    return result;
}
// TODO: optimize
void Vertex::recalculateCost(int count) {
    calculateCost(0);
    for (Face *f : adjacent_faces) {
        f->v1->calculateCost(count);
        f->v2->calculateCost(count);
        f->v3->calculateCost(count);
    }
}