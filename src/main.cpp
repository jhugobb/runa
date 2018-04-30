#include <QCoreApplication>
#include <QMap>
#include <iostream>
#include <string>
#include <eigen3/Eigen/Dense>

#include "headers/model.h"
#include "headers/vertex.h"
#include "headers/face.h"

using namespace std;

int main(int argc, char *argv[])
{

    if (argc < 2 || argc > 3)
    {
        cerr << "Usage: " << argv[0] << " in-file numFaces \n";
        return 1;
    }

    Model m = Model(argv[1]);

    cout << "Finished loading." << endl; 
    QVector<Vertex*> vertexes = m.getVertexObjs();
    QMap<double, Vertex*> vertex_heap;
    QVector<Face> faces;
    cout << "Finished creating Vertexes and Faces." << endl; 

    cout << "Starting the vertex cost calculation..." << endl;
    int k = 0;
    int count = 0;
    for (Vertex* v : vertexes) {
        k++;
        count = v->calculateCost(count);
        vertex_heap.insert(v->cost, v);
    }
    cout << "Calculation finished." << endl;

    cout << "N of Vertexes: " << k << ", and of those, " << count << " have no pairs" << endl;

    cout << "The lowest cost is " << vertex_heap.firstKey() << endl;
    cout << "The highest cost is " << vertex_heap.lastKey() << endl;

    cout << "Starting the edge cost calculation..." << endl;

    Vertex* optimalVertex = vertex_heap.first();
    Vertex* vi = optimalVertex->getOptimalEdge();
    
    cout << "Calculation Finished." << endl;

    cout << "The optimal Vertex is p(" 
         << optimalVertex->coords->x() 
         << ", " << optimalVertex->coords->y() 
         << ", " << optimalVertex->coords->z() 
         << ")." << endl;
    
    cout << "The optimal edge is from p to e(" 
         << vi->coords->x() 
         << ", " << vi->coords->y() 
         << ", " << vi->coords->z() 
         << ")." << endl;

    cout << "Starting optimal point calculation..." << endl;

    QPair<Eigen::Matrix3d, Eigen::Vector3d> linear;
    optimalVertex->getLinearPair(linear);
    QPair<Eigen::Matrix3d, Eigen::Vector3d> linearI;
    vi->getLinearPair(linearI);

    cout << optimalVertex->equals(vi) << endl;

    Eigen::Matrix3d A = linear.first + linearI.first;
    Eigen::Vector3d b = linear.second + linearI.second;

    Eigen::Vector3d optimalPoint = A.colPivHouseholderQr().solve(b);

    cout << "Optimal point calculated." << endl;

    cout << "The new point is np("
         << optimalPoint 
         << ")." << endl;

}
