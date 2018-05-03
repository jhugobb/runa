#include "headers/model.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMatrix4x4>

#include <iostream>

using namespace std;

// A Private Vertex class for vertex comparison
// DO NOT include "vertex.h" or something similar in this file
struct Vertice {

    QVector3D coord;
    QVector3D normal;
    QVector2D texCoord;

    Vertice() : coord(), normal(), texCoord(){}
    Vertice(QVector3D coords, QVector3D normal, QVector3D texc): coord(coords), normal(normal), texCoord(texc){}

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
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);

        QString line;
        QStringList tokens = QStringList();

        while(!in.atEnd()) {
            line = in.readLine();
            if (line.startsWith("#")) continue; // skip comments

            tokens = line.split(" ", QString::SkipEmptyParts);

            // Notify Jiri about this bug
            if (tokens.isEmpty()) continue;
            // Switch depending on first element
            if (tokens.at(0) == "v") {
                parseVertex(tokens);
            }

            if (tokens.at(0) == "vn" ) {
                parseNormal(tokens);
            }

            if (tokens.at(0) == "f" ) {
                parseFace(tokens);
            }
        }

        file.close();
        
    }

    //addNormals();
}

QVector<Vertex *> Model::getVertices()
{
    return vertices;
}

QVector<Face *> Model::getFaces() {
    return faces;
}

/**
 * @brief Model::getNumTriangles
 *
 * Gets the number of triangles in the model
 *
 * @return number of triangles
 */
int Model::getNumTriangles()
{
    return faces.size();
}

void Model::parseVertex(QStringList tokens)
{
    float x, y, z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    Vertex* v = new Vertex(QVector3D(x, y, z));
    vertices.append(v);
}

void Model::parseNormal(QStringList tokens)
{
    hNorms = true;
    float x, y, z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    normals.append(QVector3D(x, y, z));
    numNormals++;
}

void Model::parseFace(QStringList tokens)
{
    QStringList elements;
    QVector<Vertex*> verts;
    for (int i = 1; i != tokens.size(); ++i)
    {
        elements = tokens[i].split("/");
        // -1 since .obj count from 1
        verts.append(vertices.at(elements[0].toInt() - 1));
        // TODO: Change to incorporate multiple normal vertices
        if (elements.size() > 2 && !elements[2].isEmpty() ) {
            Vertex* v = vertices.at(elements[0].toInt() - 1);
            v->addNormal(normals.at(elements[2].toInt() - 1));
            vertices.replace(elements[0].toInt() - 1, v);
        }
    }

    Face *f = new Face(verts.at(0), verts.at(1), verts.at(2));
    faces.append(f);
}

// TODO: Implement unitize
// /**
//  * @brief getXLength Calculates the length of the element (assuming the vertexes composes an element) from the X axis perspective.
//  * @param v Vertexes
//  * @param N Number of vertexes
//  * @return X Length
//  */
// float Model::getXLength () {
//     float min = std::numeric_limits<float>::max(), max = - std::numeric_limits<float>::max();
//     for(int i = 0 ; i < vertices.size() ; i++) {
//         if (vertices.at(i).x() < min) {
//             min = vertices.at(i).x();
//         }
//         else if (vertices.at(i).x() > max) {
//             max = vertices.at(i).x();
//         }

//     }
//     return (max - min);
// }

// /**
//  * @brief getYLength Calculates the length of the element (assuming the vertexes composes an element) from the Z axis perspective.
//  * @param v Vertexes
//  * @param N Number of vertexes
//  * @return Y Length
//  */
// float Model::getYLength () {
//     float min = std::numeric_limits<float>::max(), max = - std::numeric_limits<float>::max();
//     for(int i = 0 ; i < vertices.size() ; i++) {
//         if (vertices.at(i).y() < min) {
//             min = vertices.at(i).y();
//         }
//         else if (vertices.at(i).y() > max) {
//             max = vertices.at(i).y();
//         }

//     }
//     return (max - min);
// }

// /**
//  * @brief getZLength Calculates the length of the element (assuming the vertexes composes an element) from the Z axis perspective.
//  * @param v Vertexes
//  * @param N Number of vertexes
//  * @return Z Length
//  */
// float Model::getZLength () {
//     float min = std::numeric_limits<float>::max(), max = - std::numeric_limits<float>::max();
//     for(int i = 0 ; i < vertices.size() ; i++) {
//         if (vertices.at(i).z() < min) {
//             min = vertices.at(i)->.z();
//         }
//         else if (vertices.at(i).z() > max) {
//             max = vertices.at(i).z();
//         }

//     }
//     return (max - min);
// }

// /**
//  * @brief getMaxLength Returns the greatest length from the 3 axis perspective of a model.
//  * @param v Model.
//  * @param N Number of vertexes.
//  * @return Greatest length.
//  */
// float Model::getMaxLength () {
//     float max = getXLength(), tmp;

//     if ((tmp = getYLength()) > max) {
//         max = tmp;
//     }

//     if ((tmp = getZLength()) > max) {
//         max = tmp;
//     }

//     return max;
// }


// /**
//  * @brief Model::unitze Not Implemented yet!
//  *
//  * Unitize the model by scaling so that it fits a box with sides 1
//  * and origin at 0,0,0
//  * Usefull for models with different scales
//  *
//  */
// void Model::unitize() {
//     QMatrix4x4 m = QMatrix4x4();
//     m.scale(1.0 / getMaxLength());
//     QVector<QVector3D> tmp = QVector<QVector3D>();

//     for (int i = 0 ; i < vertices.size() ; i++) {
//         tmp.append(m * vertices.at(i));
//     }

//     vertices = tmp;
// }
