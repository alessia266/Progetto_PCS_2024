#pragma once

#include <iostream>
#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;


namespace DFNLibrary{

struct Intersections
{
    vector<Vector3d> IntersectionCoord = {}; //coordinates intersections between fracture end trace direction
    vector<unsigned int> sideId = {}; //marker
    bool Tips;  //passante/non passante
    double Length; //trace length

};

struct SortingTraces
{
    unsigned int TraceID;
    bool Tips;
    double Length;

    //operator to define the order in type SortingTrace
    friend bool operator>(const SortingTraces& s1, const SortingTraces& s2)
    {
        if (s1.Tips == false && s2.Tips == true) return true;
        if (s1.Tips == true && s2.Tips == false) return false;
        return (s1.Length > s2.Length);
    }

    // friend bool operator<=(const SortingTraces& s1, const SortingTraces& s2)
    // {
    //     if (s1.Tips == false && s2.Tips == true) return false;
    //     if (s1.Tips == true && s2.Tips == false) return true;
    //     return (s1.Length <= s2.Length);
    // }
};


struct Utils
{
    unsigned int DimToIgnore;  //dimension to ignore in projection in 2D
    Vector3d Norm;  //normal vector to the fracture
};


struct PolygonalMesh
{
    //cell0D
    unsigned int NumberCell0D = 0;
    map<unsigned int, Vector3d> IdCoord0D = {};  //map: {id coordinate, Coordinate}

    //cell1D
    unsigned int NumberCell1D = 0;
    map<unsigned int, Vector2i> IdEdg1D = {};  //map: {id edge, id coordinates}


    //cell2D
    unsigned int NumberCell2D = 0;
    map<unsigned int, vector<unsigned int>> VerticesCell2D = {};  //map: {id polygon, id vertices}
    map<unsigned int, vector<unsigned int>> EdgesCell2D = {};   //map: {id polygon, id edges}

};



struct Fractures
{
    unsigned int numberFractures = 0;
    map<unsigned int, vector<Vector3d>>  IDFracture = {}; //map: {ID, {{coordinates of a vertex}}}
    map<unsigned int, Vector3d>  MinFract = {}; //map: {ID, {smallest coordinates x, y, z}}
    map<unsigned int, Vector3d>  MaxFract = {}; //map: {ID, {biggest coordinates x, y, z}}
    vector<Vector2i> IDFracturesComparable ={};
    map <unsigned int, Utils> UtilsID = {}; //map: {ID, dimension to ignore in fracture projection}
    map <unsigned int, map<unsigned int, Intersections>> IntersectionsFracture = {}; //map {IDFracture, {{IDTrace, {{{coord}}, sideId, Tips, length}}}}
    map<unsigned int, vector<SortingTraces>> FractureSortTraces = {}; //map {IDFracture, sorted vector of struct SortingTrace}
    map<unsigned int, PolygonalMesh> Polygons = {}; //map {ID fracture, struct PolygonalMesh}
};



struct Traces
{
    map<unsigned int, Vector2i> IDTraceFract = {}; //map: {IDTrace, {IDFractures}}
    map<unsigned int, vector<Vector3d>> IDTraceCoord = {}; //map: {IDTrace, {{coordinates of an endpoint}}}
};




}



