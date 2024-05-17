#pragma once
#include <iostream>
#include "DFN.hpp"

using namespace std;

namespace DFNLibrary{

bool ImportFract(const string& filename,
                 Fractures& fracture);

bool FilterFract(Fractures& fracture);

}
