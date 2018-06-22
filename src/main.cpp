#include <QCoreApplication>
#include <ctime>
#include <eigen3/Eigen/Dense>
#include <iostream>
#include <map>
#include <string>

#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/Simple_cartesian.h>
#include <list>

#include "headers/face.h"
#include "headers/filecreator.h"
#include "headers/halfedge.h"
#include "headers/model.h"
#include "headers/vertex.h"

struct vertex_greater_than {
    bool operator()(Vertex *const &a, Vertex *const &b) const {
        return a->cost - b->cost;
    }
};

typedef CGAL::Simple_cartesian<double> K;
// the custom triangles are stored into a vector
typedef std::vector<Face *>::const_iterator Iterator;

// The following primitive provides the conversion facilities between
// the custom triangle and point types and the CGAL ones
struct Face_primitive {
   public:
    // this is the type of data that the queries returns. For this example
    // we imagine that, for some reasons, we do not want to store the iterators
    // of the vector, but raw pointers. This is to show that the Id type
    // does not have to be the same as the one of the input parameter of the
    // constructor.
    typedef const Face *Id;
    // CGAL types returned
    typedef K::Point_3 Point;     // CGAL 3D point type
    typedef K::Triangle_3 Datum;  // CGAL 3D triangle type
   private:
    Id m_pt;  // this is what the AABB tree stores internally
   public:
    Face_primitive() {}  // default constructor needed
    // the following constructor is the one that receives the iterators from the
    // iterator range given as input to the AABB_tree
    Face_primitive(Iterator it)
        : m_pt((*it)) {}
    const Id &id() const { return m_pt; }
    // utility function to convert a custom
    // point type to CGAL point type.
    Point convert(const Vertex *v) const {
        return Point(v->coords.x(), v->coords.y(), v->coords.z());
    }
    // on the fly conversion from the internal data to the CGAL types
    Datum datum() const {
        return Datum(convert(m_pt->v1),
                     convert(m_pt->v2),
                     convert(m_pt->v3));
    }
    // returns a reference point which must be on the primitive
    Point reference_point() const {
        return convert(m_pt->v1);
    }
};

