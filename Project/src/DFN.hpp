#pragma once

#include <iostream>
#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;


namespace DFNLibrary{

struct Intersections
{
    vector<Vector3d> IntersectionCoord = {};
    bool Tips;
    double Length;
};

struct SortingTraces
{
    unsigned int TraceID;
    bool Tips;
    double Length;

    friend bool operator>(const SortingTraces& s1, const SortingTraces& s2)
    {
        if (s1.Tips == false && s2.Tips == true) return true;
        if (s1.Tips == true && s2.Tips == false) return false;
        return (s1.Length > s2.Length);
    }

};


struct Fractures
{
    unsigned int numberFractures = 0;
    map<unsigned int, vector<Vector3d>>  IDFracture = {}; //map: {ID, {{coordinates of a vertex}}}
    map<unsigned int, Vector3d>  MinFract = {}; //map: {ID, {smallest coordinates x, y, z}}
    map<unsigned int, Vector3d>  MaxFract = {}; //map: {ID, {biggest coordinates x, y, z}}
    vector<Vector2i> IDFracturesComparable ={};
    map <unsigned int, map<unsigned int, Intersections>> IntersectionsFracture = {}; //map {IDFracture, {{IDTrace, {{{coord}}, {Marker}, Tips, length}}}}
    map<unsigned int, vector<SortingTraces>> FractureSortTraces = {}; //map {IDFracture, sorted vector of traces}
};



struct Traces
{
    map<unsigned int, Vector2i> IDTraceFract = {}; //map: {IDTrace, {IDFractures}}
    map<unsigned int, vector<Vector3d>> IDTraceCoord = {}; //map: {IDTrace, {{coordinates of an endpoint}}}
};

}
