#pragma once
#include <iostream>
#include "DFN.hpp"
#include <vector>

using namespace std;

namespace DFNLibrary{

bool ImportFract(const string& filename,
                 Fractures& fracture);

void FilterFract(Fractures& fracture);

bool CalculateIntersection(Fractures& fracture,
                           Traces& traces);
vector<Vector2d> SegmentIntersection(vector<Vector2d> fractProjection1,
                             vector<Vector2d> fractProjection2,
                             Vector2d tanProjection1,
                             Vector2d tanProjection2,
                             Vector2d pointProjection1,
                             Vector2d pointProjection2);

}
