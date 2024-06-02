#include <iostream>
#include <fstream>
#include "Utils.hpp"
#include "DFN.hpp"

using namespace std;
using namespace DFNLibrary;
using namespace Eigen;

int main()
{
    string filename;
    cout << "File name:" << endl;
    cin >> filename;
    filename = "DFN/" + filename;

    Fractures fracture;
    Traces trace;

    if (!ImportFract(filename,
                     fracture))
        return 1;

    FilterFract(fracture);

    if (!CalculateIntersection(fracture,
                               trace))
        return 2;

    return 0;
}
