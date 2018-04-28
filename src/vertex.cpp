#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <iostream>

#include "headers/vertex.h"
#include "headers/face.h"

using namespace std;

Vertex::Vertex() {}


Vertex::Vertex(QVector3D coordinates, QVector3D n) {
    coords = new QVector3D(coordinates);
    normal = new QVector3D(n);
    adjacent_faces = new QVector<Face*>();
    cost = 0;
}

Vertex::Vertex(double x, double y, double z, double nx, double ny, double nz) {
    coords = new QVector3D(x, y, z);
    normal = new QVector3D(nx, ny, nz);
    adjacent_faces = new QVector<Face*>();
    cost = 0;
}

void Vertex::addFace(Face* f) {
    adjacent_faces->append(f);
}

int Vertex::calculateCost(int count) {
    QVector<QPair<Face, Face>> twin_faces;
    int size = adjacent_faces->size();
    bool found;
    int checked[size] = {};
    double areaSum = 0;
    QVector3D normal_field = QVector3D();
    int i, j;
    for (i = 0; i < size; i++) {
        Face f = *adjacent_faces->at(i);
        areaSum += f.area;
        normal_field += f.area * (*f.n);

        if (checked[i] != 2) {
            QPair<Face, Face> twins1, twins2;

            twins1.first = f;
            twins2.first = f;

            found = false;

            for (j = i+1; j < size && !found; j++) {
                if((checked[j] != 2) && f.areTwins(*adjacent_faces->at(j))) {
                    twins1.second = *adjacent_faces->at(j);
                    found = true;
                    checked[i]++;
                    checked[j]++;
                }
            }

            found = false;

            for (; j < size && !found; j++) {
                if((checked[j] != 2) && f.areTwins(*adjacent_faces->at(j))) {
                    twins2.second = *adjacent_faces->at(j);
                    found = true;
                    checked[i]++;
                    checked[j]++;
                }
            }
            twin_faces.append(twins1);
            twin_faces.append(twins2);            
        }   
    }
    bool t = false;
    for (QPair<Face, Face> tf : twin_faces) {
        t = true;
        // NFD
        Face f1 = tf.first, f2 = tf.second;
        double c = (f1.area + f2.area) * (1.0f - (f1.n->x()*f2.n->x() + f1.n->y()*f2.n->y() + f1.n->z()*f2.n->z())); // a . b == a^T * b
        cost+=c;
    }
    if (!t) {
        count++;
    }
    cost += (areaSum - normal_field.length());
    if (cost != 0) {
        //cout << "here is the cost " << cost << endl;
    }
    return count;
}

bool Vertex::equals(Vertex v) {
    return ((v.coords == coords) && (v.normal == normal));
}
