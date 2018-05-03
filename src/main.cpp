#include <QCoreApplication>
#include <QMap>
#include <iostream>
#include <string>
#include <eigen3/Eigen/Dense>

#include "headers/model.h"
#include "headers/vertex.h"
#include "headers/face.h"
#include "headers/filecreator.h"

using namespace std;

int main(int argc, char *argv[])
{

    if (argc < 3 || argc > 4)
    {
        cerr << "Usage: " << argv[0] << " in-file numFaces \n";
        return 1;
    }
    QString nfaces = QString(argv[2]);
    int numfaces = nfaces.toInt();
    Model m = Model(argv[1]);

    cout << "Finished loading." << endl; 
    QVector<Vertex *> vertexes = m.getVertices();
    QMap<double, Vertex *> vertex_heap;
    QVector<Face *> faces = m.getFaces();
    cout << "Finished creating Vertexes and Faces." << endl; 

    cout << "Starting the vertex cost calculation..." << endl;
    int k = 0;
    int count = 0;
    for (Vertex *v : vertexes) {
        k++;
        count = v->calculateCost(count);
        vertex_heap.insert(v->cost, v);
    }
    cout << "Calculation finished." << endl;

    cout << "N of Vertexes: " << k << ", and of those, " << count << " have no pairs" << endl;
    cout << "N of Faces: " << m.getNumTriangles() << ", and we want " << nfaces.toInt() << endl;

    while (faces.size() > numfaces) {
        cout << "The lowest cost is " << vertex_heap.firstKey() << endl;
        cout << "The highest cost is " << vertex_heap.lastKey() << endl;

        cout << "Starting the edge cost calculation..." << endl;

        Vertex *optimalVertex = vertex_heap.take(vertex_heap.firstKey());
        cout << "Number of adjacent faces: " << optimalVertex->adjacent_faces.size() << endl; 
        Vertex *vi = optimalVertex->getOptimalEdge();

        cout << "Calculation Finished." << endl;

        cout << "The optimal Vertex is p("
             << optimalVertex->coords.x()
             << ", " << optimalVertex->coords.y()
             << ", " << optimalVertex->coords.z()
             << ")." << endl;

        cout << "The optimal edge is from p to e("
             << vi->coords.x()
             << ", " << vi->coords.y()
             << ", " << vi->coords.z()
             << ")." << endl;

        cout << "Starting optimal point calculation..." << endl;

        QPair<Eigen::Matrix3d, Eigen::Vector3d> linear;
        optimalVertex->getLinearPair(linear);
        QPair<Eigen::Matrix3d, Eigen::Vector3d> linearI;
        vi->getLinearPair(linearI);

        Eigen::Vector3d optimalPoint;
        Eigen::Matrix3d A = linear.first + linearI.first;
        if (abs(A.determinant()) < 0.001)
        {
            optimalPoint = Eigen::Vector3d(optimalVertex->coords.x(), optimalVertex->coords.y(), optimalVertex->coords.z());
        }
        else
        {
            Eigen::Vector3d b = linear.second + linearI.second;
            optimalPoint = A.colPivHouseholderQr().solve(b);
        }
        cout << "Optimal point calculated." << endl;

        cout << "The new point is np("
             << optimalPoint
             << ")." << endl;

        QVector<Face *> facesToBeRemoved;
        QVector3D optimalCoords = QVector3D(optimalPoint[0], optimalPoint[1], optimalPoint[2]);
         //TODO: Normal?

        facesToBeRemoved =  vi->replaceWith(optimalCoords, facesToBeRemoved, optimalVertex);
        for (Face *f : facesToBeRemoved) {
            faces.removeAll(f);
        }
        vertexes.removeAll(optimalVertex);

        vi->recalculateCost(count);
        vertex_heap.clear();
        for (Vertex *v : vertexes) {
            vertex_heap.insert(v->cost, v);
        }

        cout << "Current number of Faces: " << faces.size() << endl;
        cout << "Current number of Vertices: " << vertexes.size() << endl;
    }
    
    QString filename = QString(argv[1]);
    filename.replace(QString(".obj"), QString("-opt.obj"));
    FileCreator fileCreator = FileCreator(vertexes, faces, filename);
    fileCreator.createFile();

}
