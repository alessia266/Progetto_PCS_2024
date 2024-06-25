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

TEST(DFNTest, FilterFractTest)
{
    const double e = epsilon();
    Fractures fracture;
    fracture.numberFractures = 3;
    fracture.IDFracture[0] = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
    fracture.IDFracture[1] = {{0.8, 0, -0.1}, {0.8, 0, 0.3}, {0.8, 1, 0.3}, {0.8, 1, -0.1}};
    fracture.IDFracture[2] = {{-0.24, 0.5, -0.34}, {0.31,0.5,-0.34}, {0.31,0.5,0.45}, {-0.24,0.5,0.45}};
    fracture.MinFract[0] = {0, 0, 0};
    fracture.MaxFract[0] = {1, 1, 0};
    fracture.MinFract[1] = {0.8, 0, -0.1};
    fracture.MaxFract[1] = {0.8, 1, 0.3};
    fracture.MinFract[2] = {-0.24, 0.5, -0.34};
    fracture.MaxFract[2] = {0.31, 0.5, 0.45};

    vector<Vector2i> possible = {{0,1}, {0,2}};

    FilterFract(fracture, e);

    EXPECT_EQ(fracture.IDFracturesComparable, possible);
}

TEST(DFNTest, FindIntersectionTest)
{
    const double e = epsilon();
    vector<Vector2d> tangente = {{0.8, 0}, {0.8, 1}};
    vector<Vector2d> side1 = {{0, 0}, {1, 0}};
    vector<Vector2d> side2 = {{0, 0}, {0, 1}};
    vector<Vector2d> intersection;


    EXPECT_TRUE(FindIntersection(tangente, side1, intersection, e));
    EXPECT_FALSE(FindIntersection(tangente, side2, intersection, e));
}

TEST(DFNTest, SegmentIntersectionTest)
{
    const double e = epsilon();
    vector<Vector2d> fractProj = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    Vector2d tanProj = {0, -0.4};
    Vector2d pointProj = {0.8, 0};
    vector<Vector2d> coordIntersection;
    Vector2d parameter;
    vector<unsigned int> sideID;

    SegmentIntersection(fractProj, tanProj, pointProj, e, coordIntersection, parameter, sideID);

    Vector2d par1 = {0, -2.5};

    EXPECT_EQ(parameter, par1);
}

TEST(DFNTest, ComputeProjectionTest)
{
    const double e = epsilon();
    Fractures fracture;
    unsigned int id = 0;
    fracture.IDFracture[id] = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};

    ComputeProjection(fracture, id, e);

    Vector3d norm = {0, 0, 1};

    EXPECT_EQ(fracture.UtilsID[id].Norm, norm);
    EXPECT_EQ(fracture.UtilsID[id].DimToIgnore, 2);
}


TEST(DFNTest, CalculateIntersectionTest)
{
    const double e = epsilon();
    Fractures fracture;
    Traces traces;
    fracture.IDFracturesComparable = {{0,1}, {0,2}};
    fracture.IDFracture[0] = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
    fracture.IDFracture[1] = {{0.8, 0, -0.1}, {0.8, 0, 0.3}, {0.8, 1, 0.3}, {0.8, 1, -0.1}};
    fracture.IDFracture[2] = {{-0.24, 0.5, -0.34}, {0.31,0.5,-0.34}, {0.31,0.5,0.45}, {-0.24,0.5,0.45}};
    fracture.UtilsID[0].Norm = {0,0,1};
    fracture.UtilsID[1].Norm = {-4, 0, 0};
    fracture.UtilsID[2].Norm = {0, -0.4, 0};
    fracture.UtilsID[0].DimToIgnore = 2;
    fracture.UtilsID[1].DimToIgnore = 0;
    fracture.UtilsID[2].DimToIgnore = 1;

    EXPECT_TRUE(CalculateIntersection(fracture, traces, e));
}

#endif // DFN_TEST_H
