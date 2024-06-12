#pragma once
#include <iostream>
#include "DFN.hpp"
#include <vector>

using namespace std;

namespace DFNLibrary{

bool ImportFract(const string& filename,
                 Fractures& fracture);

bool ExportTraces(Traces& traces);

bool ExportFractTraces(Fractures& fracture);

void FilterFract(Fractures& fracture,
                 const double& e);

bool CalculateIntersection(Fractures& fracture,
                           Traces& traces,
                           const double& e);

bool FindIntersection(vector<Vector2d>& tan,
                      vector<Vector2d>& side,
                      vector<Vector2d>& intersection,
                      const double& e);


void SegmentIntersection(vector<Vector2d>& fractProjection,
                         Vector2d& tanProjection,
                         Vector2d& pointProjection,
                         const double& e,
                         vector<Vector2d>& coordIntersection,
                         Vector2d& parameter);

}
