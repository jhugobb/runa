#include <QCoreApplication>
#include <QMap>
#include <iostream>
#include <string>
#include <eigen3/Eigen/Dense>
#include <ctime>

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
    clock_t t;
    double totalTime = 0, totalTimeEdge = 0, totalTimeReorder = 0, totalTimeCalculation = 0;
    t = clock();
    Model m = Model(argv[1]);
    m.unitize();
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC;

    cout << "Finished loading in " << time_taken << " seconds." << endl; 
    QVector<Vertex *> vertexes = m.getVertices();
    QMap<double, Vertex *> vertex_heap;
    QVector<Face *> faces = m.getFaces();
    cout << "Finished creating Vertexes and Faces." << endl; 

    cout << "Starting the vertex cost calculation..." << endl;
    int k = 0;
    int count = 0;
    t = clock();
    for (Vertex *v : vertexes) {
        k++;
        count = v->calculateCost(count);
        if (v->twin_faces.size() != 0)
            vertex_heap.insert(v->cost, v);
    }
    cout << "Calculation finished in " << time_taken << " seconds." << endl;
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;

    cout << "N of Vertexes: " << k << ", and of those, " << count << " have no pairs" << endl;
    cout << "N of Faces: " << m.getNumTriangles() << ", and we want " << nfaces.toInt() << endl;

    while (faces.size() > numfaces) {
        cout << "The lowest cost is " << vertex_heap.firstKey() << endl;
        cout << "The highest cost is " << vertex_heap.lastKey() << endl;

        cout << "Starting the edge cost calculation..." << endl;
        t = clock();
        Vertex *optimalVertex = vertex_heap.take(vertex_heap.firstKey());
        cout << "Number of adjacent faces: " << optimalVertex->adjacent_faces.size() << endl; 
        Vertex *vi = optimalVertex->getOptimalEdge();
        t = clock() - t;
        time_taken = ((double)t) / CLOCKS_PER_SEC;
        totalTimeCalculation += time_taken;
        cout << "Calculation Finished in " << time_taken << " seconds." << endl;

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
        // Calculation of Optimal Point 
        t = clock();        
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
        t = clock() - t;
        time_taken = ((double)t) / CLOCKS_PER_SEC;
        totalTimeEdge += time_taken;

        cout << "Optimal point calculated in " << time_taken << " seconds." << endl;

        cout << "The new point is np("
             << optimalPoint
             << ")." << endl;

        QVector<Face *> facesToBeRemoved;
        QVector3D optimalCoords = QVector3D(optimalPoint[0], optimalPoint[1], optimalPoint[2]);
        assert(optimalCoords == optimalVertex->coords);
         //TODO: Normal?
        // Edge collapse 
        t = clock();
        facesToBeRemoved =  vi->replaceWith(optimalCoords, facesToBeRemoved, optimalVertex);

        for (Face *f : faces) {
            if (f->isDegenerate() && !facesToBeRemoved.contains(f)) {
                facesToBeRemoved.append(f);
            }
        }
        for (Face *f : facesToBeRemoved) {
            faces.removeAll(f);
            f->v1->adjacent_faces.removeAll(f);
            f->v2->adjacent_faces.removeAll(f);
            f->v3->adjacent_faces.removeAll(f);
        }
        vertexes.removeAll(optimalVertex);
        vertex_heap.remove(optimalVertex->cost);

        QVector<Vertex *> changed;
        changed = vi->getChanged(); 
        for (Vertex *v : changed) {
            vertex_heap.remove(v->cost);
            for (Face *f : facesToBeRemoved) {
                v->adjacent_faces.removeAll(f);
            }
        }
        changed = vi->recalculateCost(count, changed);
        for (Vertex *v : changed) {
            if (v->twin_faces.size() > 0) {
                vertex_heap.insert(v->cost, v);
            } else {
                vertexes.removeAll(v);
            }
        }
        t = clock() - t;
        time_taken = ((double)t) / CLOCKS_PER_SEC;
        totalTimeReorder += time_taken;
        cout << "Finished the collapse and reorder in " << time_taken << " seconds." << endl;
        cout << "Current number of Faces: " << faces.size() << endl;
        cout << "Current number of Vertices: " << vertexes.size() << endl;
    }

    QString filename = QString(argv[1]);
    filename.replace(QString(".obj"), QString("-opt.obj"));
    FileCreator fileCreator = FileCreator(vertexes, faces, filename);
    fileCreator.createFile();

    totalTime+=totalTimeCalculation + totalTimeReorder + totalTimeEdge;

    cout << "The algorithm's total time is: " << totalTime << " seconds." << endl;
    cout << "Vertex cost calculation took " << totalTimeEdge << " seconds." << endl;
    cout << "Edge cost calculation took " << totalTimeCalculation << " seconds." << endl;
    cout << "Collapse and reorder took " << totalTimeReorder << " seconds." << endl;
}
