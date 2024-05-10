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

    if (!ImportFract(filename,
                     fracture))
        return 1;


    return 0;
}
