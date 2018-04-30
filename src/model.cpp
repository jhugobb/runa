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

            if (tokens.at(0) == "vt" ) {
                parseTexture(tokens);
            }

            if (tokens.at(0) == "f" ) {
                parseFace(tokens);
            }
        }

        file.close();

        // create an array version of the data
        unpackIndexes();

        // Allign all vertex indices with the right normal/texturecoord indices
        alignData();

        // Create Vertex Objects
        createVertexes();
        if(file.open(QIODevice::ReadOnly)) {
            int count = 0;
            QTextStream in(&file);

            QString line;
            QStringList tokens = QStringList();

            while(!in.atEnd()) {
                line = in.readLine();
                if (line.startsWith("#")) continue; // skip comments

                tokens = line.split(" ", QString::SkipEmptyParts);
                if (tokens.at(0) == "f" ) {
                    count++;
                    createFace(tokens);
                }
            }

            file.close();
            cout << "Nbr or faces: " << count << endl;
            QVector<Vertex*> tmp;
            for (Vertex* v : verticesObj) {
                if (v->adjacent_faces->size() != 0)
                    tmp.append(v);
            }
            verticesObj = tmp;
        }
    }
}

/**
 * @brief getXLength Calculates the length of the element (assuming the vertexes composes an element) from the X axis perspective.
 * @param v Vertexes
 * @param N Number of vertexes
 * @return X Length
 */
