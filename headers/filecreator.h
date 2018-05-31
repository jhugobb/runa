#ifndef FILECREATOR_H
#define FILECREATOR_H

#include <QVector>
#include "face.h"
#include "vertex.h"

class FileCreator {
   public:
    FileCreator(QVector<Vertex *> v, QVector<Face *> f, QString filename);
    void createFile();

   private:
    QString filename;
    QVector<Vertex *> vertices;
    QVector<Face *> faces;
};

#endif  // FILECREATOR_H