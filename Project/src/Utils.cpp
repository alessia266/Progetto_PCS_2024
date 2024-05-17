#include "DFN.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

namespace DFNLibrary{

bool ImportFract(const string& filename,
                 Fractures& fracture)
{
    ifstream file(filename);

    if(file.fail())
    {
        cerr << "File not found" << endl;
        return false;
    }

    string line;

    //ignored line "Number of Fractures"
    getline(file, line);

    //number of fractures
    getline(file, line);
    istringstream convert(line);
    convert >> fracture.numberFractures;

    for (unsigned int i = 0; i < fracture.numberFractures; i++)
    {
        //ignored line "FractureId; NumVertices"
        getline(file, line);

        //fracture Id, NumVertices
        getline(file, line);
        replace(line.begin(), line.end(), ';', ' ');
        istringstream convert(line);
        unsigned int id;
        unsigned int numVert;
        convert >> id >> numVert;

        //ignored line "Vertices"
        getline(file, line);

        //coordinates
        string coord_x;
        string coord_y;
        string coord_z;

        //x
        getline(file, coord_x);
        replace(coord_x.begin(), coord_x.end(), ';', ' ');
        istringstream convertX(coord_x);

        //y
        getline(file, coord_y);
        replace(coord_y.begin(), coord_y.end(), ';', ' ');
        istringstream convertY(coord_y);

        //z
        getline(file, coord_z);
        replace(coord_z.begin(), coord_z.end(), ';', ' ');
        istringstream convertZ(coord_z);


        double minX = 10e5;
        double maxX = 0;
        double minY = 10e5;
        double maxY = 0;
        double minZ = 10e5;
        double maxZ = 0;

        for (unsigned int j = 0; j < numVert; j++)
        {
            Vector3d coord;
            convertX >> coord[0];
            //assegnazione minimo e massimo della variabile x
            if (minX > coord[0])
                minX = coord[0];
            if (maxX < coord[0])
                maxX = coord[0];

            convertY >> coord[1];
            //assegnazione minimo e massimo della variabile y
            if (minY > coord[1])
                minY = coord[1];
            if (maxY < coord[1])
                maxY = coord[1];

            convertZ >> coord[2];
            //assegnazione minimo e massimo della variabile z
            if (minZ > coord[2])
                minZ = coord[2];
            if (maxZ < coord[2])
                maxZ = coord[2];

            fracture.IDFracture[i].push_back(coord);

        }
        fracture.MinFract[i] = {minX, minY, minZ};
        fracture.MaxFract[i] = {maxX, maxY, maxZ};
    }
    return true;
}

bool FilterFract(Fractures& fracture)
{
    //tolerance
    float e = 1.0;
    while ((1+e) > 1.0)
        e /= 2.0;

    for(unsigned int fract1 = 0; fract1 < fracture.numberFractures; fract1++)
    {
        for(unsigned int fract2 = (fract1 + 1); fract2 < fracture.numberFractures; fract2++)
        {
            if (((fracture.MinFract[fract1][0] >= fracture.MinFract[fract2][0] - e
                  && fracture.MinFract[fract1][0] <= fracture.MaxFract[fract2][0] + e)
                 || (fracture.MinFract[fract2][0] >= fracture.MinFract[fract1][0] - e
                  && fracture.MinFract[fract2][0] <= fracture.MaxFract[fract1][0] + e))
                &&
                ((fracture.MinFract[fract1][1] >= fracture.MinFract[fract2][1] - e
                  && fracture.MinFract[fract1][1] <= fracture.MaxFract[fract2][1] + e)
                 || (fracture.MinFract[fract2][1] >= fracture.MinFract[fract1][1] - e
                  && fracture.MinFract[fract2][1] <= fracture.MaxFract[fract1][1] + e))
                &&
                ((fracture.MinFract[fract1][2] >= fracture.MinFract[fract2][2] - e
                  && fracture.MinFract[fract1][2] <= fracture.MaxFract[fract2][2] + e)
                 || (fracture.MinFract[fract2][2] >= fracture.MinFract[fract1][2] - e
                     && fracture.MinFract[fract2][2] <= fracture.MaxFract[fract1][2] + e))
                )
            {
                fracture.IDFracturesComparable.push_back({fract1, fract2}); //IDFractures to compare
            }
        }

    }
}

}


