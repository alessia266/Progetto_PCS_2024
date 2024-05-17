#pragma once

#include <iostream>
#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;


namespace DFNLibrary{

struct Fractures
{
    unsigned int numberFractures = 0;
    std::map<unsigned int, vector<Vector3d>>  IDFracture = {}; //map: {ID, {{coordinates of a vertex}}}
    std::map<unsigned int, Vector3d>  MinFract = {}; //map: {ID, {{smallest coordinates x, y, z}}
    std::map<unsigned int, Vector3d>  MaxFract = {}; //map: {ID, {{biggest coordinates x, y, z}}
    vector<Vector2i> IDFracturesComparable ={};
};


struct Traces
{
    std::map<unsigned int, Vector2d> IDTraceFract = {}; //map: {IDTrace, {IDFractures}}
    std::map<unsigned int, vector<Vector3d>> IDTraceCoord = {}; //map: {IDTrace, {{coordinates of an endpoint}}}

};

}
