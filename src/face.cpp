#include "headers/vertex.h"
#include "headers/face.h"

#include <iostream>

using namespace std;

Face::Face() {}

Face::Face(Vertex* pos1, Vertex* pos2, Vertex* pos3) {
    v1 = pos1;
    v2 = pos2;
    v3 = pos3;
    pos1->addFace(this);
    pos2->addFace(this);
    pos3->addFace(this);
    n = new QVector3D(QVector3D::crossProduct((*v2->coords - *v1->coords), (*v3->coords - *v1->coords)));
    if (n->x()==0 && n->y()==0 && n->z()==0) {
        cout <<(v2->coords->x())<< " " << (v2->coords->y()) << " " << (v2->coords->z()) << endl;
    }
    area = n->length()/2.0f;
    n->normalize();
}


bool Face::areTwins(Face f, Vertex* v) {
    bool v1t = (v1->equals(f.v1) || v1->equals(f.v2) || v1->equals(f.v3));
    bool v2t = (v2->equals(f.v1) || v2->equals(f.v2) || v2->equals(f.v3));
    bool v3t = (v3->equals(f.v1) || v3->equals(f.v2) || v3->equals(f.v3));

    bool ver1 = (v1t && v2t && (v->equals(v1) || v->equals(v2)));
    bool ver2 = (v1t && v3t && (v->equals(v1) || v->equals(v3)));
    bool ver3 = (v2t && v3t && (v->equals(v2) || v->equals(v3)));

    return (ver1 || ver2 || ver3);
}

double Face::normalFieldDeviation() {
    // TODO: CHANGE GIVEN THE ALGORITM. PAPER IS CONFUSING
    return area;
}

Vertex* Face::vertexOf(Face f, Vertex* v) {
    if ((!v1->equals(v)) && (v1->equals(f.v1) || v1->equals(f.v2) || v1->equals(f.v3)))
        return v1;
    if ((!v2->equals(v)) && (v2->equals(f.v1) || v2->equals(f.v2) || v2->equals(f.v3)))
        return v2;
    if ((!v3->equals(v)) && (v3->equals(f.v1) || v3->equals(f.v2) || v3->equals(f.v3)))
        return v3;
    return NULL;
}
