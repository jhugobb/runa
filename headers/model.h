#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include "vertex.h"
#include "face.h"
#include "halfedge.h"

/**
 * @brief The Model class
 *
 * Loads all data from a Wavefront .obj file
 * IMPORTANT! Current only supports TRIANGLE meshes!
 *
 * Support for other meshes can be implemented by students
 *
 */
class Model
{
public:
    Model(QString filename);

    QVector<Vertex *> getVertices();
    QVector<Face *> getFaces();
    QMap<QPair<Vertex *, Vertex *>, HalfEdge *> getEdges();
    QVector<Face *> getCopy(QVector<Face *> result);

    bool hasNormals();
    bool hasTextureCoords();
    int getNumTriangles();

    float getXLength();
    float getYLength() ;
    float getZLength() ;
    float getMaxLength() ;
    double unitize();

private:

    // OBJ parsing
    void parseVertex(QStringList tokens);
    void parseNormal(QStringList tokens);
    void parseFace(QStringList tokens);

    void addNormals();


    QVector<Vertex*> vertices;
    QVector<QVector3D> normals;
    QVector<Face*> faces;
    QMap<QPair<Vertex *, Vertex *>, HalfEdge *> halfedges;

    // Utility storage
    int numNormals;

    bool hNorms;
    bool hTexs;
};

#endif // MODEL_H
