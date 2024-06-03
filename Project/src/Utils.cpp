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


        double minX = 10e127;
        double maxX = -10e127;
        double minY = 10e127;
        double maxY = -10e127;
        double minZ = 10e127;
        double maxZ = -10e127;

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

void FilterFract(Fractures& fracture)
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


//intersection line between the planes containing each fracture, checks for parallelism between the planes,
//solve a linear system to find the intersection point, and add it to the trace
bool CalculateIntersection(Fractures& fracture, Traces& traces)
{
    //tolerance
    float e = 1.0;
    while ((1+e) > 1.0)
        e /= 2.0;

    for (unsigned int i = 0; i < size(fracture.IDFracturesComparable); i++)
    {
        unsigned int ID1 = fracture.IDFracturesComparable[i][0];
        unsigned int ID2 = fracture.IDFracturesComparable[i][1];

        vector<Vector3d>& fracture1 = fracture.IDFracture[ID1]; //fracture's coord
        vector<Vector3d>& fracture2 = fracture.IDFracture[ID2]; //fracture's coord

        //normal plane
        Vector3d norm1 = {0, 0, 0};
        Vector3d norm2 = {0, 0, 0};
        Vector3d point1 = fracture1[1];
        Vector3d point2 = fracture2[1];

        unsigned int k = 2;
        while (norm1.norm() < e && k < size(fracture1))
        {
            norm1 = (fracture1[1] - fracture1[0]).cross(fracture1[k] - fracture1[0]); //vector product
            k++;
        }
        if (k == size(fracture1))
        {
            cout << "Fracture " << ID1 << " is degenerate";
            //return false;
        }

        k = 2;
        while (norm2.norm() < e && k < size(fracture2))
        {
            norm2 = (fracture2[1] - fracture2[0]).cross(fracture2[k] - fracture2[0]); //vector product
            k++;
        }
        if (k == size(fracture2))
        {
            cout << "Fracture " << ID2 << " is degenerate";
            //return false;
        }

        cout << "Norm1: " << norm1 << endl;
        cout << "Norm2: " << norm2 << endl;

        //direction of the intersection line between the two plane
        Vector3d t = norm1.cross(norm2);

        cout <<  "Direction: " << t << endl;

        //check parallel planes
        if(t.norm() < e)
        {
            cout << "The planes are parallel" << endl;
            //return false; //no intersection
        }

        //linear system
        Matrix3d A;
        Vector3d b;
        A << norm1[0], norm1[1], norm1[2],
            norm2[0], norm2[1], norm2[2],
            t[0], t[1], t[2];
        b << norm1.dot(point1), norm2.dot(point2), 0;

        if(A.determinant() < e)
        {
            cout << "No unique intersection line" << endl;
            //return false; //no intersection
        }

        //intersection point
        Vector3d intersectionPoint = A.colPivHouseholderQr().solve(b);


        //projection fracture on axes        
        //find the dimension to ignore (ptrMax1, ptrMax2)
        unsigned int ptrMin1 = 0;
        unsigned int ptrMin2 = 0;
        vector<Vector3d> unitvector = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        double valMin1 = norm1.cross(unitvector[0]).norm();
        double valMin2 = norm2.cross(unitvector[0]).norm();
        for (unsigned int j = 1; j < 3; j++)
        {
            double tmp = norm1.cross(unitvector[j]).norm();
            if (tmp < valMin1)
            {
                valMin1 = tmp;
                ptrMin1 = j;
            }
            tmp = norm2.cross(unitvector[j]).norm();
            if (tmp < valMin2)
            {
                valMin2 = tmp;
                ptrMin2 = j;
            }
        }

        //create new 2D variables
        unsigned int index1 = 0;
        unsigned int index2 = 0;
        vector<Vector2d> fractProjection1(size(fracture1));
        vector<Vector2d> fractProjection2(size(fracture2));
        Vector2d tanProjection1;
        Vector2d tanProjection2;
        Vector2d pointProjection1;
        Vector2d pointProjection2;


        for (unsigned int j = 0; j < 3; j++)
        {
            if (j!=ptrMin1)
            {
                for (unsigned int k=0; k < size(fracture1); k++)
                    fractProjection1[k][index1] = fracture1[k][j];
                tanProjection1(index1) = t[j];
                pointProjection1(index1) = intersectionPoint[j];
                index1++;
            }
            if (j!=ptrMin2)
            {
                for (unsigned int k=0; k < size(fracture2); k++)
                    fractProjection2[k][index2] = fracture2[k][j];
                tanProjection2(index2) = t[j];
                pointProjection2(index2) = intersectionPoint[j];
                index2++;
            }
        }

    }
    return true;

}

vector<Vector2d> SegmentIntersection(vector<Vector2d> fractProjection1,
                             vector<Vector2d> fractProjection2,
                             Vector2d tanProjection1,
                             Vector2d tanProjection2,
                             Vector2d pointProjection1,
                             Vector2d pointProjection2)
{

}

}


