#include <iostream>
#include <fstream>
#include "Utils.hpp"
#include "DFN.hpp"
#include "DFN_Test.hpp"
#include "UCDUtilities.hpp"



using namespace std;
using namespace DFNLibrary;
using namespace Eigen;
using namespace Gedim;

int main(int argc, char **argv)
{

    //tolerance
    double toll = 1.0;
    while ((1+toll) > 1.0)
        toll /= 2.0;
    const double e = toll;

    string filename;
    cout << "File name:" << endl;
    cin >> filename;
    filename = "DFN/" + filename;

    Fractures fracture;
    Traces trace;


    if (!ImportFract(filename,
                     fracture))
        return 1;

    FilterFract(fracture,
                e);

    if (!CalculateIntersection(fracture,
                               trace,
                               e))
        return 2;


    if (!ExportTraces(trace))
        return 3;

    if (!ExportFractTraces(fracture))
        return 4;

    if (!CalculateSubPolygons(fracture,
                              trace,
                              e))
        return 5;

    if (!ExportPolygonalMesh(fracture))
        return 6;



    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    //exporting fractureId 0 in paraview
    auto fracture0 = fracture.Polygons[0];
    string filepath = "./";
    MatrixXd point = MatrixXd::Zero(3, size(fracture0.IdCoord0D));
    MatrixXi segments = MatrixXi::Zero(2, size(fracture0.IdEdg1D));

    unsigned int c = 0;
    for(map<unsigned int, Vector3d>::iterator it = fracture0.IdCoord0D.begin();
         it != fracture0.IdCoord0D.end(); it++)
    {
        for (unsigned int r = 0; r < 3; r++)
        {
            point(r,c) = it->second[r];
        }
        c++;
    }
    unsigned int edg = 0;
    for(map<unsigned int, Vector2i>::iterator it1 = fracture0.IdEdg1D.begin();
         it1 != fracture0.IdEdg1D.end(); it1++)
    {
        segments(0, edg) = it1->second[0];
        segments(1, edg) = it1->second[1];
        edg++;
    }


    Gedim::UCDUtilities exporter;

    exporter.ExportPoints(filepath + "/Geometry0Ds.inp",
                          point);
    exporter.ExportSegments(filepath + "/Geometry1Ds.inp",
                            point,
                            segments);


    return 0;
}

