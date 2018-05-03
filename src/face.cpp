#include "headers/vertex.h"
#include "headers/face.h"

#include <iostream>

using namespace std;

Face::Face() {}

Face::Face(Vertex* pos1, Vertex* pos2, Vertex* pos3) {
    pos1->addFace(this);
    pos2->addFace(this);
    pos3->addFace(this);
    v1 = pos1;
    v2 = pos2;
    v3 = pos3;
    n = QVector3D(QVector3D::crossProduct((v2->coords - v1->coords), (v3->coords - v1->coords)));
    area = n.length()/2.0f;
}

bool Face::operator==(const Face &f2) const {
    bool v1t = (v1 == (f2.v1) || v1 == (f2.v2) || v1 == (f2.v3));
    bool v2t = (v2 == (f2.v1) || v2 == (f2.v2) || v2 == (f2.v3));
    bool v3t = (v3 == (f2.v1) || v3 == (f2.v2) || v3 == (f2.v3));
    return (v1t && v2t && v3t);
}

bool Face::areTwins(Face *f, Vertex *v) {
    bool v1t = (v1 == (f->v1) || v1 == (f->v2) || v1 == (f->v3));
    bool v2t = (v2 == (f->v1) || v2 == (f->v2) || v2 == (f->v3));
    bool v3t = (v3 == (f->v1) || v3 == (f->v2) || v3 == (f->v3));

    bool ver1 = (v1t && v2t && (v == (v1) || v == (v2)));
    bool ver2 = (v1t && v3t && (v == (v1) || v == (v3)));
    bool ver3 = (v2t && v3t && (v == (v2) || v == (v3)));

    return (ver1 || ver2 || ver3);
}

double Face::normalFieldDeviation() {
    // TODO: CHANGE GIVEN THE ALGORITM-> PAPER IS CONFUSING
    return area;
}

Vertex* Face::vertexOf(Face *f, Vertex *v) {
    if ((v1 == (f->v1) || v1 == (f->v2) || v1 == (f->v3)) && !(v1 == (v)))
        return v1;
    if ((v2 == (f->v1) || v2 == (f->v2) || v2 == (f->v3)) && !(v2 == (v)))
        return v2;
    return v3;
}

void Face::replace(Vertex *old, Vertex *actual) {
    if (v1 == (old)) {
        v1 = actual;
    } else if (v2 == (old)) {
        v2 = actual;
    } else {
        v3 = actual;
    }
    recalculate();
}

void Face::recalculate() {
    n = QVector3D(QVector3D::crossProduct((v2->coords - v1->coords), (v3->coords - v1->coords)));
    area = n.length() / 2.0f;
}
