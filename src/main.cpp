#include <QCoreApplication>
#include <iostream>
#include <map>
#include <string>
#include <eigen3/Eigen/Dense>
#include <ctime>

#include <list>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>

#include "headers/model.h"
#include "headers/vertex.h"
#include "headers/face.h"
#include "headers/filecreator.h"
#include "headers/halfedge.h"

struct vertex_greater_than
{
    bool operator()(Vertex *const &a, Vertex *const &b) const
    {
        return a->cost - b->cost;
    }
};

typedef CGAL::Simple_cartesian<double> K;
// the custom triangles are stored into a vector
typedef std::vector<Face>::const_iterator Iterator;

// The following primitive provides the conversion facilities between
// the custom triangle and point types and the CGAL ones
struct Face_primitive
{
  public:
    // this is the type of data that the queries returns. For this example
    // we imagine that, for some reasons, we do not want to store the iterators
    // of the vector, but raw pointers. This is to show that the Id type
    // does not have to be the same as the one of the input parameter of the
    // constructor.
    typedef const Face *Id;
    // CGAL types returned
    typedef K::Point_3 Point;    // CGAL 3D point type
    typedef K::Triangle_3 Datum; // CGAL 3D triangle type
  private:
    Id m_pt; // this is what the AABB tree stores internally
  public:
    Face_primitive() {} // default constructor needed
    // the following constructor is the one that receives the iterators from the
    // iterator range given as input to the AABB_tree
    Face_primitive(Iterator it)
        : m_pt(&(*it)) {}
    const Id &id() const { return m_pt; }
    // utility function to convert a custom
    // point type to CGAL point type.
    Point convert(const Vertex *v) const
    {
        return Point(v->coords.x(), v->coords.y(), v->coords.z());
    }
    // on the fly conversion from the internal data to the CGAL types
    Datum datum() const
    {
        return Datum(convert(m_pt->v1),
                     convert(m_pt->v2),
                     convert(m_pt->v3));
    }
    // returns a reference point which must be on the primitive
    Point reference_point() const
    {
        return convert(m_pt->v1);
    }
};

typedef CGAL::AABB_traits<K, Face_primitive> My_AABB_traits;
typedef CGAL::AABB_tree<My_AABB_traits> Tree;

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
    vector<Face> cgalfaces;
    for (Face *f : faces) {
        cgalfaces.push_back(*f);
    }

    int k = 0;
    int count = 0;
    t = clock();

    multimap<double, Vertex *> map;
    for (Vertex *v : vertexes) {
        count = v->calculateCost(count);
        map.insert(pair<double, Vertex*>(v->cost, v));
        k++;
    }

    Tree tree(cgalfaces.begin(), cgalfaces.end());

    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;

    bool refined = false;
    unsigned iterations = 0;

    while (faces.size() > numfaces) { //&& !refined) {
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
        QPair<Eigen::Matrix3d, Eigen::Vector3d> linear;
        optimalVertex->getLinearPair(linear);
        QPair<Eigen::Matrix3d, Eigen::Vector3d> linearI;
        vi->getLinearPair(linearI);

        Eigen::Vector3d optimalPoint;
        Eigen::Matrix3d A = linear.first + linearI.first;
        if (abs(A.determinant()) < 0.001)
        {
            optimalPoint = Eigen::Vector3d(vi->coords.x(), vi->coords.y(), vi->coords.z());
        }
        else
        {
            Eigen::Vector3d b = linear.second + linearI.second;
            optimalPoint = A.colPivHouseholderQr().solve(b);
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

        // TODO: fix this to not delete vertices for evaluation with same cost 
        for (Vertex *v : changed) {
            map.erase(v->cost);
        }
        t = clock() - t;
        time_taken = ((double)t) / CLOCKS_PER_SEC;
        totalTimeReorder += time_taken;

        t = clock();

        HalfEdge *edge = optimalVertex->getEdge(vi);
        facesToBeRemoved =  optimalVertex->replaceWith(optimalCoords, facesToBeRemoved, vi, edge);

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

        changed = vi->recalculateCost(count, changed);

        t = clock() - t;
        time_taken = ((double)t) / CLOCKS_PER_SEC;
        totalTimeReorder += time_taken;

        t = clock();
        // This is what takes long
        for (Vertex *v : changed) {
            map.insert(pair<double, Vertex *>(v->cost, v));
        }

        t = clock() - t;
        time_taken = ((double)t) / CLOCKS_PER_SEC;
        totalTimeReorder += time_taken;

        // if (iterations >= 100) {
        //     iterations = 0;
        //     for (Vertex *v : vertexes) {
        //         if (tree.squared_distance(K::Point_3(v->coords.x(), v->coords.y(), v->coords.z())) > 180)
        //             refined = true;
        //     }
        // }
    }

    QString filename = QString(argv[1]);
    filename.replace(QString(".obj"), QString("-opt.obj"));
    FileCreator fileCreator = FileCreator(vertexes, faces, filename);
    fileCreator.createFile();

    totalTime+=totalTimeCalculation + totalTimeReorder + totalTimeEdge;

    cout << "Current number of Faces: " << faces.size() << endl;
    cout << "Current number of Vertices: " << vertexes.size() << endl;
    cout << "Current number of Vertices in the heap: " << map.size() << endl;
    cout << "The algorithm's total time is: " << totalTime << " seconds." << endl;
    cout << "Vertex cost calculation took " << totalTimeEdge << " seconds." << endl;
    cout << "Edge cost calculation took " << totalTimeCalculation << " seconds." << endl;
    cout << "Collapse and reorder took " << totalTimeReorder << " seconds." << endl;
    // double max = - std::numeric_limits<double>::max();
    // for (Vertex *v : vertexes) {
    //     double cal = tree.squared_distance(K::Point_3(v->coords.x(), v->coords.y(), v->coords.z()));
    //     cout << tree.squared_distance(K::Point_3(v->coords.x(), v->coords.y(), v->coords.z())) << endl;
    //     if (cal > max)
    //         max = cal;
    // }
    // cout << "Max distance: " << max << endl;
}
