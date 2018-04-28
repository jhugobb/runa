#include "headers/vertex.h"
#include "headers/face.h"

#include <iostream>

using namespace std;

Face::Face() {}

Face::Face(Vertex* pos1, Vertex* pos2, Vertex* pos3) {
    v1 = *pos1;
    v2 = *pos2;
    v3 = *pos3;
    pos1->addFace(this);
    pos2->addFace(this);
    pos3->addFace(this);
    n = new QVector3D(QVector3D::crossProduct((*v2.coords - *v1.coords), (*v3.coords - *v1.coords)));
    area = n->length()/2.0f;
    n->normalize();
}


bool Face::areTwins(Face f) {
    bool v1t = (v1.equals(f.v1) || v1.equals(f.v2) || v1.equals(f.v3));
    bool v2t = (v2.equals(f.v1) || v2.equals(f.v2) || v2.equals(f.v3));
    bool v3t = (v3.equals(f.v1) || v3.equals(f.v2) || v3.equals(f.v3));
    return ((v1t && v2t) || (v1t && v3t) || (v2t && v3t));
}