typedef CGAL::AABB_traits<K, Face_primitive> My_AABB_traits;
typedef CGAL::AABB_tree<My_AABB_traits> Tree;

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 5 || argc > 6) {
        cerr << "Usage: " << argv[0] << " in-file NumFaces Tolerance Error\n";
        return 1;
    }
    QString nfaces = QString(argv[2]);
    int numfaces = nfaces.toInt();

    QString tol = QString(argv[3]);
    double tolerance = tol.toDouble();

    QString err = QString(argv[4]);
    double error = err.toDouble();

    clock_t t;
    double totalTime = 0, totalTimeEdge = 0, totalTimeReorder = 0, totalTimeCalculation = 0, totalTimeError = 0;
    t = clock();
    Model m = Model(argv[1]);
    //m.unitize();
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC;

    cout << "Finished loading in " << time_taken << " seconds." << endl;
    QVector<Vertex *> vertexes = m.getVertices();
    QMap<double, Vertex *> vertex_heap;
    QVector<Face *> faces = m.getFaces();
    QVector<Face *> copy;
    QVector<Face *> copyFaces = m.getCopy(copy);
    vector<Face *> cgalfaces;
    vector<Vertex *> queue;
    for (Face *f : copyFaces) {
        cgalfaces.push_back(f);
    }

    t = clock();

    multimap<double, Vertex *> map;
    for (Vertex *v : vertexes) {
        v->calculateCost(tolerance);
        map.insert(pair<double, Vertex *>(v->cost, v));
    }

    Tree tree(cgalfaces.begin(), cgalfaces.end());

    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;

    bool refined = false;
    unsigned iterations = 1;
    if (numfaces != 0) {
        while (faces.size() > numfaces || !refined) {
            if (map.empty()) {
                break;
            }
            iterations++;

            t = clock();

            Vertex *optimalVertex = (*map.begin()).second;
            map.erase(map.begin());
            Vertex *vi = optimalVertex->getOptimalEdge();

            t = clock() - t;
            time_taken = ((double)t) / CLOCKS_PER_SEC;
            totalTimeCalculation += time_taken;

            if (vi == NULL) continue;
            // Calculation of Optimal Point
            t = clock();
            QVector<Face *> *facesVisited = new QVector<Face *>;
            QPair<Eigen::Matrix3d, Eigen::Vector3d> linear;
            linear = optimalVertex->getLinearPair(linear, facesVisited);
            QPair<Eigen::Matrix3d, Eigen::Vector3d> linearI;
            linearI = vi->getLinearPair(linearI, facesVisited);

            Eigen::Vector3d optimalPoint;
            Eigen::Matrix3d A = linear.first + linearI.first;
            if (abs(A.determinant()) < 0.0001) {
                optimalPoint = Eigen::Vector3d(vi->coords.x(), vi->coords.y(), vi->coords.z());
            } else {
                Eigen::Vector3d b = linear.second + linearI.second;
                optimalPoint = A.fullPivLu().solve(b);
            }
            t = clock() - t;
            time_taken = ((double)t) / CLOCKS_PER_SEC;
            totalTimeEdge += time_taken;

            QVector<Face *> facesToBeRemoved;
            QVector3D optimalCoords = QVector3D(optimalPoint[0], optimalPoint[1], optimalPoint[2]);
            // Edge collapse
            t = clock();

            QVector<Vertex *> changed;
            changed = optimalVertex->getChanged();
            changed = vi->getChanged();
            changed.removeAll(optimalVertex);

            for (Vertex *v : changed) {
                map.erase(v->cost);
            }
            t = clock() - t;
            time_taken = ((double)t) / CLOCKS_PER_SEC;
            totalTimeReorder += time_taken;

            t = clock();

            HalfEdge *edge = optimalVertex->getEdge(vi);
            facesToBeRemoved = optimalVertex->replaceWith(optimalCoords, facesToBeRemoved, vi, edge);

            t = clock() - t;
            time_taken = ((double)t) / CLOCKS_PER_SEC;
            totalTimeReorder += time_taken;

            t = clock();

            for (Face *f : facesToBeRemoved) {
                f->changeEdges();
                faces.removeAll(f);
            }
            vertexes.removeAll(optimalVertex);

            t = clock() - t;
            time_taken = ((double)t) / CLOCKS_PER_SEC;
            totalTimeReorder += time_taken;

            t = clock();

            changed = vi->recalculateCost(tolerance, changed);

            t = clock() - t;
            time_taken = ((double)t) / CLOCKS_PER_SEC;
            totalTimeReorder += time_taken;

            t = clock();
            for (Vertex *v : changed) {
                map.insert(pair<double, Vertex *>(v->cost, v));
            }

            t = clock() - t;
            time_taken = ((double)t) / CLOCKS_PER_SEC;
            totalTimeReorder += time_taken;

            t = clock();

            double cal = sqrt(tree.squared_distance(K::Point_3(vi->coords.x(), vi->coords.y(), vi->coords.z())));
            if (cal >= error)
                refined = true;

            t = clock() - t;
            time_taken = ((double)t) / CLOCKS_PER_SEC;
            totalTimeError += time_taken;
        }

        double sqSum = 0;
        double max = -std::numeric_limits<double>::max();
        double sum = 0, size = vertexes.size(), avg;
        double rms;
        for (Vertex *v : vertexes) {
            double cal = sqrt(tree.squared_distance(K::Point_3(v->coords.x(), v->coords.y(), v->coords.z())));
            sum += cal;
            sqSum += pow(cal, 2);
            if (cal > max)
                max = cal;
        }
        avg = sum / size;
        rms = sqrt(sqSum / size);
        cout << "Max distance: " << max << endl;
        cout << "Average distance: " << avg << endl;
        cout << "RMS: " << rms << endl;

        double max2 = -std::numeric_limits<double>::max();

        for (Vertex *v : vertexes) {
            double cal = sqrt(tree.squared_distance(K::Point_3(v->coords.x(), v->coords.y(), v->coords.z())));
            if (cal != max && cal > max2) {
                max2 = cal;
            }
        }
        cout << "Max second distance: " << max2 << endl;
    }
    
    QString filename = QString(argv[1]);
    filename.replace(QString(".obj"), QString("-opt.obj"));
    FileCreator fileCreator = FileCreator(vertexes, faces, filename);
    fileCreator.createFile();

    totalTime += totalTimeCalculation + totalTimeReorder + totalTimeEdge + totalTimeError;

    cout << "Current number of Faces: " << faces.size() << endl;
    cout << "Current number of Vertices: " << vertexes.size() << endl;
    cout << "Current number of Vertices in the heap: " << map.size() << endl;
    cout << "The algorithm's total time is: " << totalTime << " seconds." << endl;
    cout << "Vertex cost calculation took " << totalTimeEdge << " seconds." << endl;
    cout << "Edge cost calculation took " << totalTimeCalculation << " seconds." << endl;
    cout << "Collapse and reorder took " << totalTimeReorder << " seconds." << endl;
    cout << "Error calculation took " << totalTimeError << " seconds." << endl;
}
