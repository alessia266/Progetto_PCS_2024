#ifndef DFN_TEST_H
#define DFN_TEST_H

#include <gtest/gtest.h>
#include <fstream>
#include "DFN.hpp"
#include "Utils.hpp"

using namespace DFNLibrary;
using namespace std;

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good(); //return true if the file was opened correctly
}

//tolerance
double epsilon(){
    double toll = 1.0;
    while ((1+toll) > 1.0)
        toll /= 2.0;
    const double e = toll;
    return e;
}


//IMPORT FRACTURES TEST
TEST(DFNTest, ImportFractTest)
{
    Fractures fracture;
    string filename3="FR3_data.txt";
    string filename10="FR10_data.txt";
    string filename50="FR50_data.txt";
    string filename82="FR82_data.txt";
    string filename200="FR200_data.txt";
    string filename362="FR362_data.txt";

    if(fileExists(filename3))
    {
        bool result = ImportFract(filename3, fracture);
        ASSERT_TRUE(result);
        ASSERT_EQ(fracture.numberFractures, 3);  //expected value & actual value

    } else if(fileExists(filename10))
    {
        bool result = ImportFract(filename10, fracture);
        ASSERT_TRUE(result);
        ASSERT_EQ(fracture.numberFractures, 10);  //expected value & actual value

    } else if(fileExists(filename50))
    {
        bool result = ImportFract(filename50, fracture);
        ASSERT_TRUE(result);
        ASSERT_EQ(fracture.numberFractures, 50);  //expected value & actual value

    } else if(fileExists(filename82))
    {
        bool result = ImportFract(filename82, fracture);
        ASSERT_TRUE(result);
        ASSERT_EQ(fracture.numberFractures, 82);  //expected value & actual value

    } else if(fileExists(filename200))
    {
        bool result = ImportFract(filename200, fracture);
        ASSERT_TRUE(result);
        ASSERT_EQ(fracture.numberFractures, 200);  //expected value & actual value

    } else if(fileExists(filename362))
    {
        bool result = ImportFract(filename362, fracture);
        ASSERT_TRUE(result);
        ASSERT_EQ(fracture.numberFractures, 362);  //expected value & actual value

    }
}

TEST(DFNTest, CheckNumberTraces)
{
    Fractures fracture;
    Traces traces;
    const double e = epsilon();
    FilterFract(fracture, e);
    CalculateIntersection(fracture, traces, e);
    int numFilteredFract = fracture.IDFracturesComparable.size();
    int numTraces = traces.IDTraceFract.size();
    ASSERT_GE(numFilteredFract, numTraces); // check that the number of possible intersections considered by the first filter is >= to the number of traces
}

TEST(DFNTest, CheckNumberSubPolygons)
{
    Fractures fracture;
    const double e = epsilon();
    Traces traces;
    CalculateIntersection(fracture, traces, e);
    CalculateSubPolygons(fracture, traces, e);
    ExportPolygonalMesh(fracture);

    // Esegui il test per ogni frattura
    for (map<unsigned int, PolygonalMesh>::iterator it = fracture.Polygons.begin();
         it != fracture.Polygons.end(); it++)
    {
        // Verifica il numero di celle (NumberCell2D) per la frattura attuale
        unsigned int numCells = fracture.Polygons[it->first].NumberCell2D;

        // Verifica il numero di tracce (IDTraceFract) per la frattura attuale
        unsigned int numTraces = fracture.FractureSortTraces[it->first].size();

        // Verifica che il numero di celle sia maggiore o uguale al numero di tracce + 1
        ASSERT_GE(numCells, numTraces + 1)<<"Numero celle " << numCells << " numero tracce " << numTraces << endl;
    }
}



#endif // DFN_TEST_H
