
#include "headers/model.h"

#include <QCoreApplication>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{

    if (argc < 2 || argc > 3)
    {
        cerr << "Usage: " << argv[0] << "in-file numFaces \n";
        return 1;
    }

    Model m = new Model(argv[1]);



}
