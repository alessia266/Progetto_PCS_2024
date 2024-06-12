#include <iostream>
#include <fstream>
#include "Utils.hpp"
#include "DFN.hpp"

using namespace std;
using namespace DFNLibrary;
using namespace Eigen;

int main()
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

    return 0;
}
