
#include "headers/face.h"

#include <iostream>

using namespace std;

Face::Face() {}

Face::Face(Vertex *pos1, Vertex *pos2, Vertex *pos3) {
    v1 = pos1;
    v2 = pos2;
    v3 = pos3;
    n = QVector3D(QVector3D::crossProduct((v2->coords - v1->coords), (v2->coords - v3->coords))).normalized();
    double edge1 = sqrt(pow((v2->coords.x() - v1->coords.x()), 2) + pow((v2->coords.y() - v1->coords.y()), 2) + pow((v2->coords.z() - v1->coords.z()), 2));
    double edge2 = sqrt(pow((v2->coords.x() - v3->coords.x()), 2) + pow((v2->coords.y() - v3->coords.y()), 2) + pow((v2->coords.z() - v3->coords.z()), 2));
    double edge3 = sqrt(pow((v3->coords.x() - v1->coords.x()), 2) + pow((v3->coords.y() - v1->coords.y()), 2) + pow((v3->coords.z() - v1->coords.z()), 2));
    double p = (edge1 + edge2 + edge3) / 2.0;
    area = sqrt(p * (p - edge1) * (p - edge2) * (p - edge3));
}

bool Face::operator==(const Face &f2) const {
    bool v1t = (v1 == (f2.v1) || v1 == (f2.v2) || v1 == (f2.v3));
    bool v2t = (v2 == (f2.v1) || v2 == (f2.v2) || v2 == (f2.v3));
    bool v3t = (v3 == (f2.v1) || v3 == (f2.v2) || v3 == (f2.v3));
    return (v1t && v2t && v3t);
}

bool Face::areTwins(Face *f, Vertex *v) {
    bool vt = ((v1 == v) ^ (v2 == v) ^ (v3 == v));
    bool v1t = (v1 == (f->v1) || v1 == (f->v2) || v1 == (f->v3));
    bool v2t = (v2 == (f->v1) || v2 == (f->v2) || v2 == (f->v3));
    bool v3t = (v3 == (f->v1) || v3 == (f->v2) || v3 == (f->v3));

    bool ver1 = (v1t && v2t);
    bool ver2 = (v1t && v3t);
    bool ver3 = (v2t && v3t);

    return vt && (ver1 || ver2 || ver3);
}

Vertex *Face::vertexOf(Face *f, Vertex *v) {
    if ((v1 == (f->v1) || v1 == (f->v2) || v1 == (f->v3)) && (v1 != (v)))
        return v1;
    if ((v2 == (f->v1) || v2 == (f->v2) || v2 == (f->v3)) && (v2 != (v)))
        return v2;
    if ((v3 == (f->v1) || v3 == (f->v2) || v3 == (f->v3)) && (v3 != (v)))
        return v3;
    return NULL;
}

void Face::replace(Vertex *old, Vertex *actual) {
    if (v1 == (old)) {
        v1 = actual;
    } else if (v2 == (old)) {
        v2 = actual;
    } else if (v3 == (old)) {
        v3 = actual;
    }
    recalculate();
    //reorientCoherently();
}

void Face::recalculate() {
    QVector3D oldNormal = n;
    n = QVector3D(QVector3D::crossProduct((v2->coords - v1->coords), (v2->coords - v3->coords))).normalized();
    double edge1 = sqrt(pow((v2->coords.x() - v1->coords.x()), 2) + pow((v2->coords.y() - v1->coords.y()), 2) + pow((v2->coords.z() - v1->coords.z()), 2));
    double edge2 = sqrt(pow((v2->coords.x() - v3->coords.x()), 2) + pow((v2->coords.y() - v3->coords.y()), 2) + pow((v2->coords.z() - v3->coords.z()), 2));
    double edge3 = sqrt(pow((v3->coords.x() - v1->coords.x()), 2) + pow((v3->coords.y() - v1->coords.y()), 2) + pow((v3->coords.z() - v1->coords.z()), 2));
    double p = (edge1 + edge2 + edge3) / 2.0;
    area = sqrt(p * (p - edge1) * (p - edge2) * (p - edge3));

    if (QVector3D::dotProduct(n, oldNormal) < 0) { 
        Vertex *tmp = v1;
        v1 = v2;
        v2 = tmp;
        n = QVector3D(QVector3D::crossProduct((v2->coords - v1->coords), (v2->coords - v3->coords))).normalized();
    }
}

bool Face::isDegenerate() {
    recalculate();
    double edge1 = sqrt(pow((v2->coords.x() - v1->coords.x()), 2) + pow((v2->coords.y() - v1->coords.y()), 2) + pow((v2->coords.z() - v1->coords.z()), 2));
    double edge2 = sqrt(pow((v2->coords.x() - v3->coords.x()), 2) + pow((v2->coords.y() - v3->coords.y()), 2) + pow((v2->coords.z() - v3->coords.z()), 2));
    double edge3 = sqrt(pow((v3->coords.x() - v1->coords.x()), 2) + pow((v3->coords.y() - v1->coords.y()), 2) + pow((v3->coords.z() - v1->coords.z()), 2));

    bool cond1 = !(edge1 + edge2 > edge3 && edge1 + edge2 > edge2 && edge3 + edge2 > edge1);
    bool cond2 = !((v1 != v2) && (v2 != v3) && (v1 != v3));

    return cond1 && cond2;
}

void Face::changeEdges() {
    if (v1->edge->face == this) {
        v1->edge = v1->edge->twin->nextEdge;
    }
    if (v2->edge->face == this) {
        v2->edge = v2->edge->twin->nextEdge;
    }
    if (v3->edge->face == this) {
        v3->edge = v3->edge->twin->nextEdge;
    }
}

bool Face::reorientCoherently() {
    Face *f1 = edge->twin->face;
    Face *f2 = edge->nextEdge->twin->face;

    double dot1 = QVector3D::dotProduct(n, f1->n);
    double dot2 = QVector3D::dotProduct(n, f2->n);

    Vertex *vv1, *vv2;
    vv1 = edge->next;
    vv2 = edge->twin->next;
    bool b1 = vv1 == f1->v1 && vv2 == f1->v2;
    bool b2 = vv1 == f1->v3 && vv2 == f1->v1;
    bool b3 = vv1 == f1->v2 && vv2 == f1->v3;

    Vertex *vvv1, *vvv2;
    vvv1 = edge->nextEdge->next;
    vvv2 = edge->nextEdge->twin->next;

    bool bb1 = vvv1 == f2->v1 && vvv2 == f2->v2;
    bool bb2 = vvv1 == f2->v3 && vvv2 == f2->v1;
    bool bb3 = vvv1 == f2->v2 && vvv2 == f2->v3;

    bool fi = (b1 || b2 || b3) && (bb1 || bb2 || bb3);
    return (dot1 < 0 && dot2 < 0) || !fi;
}