float Model::getXLength () {
    float min = std::numeric_limits<float>::max(), max = - std::numeric_limits<float>::max();
    for(int i = 0 ; i < vertices.size() ; i++) {
        if (vertices.at(i).x() < min) {
            min = vertices.at(i).x();
        }
        else if (vertices.at(i).x() > max) {
            max = vertices.at(i).x();
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
float Model::getYLength () {
    float min = std::numeric_limits<float>::max(), max = - std::numeric_limits<float>::max();
    for(int i = 0 ; i < vertices.size() ; i++) {
        if (vertices.at(i).y() < min) {
            min = vertices.at(i).y();
        }
        else if (vertices.at(i).y() > max) {
            max = vertices.at(i).y();
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
float Model::getZLength () {
    float min = std::numeric_limits<float>::max(), max = - std::numeric_limits<float>::max();
    for(int i = 0 ; i < vertices.size() ; i++) {
        if (vertices.at(i).z() < min) {
            min = vertices.at(i).z();
        }
        else if (vertices.at(i).z() > max) {
            max = vertices.at(i).z();
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
float Model::getMaxLength () {
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
    QVector<QVector3D> tmp = QVector<QVector3D>();

    for (int i = 0 ; i < vertices.size() ; i++) {
        tmp.append(m * vertices.at(i));
    }

    vertices = tmp;
}

QVector<QVector3D> Model::getVertices() {
    return vertices;
}

QVector<QVector3D> Model::getNormals() {
    return normals;
}

QVector<QVector2D> Model::getTextureCoords() {
    return textureCoords;
}

QVector<QVector3D> Model::getVertices_indexed() {
    return vertices_indexed;
}

QVector<QVector3D> Model::getNormals_indexed() {
    return normals_indexed;
}

QVector<QVector2D> Model::getTextureCoords_indexed() {
    return textureCoords_indexed;
}

QVector<unsigned>  Model::getIndices() {
    return indices;
}

QVector<Vertex*> Model::getVertexObjs() {
    return verticesObj;
}

QVector<float> Model::getVNInterleaved() {
    QVector<float> buffer;

    for (int i = 0; i != vertices.size(); ++i) {
        QVector3D vertex = vertices.at(i);
        QVector3D normal = normals.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
    }

    return buffer;
}

QVector<float> Model::getVNTInterleaved() {
    QVector<float> buffer;

    for (int i = 0; i != vertices.size(); ++i) {
        QVector3D vertex = vertices.at(i);
        QVector3D normal = normals.at(i);
        QVector2D uv = textureCoords.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
        buffer.append(uv.x());
        buffer.append(uv.y());
    }

    return buffer;
}

QVector<float> Model::getVNInterleaved_indexed() {
    QVector<float> buffer;

    for (int i = 0; i != vertices_indexed.size(); ++i) {
        QVector3D vertex = vertices_indexed.at(i);
        QVector3D normal = normals_indexed.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
    }

    return buffer;
}

QVector<float> Model::getVNTInterleaved_indexed() {
    QVector<float> buffer;

    for (int i = 0; i != vertices_indexed.size(); ++i) {
        QVector3D vertex = vertices_indexed.at(i);
        QVector3D normal = normals_indexed.at(i);
        QVector2D uv = textureCoords_indexed.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
        buffer.append(uv.x());
        buffer.append(uv.y());
    }

    return buffer;
}



/**
 * @brief Model::getNumTriangles
 *
 * Gets the number of triangles in the model
 *
 * @return number of triangles
 */
int Model::getNumTriangles() {
    return vertices.size()/3;
}

void Model::parseVertex(QStringList tokens) {
    float x,y,z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    vertices_indexed.append(QVector3D(x,y,z));
}

void Model::parseNormal(QStringList tokens) {
    hNorms = true;
    float x,y,z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    norm.append(QVector3D(x,y,z));
}

void Model::parseTexture(QStringList tokens) {
    hTexs = true;
    float u,v;
    u = tokens[1].toFloat();
    v = tokens[2].toFloat();
    tex.append(QVector2D(u,v));
}

void Model::parseFace(QStringList tokens) {
    QStringList elements;

    for( int i = 1; i != tokens.size(); ++i ) {
        elements = tokens[i].split("/");
        // -1 since .obj count from 1
        indices.append(elements[0].toInt()-1);
        
        if (elements.size() > 1 && !elements[1].isEmpty() ) {
            texcoord_indices.append(elements[1].toInt()-1);
        }

        if (elements.size() > 2 && !elements[2].isEmpty() ) {
            normal_indices.append(elements[2].toInt()-1);
        }
    }
}

void Model::createFace(QStringList tokens) {
    QStringList elements;

    QList<int> vertexes = QList<int>();
    QList<int> nidx = QList<int>();
    for( int i = 1; i != tokens.size(); ++i ) {
        elements = tokens[i].split("/");
        // -1 since .obj count from 1
        vertexes.append(elements[0].toInt()-1);
    }

    Face* f = new Face(verticesObj.at(vertexes[0]), verticesObj.at(vertexes[1]), verticesObj.at(vertexes[2]));
    faces.append(f);
}


/**
 * @brief Model::alignData
 *
 * Make sure that the indices from the vertices align with those
 * of the normals and the texture coordinates, create extra vertices
 * if vertex has multiple normals or texturecoords
 */
void Model::alignData() {
    QVector<QVector3D> verts = QVector<QVector3D>();
    verts.reserve(vertices_indexed.size());
    QVector<QVector3D> norms = QVector<QVector3D>();
    norms.reserve(vertices_indexed.size());
    QVector<QVector2D> texcs = QVector<QVector2D>();
    texcs.reserve(vertices_indexed.size());
    QVector<Vertice> vs = QVector<Vertice>();

    QVector<unsigned> ind = QVector<unsigned>();
    ind.reserve(indices.size());

    unsigned currentIndex = 0;

    for (int i = 0; i != indices.size(); ++i) {
        QVector3D v = vertices_indexed[indices[i]];

        QVector3D n = QVector2D(0,0);
        if ( hNorms ) {
            n = norm[normal_indices[i]];
        }

        QVector2D t = QVector2D(0,0);
        if ( hTexs ) {
            t = tex[texcoord_indices[i]];
        }

        Vertice k = Vertice(v,n,t);
        if (vs.contains(k)) {
            // Vertex already exists, use that index
            ind.append(vs.indexOf(k));
        } else {
            // Create a new vertex
            verts.append(v);
            norms.append(n);
            texcs.append(t);
            vs.append(k);
            ind.append(currentIndex);
            ++currentIndex;
        }
    }
    // Remove old data
    vertices_indexed.clear();
    normals_indexed.clear();
    textureCoords_indexed.clear();
    indices.clear();

    // Set the new data
    vertices_indexed = verts;
    normals_indexed = norms;
    textureCoords_indexed = texcs;
    indices = ind;
}

/**
 * @brief Model::unpackIndexes
 *
 * Unpack indices so that they are available for glDrawArrays()
 *
 */
void Model::unpackIndexes() {
    vertices.clear();
    normals.clear();
    textureCoords.clear();
    for ( int i = 0; i != indices.size(); ++i ) {
        vertices.append(vertices_indexed[indices[i]]);

        if ( hNorms ) {
            normals.append(norm[normal_indices[i]]);
        }

        if ( hTexs ) {
            textureCoords.append(tex[texcoord_indices[i]]);
        }
    }
}

void Model::createVertexes() {
    for (int i = 0; i < vertices.size(); i++) {
        Vertex* v = new Vertex(vertices.at(i), normals.at(i));
        verticesObj.append(v);
    }
}