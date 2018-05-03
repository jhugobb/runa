#include <QFile>
#include <QTextStream>

#include "headers/filecreator.h"

FileCreator::FileCreator(QVector<Vertex *> v, QVector<Face *> f, QString file) {
    vertices = v;
    faces = f;
    filename = file;
}

void FileCreator::createFile() {
    QFile file(filename);
    file.remove();
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        for (Vertex *v : vertices) {
            QVector3D p = v->coords;
            QVector3D n = v->normal;
            stream << "vn " << n.x() << " " << n.y() << " " << n.z() << endl;
            stream << "v " << p.x() << " " << p.y() << " " << p.z() << endl;
        }

        for (Face *f : faces) {
            stream << "f " << vertices.indexOf(f->v1)+1 << "//" << vertices.indexOf(f->v1)+1 << " ";
            stream << vertices.indexOf(f->v2)+1 << "//" << vertices.indexOf(f->v2)+1 << " ";
            stream << vertices.indexOf(f->v3)+1 << "//" << vertices.indexOf(f->v3)+1 << endl;
        }
    }
}