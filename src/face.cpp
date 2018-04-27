#include "headers/vertex.h"
#include "headers/face.h"

Face::Face(Vertex pos1, Vertex pos2, Vertex pos3) {
    v1 = pos1;
    v2 = pos2;
    v3 = pos3;
    n = QVector3D::crossProduct((v2.coords - v1.coords), (v3.coords - v1.coords));
    area = n.length()/2.0f;
    n.normalize();
}


bool Face::areTwins(Face f) {
    bool v1 = (v1.equals(f.v1) || v1.equals(f.v2) || v1.equals(f.v3));
    bool v2 = (v2.equals(f.v1) || v2.equals(f.v2) || v2.equals(f.v3));
    bool v3 = (v3.equals(f.v1) || v3.equals(f.v2) || v3.equals(f.v3));
    return ((v1 && v2) || (v1 && v3) || (v2 && v3));
}
