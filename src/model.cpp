#include "headers/model.h"

#include <QDebug>
#include <QFile>
#include <QMatrix4x4>
#include <QTextStream>

#include <iostream>

using namespace std;

// A Private Vertex class for vertex comparison
// DO NOT include "vertex.h" or something similar in this file
struct Vertice {
    QVector3D coord;
    QVector3D normal;
    QVector2D texCoord;

    Vertice() : coord(), normal(), texCoord() {}
    Vertice(QVector3D coords, QVector3D normal, QVector3D texc) : coord(coords), normal(normal), texCoord(texc) {}

    bool operator==(const Vertice &other) const {
        if (other.coord != coord)
            return false;
        if (other.normal != normal)
            return false;
        if (other.texCoord != texCoord)
            return false;
        return true;
    }
};

Model::Model(QString filename) {
    hNorms = false;
    hTexs = false;
    numNormals = 0;

    qDebug() << ":: Loading model:" << filename;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);

        QString line;
        QStringList tokens = QStringList();

        while (!in.atEnd()) {
            line = in.readLine();
            if (line.startsWith("#")) continue;  // skip comments

            tokens = line.split(" ", QString::SkipEmptyParts);

            // Notify Jiri about this bug
            if (tokens.isEmpty()) continue;
            // Switch depending on first element
            if (tokens.at(0) == "v") {
                parseVertex(tokens);
            }

            if (tokens.at(0) == "vn") {
                parseNormal(tokens);
            }

            if (tokens.at(0) == "f") {
                parseFace(tokens);
            }
        }

        file.close();
    }

    //addNormals();
}

QVector<Vertex *> Model::getVertices() {
    return vertices;
}

QVector<Face *> Model::getFaces() {
    return faces;
}
QMap<QPair<Vertex *, Vertex *>, HalfEdge *> Model::getEdges() {
    return halfedges;
}

/**
 * @brief Model::getNumTriangles
 *
 * Gets the number of triangles in the model
 *
 * @return number of triangles
 */
int Model::getNumTriangles() {
    return faces.size();
}

void Model::parseVertex(QStringList tokens) {
    float x, y, z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    Vertex *v = new Vertex(QVector3D(x, y, z));
    vertices.append(v);
}

void Model::parseNormal(QStringList tokens) {
    hNorms = true;
    float x, y, z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    normals.append(QVector3D(x, y, z));
    numNormals++;
}

void Model::parseFace(QStringList tokens) {
    QStringList elements;
    QVector<Vertex *> verts;
    for (int i = 1; i != tokens.size(); ++i) {
        elements = tokens[i].split("/");
        // -1 since .obj count from 1
        if (elements.size() > 0 && !elements[0].isEmpty()) {
            verts.append(vertices.at(elements[0].toInt() - 1));
        }
        // TODO: Change to incorporate multiple normal vertices
        if (elements.size() > 2 && !elements[2].isEmpty()) {
            vertices.at(elements[0].toInt() - 1)->addNormal(normals.at(elements[2].toInt() - 1));
        }
    }
    Face *f = new Face(verts.at(0), verts.at(1), verts.at(2));
    QPair<Vertex *, Vertex *> edge1, edge1twin, edge2, edge2twin, edge3, edge3twin;

    edge1.first = verts.at(0);
    edge1.second = verts.at(1);

    edge1twin.first = verts.at(1);
    edge1twin.second = verts.at(0);

    edge2.first = verts.at(1);
    edge2.second = verts.at(2);

    edge2twin.first = verts.at(2);
    edge2twin.second = verts.at(1);

    edge3.first = verts.at(2);
    edge3.second = verts.at(0);

    edge3twin.first = verts.at(0);
    edge3twin.second = verts.at(2);

    HalfEdge *he1 = new HalfEdge();
    HalfEdge *he2 = new HalfEdge();
    HalfEdge *he3 = new HalfEdge();
    he1->nextEdge = he2;
    he2->nextEdge = he3;
    he3->nextEdge = he1;

    if (!halfedges.contains(edge1)) {
        verts.at(0)->edge = he1;
        he1->next = verts.at(1);
        he1->face = f;
        halfedges.insert(edge1, he1);
    } else {
        he1 = halfedges.value(edge1);
        he1->nextEdge = he2;
        he3->nextEdge = he1;
    }

    if (halfedges.contains(edge1twin)) {
        HalfEdge *he = halfedges.value(edge1twin);
        he->twin = halfedges.value(edge1);
        he->twin->twin = he;
    }

    if (!halfedges.contains(edge2)) {
        verts.at(1)->edge = he2;
        he2->next = verts.at(2);
        he2->face = f;
        halfedges.insert(edge2, he2);
    } else {
        he2 = halfedges.value(edge2);
        he1->nextEdge = he2;
        he2->nextEdge = he3;
    }

    if (halfedges.contains(edge2twin)) {
        HalfEdge *he = halfedges.value(edge2twin);
        he->twin = halfedges.value(edge2);
        he->twin->twin = he;
    }

    if (!halfedges.contains(edge3)) {
        verts.at(2)->edge = he3;
        he3->next = verts.at(0);
        he3->face = f;
        f->edge = he3;
        halfedges.insert(edge3, he3);
    } else {
        he3 = halfedges.value(edge3);
        he2->nextEdge = he3;
        he3->nextEdge = he1;
    }

    if (halfedges.contains(edge3twin)) {
        HalfEdge *he = halfedges.value(edge3twin);
        he->twin = halfedges.value(edge3);
        he->twin->twin = he;
        if (false)
            ;
    }
    faces.append(f);
}

