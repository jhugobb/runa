#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include "headers/vertex.h"
#include "headers/face.h"

Vertex::Vertex(double x, double y, double z, double nx, double ny, double nz) {
    coords = new QVector3D(x, y, z);
    normal = new QVector3D(nx, ny, nz);
    adjacent_faces = new QVector();
    cost = 0;
}

Vertex::addFace(Face f) {
    adjacent_faces.append(f);
}

Vertex::calculateCost() {
    QVector<QPair<Face, Face>> twin_faces;
    int size = adjacent_faces.size();
    bool found;
    int checked[size] = {0};
    double areaSum = 0;
    QVector3D normal_field;

    for (int i = 0; i < size; i++) {
        Face f = adjacent_faces.at(i);

        areaSum += f.area;
        normal_field+= (f.area * f.n);

        if (checked[i] != 2) {
            QPair<Face, Face> twins1, twins2;

            twins1.first = f;
            twins2.first = f;

            found = false;

            for (int j = i+1; j < size && !found; j++) {
                if((checked[j] != 2) && f.areTwins(adjacent_faces.at(j))) {
                    twins1.second = adjacent_faces.at(j);
                    found = true;
                    checked[i]++;
                    checked[j]++;
                }
            }

            found = false;

            for (; j < size && !found; j++) {
                if((checked[j] != 2) && f.areTwins(adjacent_faces.at(j))) {
                    twins2.second = adjacent_faces.at(j);
                    found = true;
                    checked[i]++;
                    checked[j]++;
                }
            }
            twin_faces.append(twin1);
            twin_faces.append(twin2);            
        }   
    }

    for (QPair<Face, Face> tf : twin_faces) {
        // NFD
        Face f1 = tf.first, f2 = tf.second;
        double c = (f1.area + f2.area) * (1.0f - QVector3D::dotProduct(f1.n, f2.n)); // a . b == a^T * b
        cost+=c;
    }

    cost+= (areaSum - normal_field.length());
}

bool Vertex::equals(Vertex v) {
    return ((v.coords == coords) && (v.normal == normal));
}
