#include <QCoreApplication>
#include <QMap>
#include <iostream>
#include <string>

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

    cout << "Finished loading" << endl; 

    QVector<Vertex*> vertexes = m.getVertexObjs();
    QMap<double, Vertex> vertex_heap;
    QVector<Face> faces;
    

    cout << "Finished creating Vertexes and Faces" << endl; 
    int k = 0;
    int count = 0;
    for (Vertex* v : vertexes) {
        k++;
        count = v->calculateCost(count);
        vertex_heap.insert(v->cost, *v);
    }

    cout << "N of Vertexes: " << k << ", and of those, " << count << " have no pairs" << endl;

    cout << "The lowest cost is " << vertex_heap.firstKey() << endl;
    cout << "The highest cost is " << vertex_heap.lastKey() << endl;

}