// TODO: Implement unitize
/**
 * @brief getXLength Calculates the length of the element (assuming the vertexes composes an element) from the X axis perspective.
 * @param v Vertexes
 * @param N Number of vertexes
 * @return X Length
 */
float Model::getXLength() {
    float min = std::numeric_limits<float>::max(), max = -std::numeric_limits<float>::max();
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices.at(i)->coords.x() < min) {
            min = vertices.at(i)->coords.x();
        } else if (vertices.at(i)->coords.x() > max) {
            max = vertices.at(i)->coords.x();
        }
    }
    return (max - min);
}

/**
 * @brief getYLength Calculates the length of the element (assuming the vertexes composes an element) from the Z axis perspective.
 * @param v Vertexes
 * @param N Number of vertexes
 * @return Y Length
 */
float Model::getYLength() {
    float min = std::numeric_limits<float>::max(), max = -std::numeric_limits<float>::max();
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices.at(i)->coords.y() < min) {
            min = vertices.at(i)->coords.y();
        } else if (vertices.at(i)->coords.y() > max) {
            max = vertices.at(i)->coords.y();
        }
    }
    return (max - min);
}

/**
 * @brief getZLength Calculates the length of the element (assuming the vertexes composes an element) from the Z axis perspective.
 * @param v Vertexes
 * @param N Number of vertexes
 * @return Z Length
 */
float Model::getZLength() {
    float min = std::numeric_limits<float>::max(), max = -std::numeric_limits<float>::max();
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices.at(i)->coords.z() < min) {
            min = vertices.at(i)->coords.z();
        } else if (vertices.at(i)->coords.z() > max) {
            max = vertices.at(i)->coords.z();
        }
    }
    return (max - min);
}

/**
 * @brief getMaxLength Returns the greatest length from the 3 axis perspective of a model.
 * @param v Model.
 * @param N Number of vertexes.
 * @return Greatest length.
 */
float Model::getMaxLength() {
    float max = getXLength(), tmp;

    if ((tmp = getYLength()) > max) {
        max = tmp;
    }

    if ((tmp = getZLength()) > max) {
        max = tmp;
    }

    return max;
}

/**
 * @brief Model::unitze Not Implemented yet!
 *
 * Unitize the model by scaling so that it fits a box with sides 1
 * and origin at 0,0,0
 * Usefull for models with different scales
 *
 */
void Model::unitize() {
    QMatrix4x4 m = QMatrix4x4();
    m.scale(1.0 / getMaxLength());

    for (int i = 0; i < vertices.size(); i++) {
        vertices.at(i)->coords = m * vertices.at(i)->coords;
    }
}

QVector<Face *> Model::getCopy(QVector<Face *> result) {
    for (Face *f : faces) {
        Face *n = new Face(*f);
        n->v1 = new Vertex(*n->v1);
        n->v2 = new Vertex(*n->v2);
        n->v3 = new Vertex(*n->v3);
        result.append(n);
    }
    return result;
}
