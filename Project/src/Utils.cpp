#include "DFN.hpp"
#include "SortingAlgorithm.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>

using namespace std;

namespace DFNLibrary{


bool ImportFract(const string& filename,
                 Fractures& fracture)
{
    ifstream file(filename);

    if(file.fail())
    {
        cerr << "File not found" << endl;
        return false;
    }

    string line;

    //ignored line "Number of Fractures"
    getline(file, line);

    //number of fractures
    getline(file, line);
    istringstream convert(line);
    convert >> fracture.numberFractures;

    for (unsigned int i = 0; i < fracture.numberFractures; i++)
    {
        //ignored line "FractureId; NumVertices"
        getline(file, line);

        //fracture Id, NumVertices
        getline(file, line);
        replace(line.begin(), line.end(), ';', ' ');
        istringstream convert(line);
        unsigned int id;
        unsigned int numVert;
        convert >> id >> numVert;

        //ignored line "Vertices"
        getline(file, line);

        //coordinates
        string coord_x;
        string coord_y;
        string coord_z;

        //x
        getline(file, coord_x);
        replace(coord_x.begin(), coord_x.end(), ';', ' ');
        istringstream convertX(coord_x);

        //y
        getline(file, coord_y);
        replace(coord_y.begin(), coord_y.end(), ';', ' ');
        istringstream convertY(coord_y);

        //z
        getline(file, coord_z);
        replace(coord_z.begin(), coord_z.end(), ';', ' ');
        istringstream convertZ(coord_z);


        double minX = 10e127;
        double maxX = -10e127;
        double minY = 10e127;
        double maxY = -10e127;
        double minZ = 10e127;
        double maxZ = -10e127;

        for (unsigned int j = 0; j < numVert; j++)
        {
            Vector3d coord;
            convertX >> coord[0];
            //assegnazione minimo e massimo della variabile x
            if (minX > coord[0])
                minX = coord[0];
            if (maxX < coord[0])
                maxX = coord[0];

            convertY >> coord[1];
            //assegnazione minimo e massimo della variabile y
            if (minY > coord[1])
                minY = coord[1];
            if (maxY < coord[1])
                maxY = coord[1];

            convertZ >> coord[2];
            //assegnazione minimo e massimo della variabile z
            if (minZ > coord[2])
                minZ = coord[2];
            if (maxZ < coord[2])
                maxZ = coord[2];

            fracture.IDFracture[i].push_back(coord);

        }
        fracture.MinFract[i] = {minX, minY, minZ};
        fracture.MaxFract[i] = {maxX, maxY, maxZ};
    }
    return true;
}


//exclude non-intersecting fractures
void FilterFract(Fractures& fracture,
                 const double& e)
{
    for(unsigned int fract1 = 0; fract1 < fracture.numberFractures; fract1++)
    {
        for(unsigned int fract2 = (fract1 + 1); fract2 < fracture.numberFractures; fract2++)
        {
            if (((fracture.MinFract[fract1][0] >= fracture.MinFract[fract2][0] - e
                  && fracture.MinFract[fract1][0] <= fracture.MaxFract[fract2][0] + e)
                 || (fracture.MinFract[fract2][0] >= fracture.MinFract[fract1][0] - e
                     && fracture.MinFract[fract2][0] <= fracture.MaxFract[fract1][0] + e))
                &&
                ((fracture.MinFract[fract1][1] >= fracture.MinFract[fract2][1] - e
                  && fracture.MinFract[fract1][1] <= fracture.MaxFract[fract2][1] + e)
                 || (fracture.MinFract[fract2][1] >= fracture.MinFract[fract1][1] - e
                     && fracture.MinFract[fract2][1] <= fracture.MaxFract[fract1][1] + e))
                &&
                ((fracture.MinFract[fract1][2] >= fracture.MinFract[fract2][2] - e
                  && fracture.MinFract[fract1][2] <= fracture.MaxFract[fract2][2] + e)
                 || (fracture.MinFract[fract2][2] >= fracture.MinFract[fract1][2] - e
                     && fracture.MinFract[fract2][2] <= fracture.MaxFract[fract1][2] + e))
                )
            {
                fracture.IDFracturesComparable.push_back({fract1, fract2}); //IDFractures to compare
            }
        }
    }
}



//intersection between two segments
bool FindIntersection(vector<Vector2d>& tan,
                      vector<Vector2d>& side,
                      vector<Vector2d>& intersection,
                      const double& e)
{
    Vector2d i1 = tan[0];
    Vector2d f1 = tan[1];
    Vector2d i2 = side[0];
    Vector2d f2 = side[1];

    if ((abs(i1[0] - f1[0]) < e) && (abs(i1[1] - f1[1]) < e))
    {
        if (((abs(i1[0] - i2[0]) < e) && ((i1[1] > i2[1] && i1[1] < (f2[1] + e)) || (i1[1] < (i2[1] + e) && i1[1] > f2[1])))
            || ((abs(i1[1] - i2[1]) < e) && ((i1[0] > i2[0] && i1[0] < (f2[0] + e)) || (i1[0] < (i2[0] + e) && i1[0] > f2[0]))))
            intersection.push_back(i1);
        return true;
    }
    else
    {
        double det = (f1[0] - i1[0]) * (-f2[1] + i2[1]) + (f1[1] - i1[1]) * (f2[0] - i2[0]);

        if (abs(det) < e*e)
        {
            cout << "No intersections" << endl;
            return false;
        }
        else
        {
            double t = ((i2[0] - i1[0]) * (- f2[1] + i2[1]) + (i2[1] - i1[1]) * (f2[0] - i2[0])) / det;
            double u = ((f1[0] - i1[0]) * (i2[1] - i1[1]) + (f1[1] - i1[1]) * (- i2[0] + i1[0])) / det;

            //Check: intersection is inside the segments
            if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
            {
                intersection.push_back(i1 + t * (f1 - i1));
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}


//compute intersection
void SegmentIntersection(vector<Vector2d>& fractProjection,
                         Vector2d& tanProjection,
                         Vector2d& pointProjection,
                         const double& e,
                         vector<Vector2d>& coordIntersection,
                         Vector2d& parameter,
                         vector<unsigned int>& sideID)
{
    unsigned int ind_v;
    unsigned int dep_v;
    //valuate gradient to select the segment from the direction
    if (abs(tanProjection[0]) > abs(tanProjection[1]))
    {
        ind_v = 0;
        dep_v = 1;
    }
    else
    {
        ind_v = 1;
        dep_v = 0;
    }
    for (unsigned int s = 0; s < size(fractProjection); s++)
    {
        //selection of the segment (side of the fracture) to analyze
        vector<Vector2d> side;
        if (s != size(fractProjection) -1)
            side = {fractProjection[s], fractProjection[s+1]};
        else
            side = {fractProjection[s], fractProjection[0]};


        vector<Vector2d> tanSegment = {{0, 0}, {0, 0}};
        float var1;
        float var2;
        float product;

        var1 = pointProjection[dep_v] + (side[0][ind_v] - pointProjection[ind_v])*(tanProjection[dep_v]/tanProjection[ind_v]);
        var2 = pointProjection[dep_v] + (side[1][ind_v] - pointProjection[ind_v])*(tanProjection[dep_v]/tanProjection[ind_v]);
        tanSegment[0][ind_v] = side[0][ind_v];
        tanSegment[0][dep_v] = var1;
        tanSegment[1][ind_v] = side[1][ind_v];
        tanSegment[1][dep_v] = var2;

        //check whether the endpoints of a segment are both major or both minors than the endpoints of the other segment
        //if the result of the product is positive the segments do not intersect each others
        product = (tanSegment[0][dep_v] - side[0][dep_v])*(tanSegment[1][dep_v] - side[1][dep_v]);

        if (product < -e)
        {
            if (!FindIntersection(tanSegment,
                                  side,
                                  coordIntersection,
                                  e))
                cout << "Error finding the intersection" << endl;
            else
                sideID.push_back(s);
        }

    }

    if (size(coordIntersection) == 2)
    {
        double p1 = (coordIntersection[0][ind_v] - pointProjection[ind_v])/tanProjection[ind_v];
        double p2 = (coordIntersection[1][ind_v] - pointProjection[ind_v])/tanProjection[ind_v];
        parameter = {p1, p2};
    }
    if (size(coordIntersection) > 2)
        cerr << "There are more than two intersections in a fracture" << endl;

}

void ComputeProjection(Fractures& fracture,
                       unsigned int& id,
                       const double& e)
{
    if (fracture.UtilsID.find(id) == fracture.UtilsID.end())
    {
        vector<Vector3d> fract = fracture.IDFracture[id]; //fracture's coord
        Vector3d nor = {0, 0, 0};
        unsigned int k = 2;
        while (nor.norm() < e && k < size(fract))
        {
            nor = (fract[1] - fract[0]).cross(fract[k] - fract[0]); //vector product
            k++;
        }
        if (k == size(fract))
        {
            cout << "Fracture " << id << " is degenerate";
        }
        fracture.UtilsID[id].Norm = nor;

        //projection fracture on axes
        //find the dimension to ignore (ptrMin)
        unsigned int ptrMin = 0;
        vector<Vector3d> idMatrix = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        double valMin = nor.cross(idMatrix[0]).norm();
        for (unsigned int j = 1; j < 3; j++)
        {
            double tmp = nor.cross(idMatrix[j]).norm();
            if (tmp < valMin)
            {
                valMin = tmp;
                ptrMin = j;
            }
        }
        fracture.UtilsID[id].DimToIgnore = ptrMin;
    }
}


//intersection line between the planes containing each fracture, checks for parallelism between the planes,
//solve a linear system to find the intersection point, and add it to the trace
bool CalculateIntersection(Fractures& fracture,
                           Traces& traces,
                           const double& e)
{
    unsigned int traceID = 0;

    for (unsigned int i = 0; i < size(fracture.IDFracturesComparable); i++)
    {
        unsigned int ID1 = fracture.IDFracturesComparable[i][0];
        unsigned int ID2 = fracture.IDFracturesComparable[i][1];

        ComputeProjection(fracture,
                          ID1,
                          e);
        ComputeProjection(fracture,
                          ID2,
                          e);

        vector<Vector3d> fracture1 = fracture.IDFracture[ID1]; //fracture's coord
        vector<Vector3d> fracture2 = fracture.IDFracture[ID2]; //fracture's coord

        Vector3d point1 = fracture1[1];
        Vector3d point2 = fracture2[1];
        Vector3d norm1 = fracture.UtilsID[ID1].Norm;
        Vector3d norm2 = fracture.UtilsID[ID2].Norm;

        //direction of the intersection line between the two plane
        Vector3d t = norm1.cross(norm2);

        Vector3d intersectionPoint;
        //check parallel planes: if (t.norm() < e) planes are parallel
        if(t.norm() > e)
        {
            //linear system
            Matrix3d A;
            Vector3d b;
            A << norm1[0], norm1[1], norm1[2],
                norm2[0], norm2[1], norm2[2],
                t[0], t[1], t[2];
            b << norm1.dot(point1), norm2.dot(point2), 0;

            //if abs(A.determinant()) < e there is no unique intersection line
            if(abs(A.determinant()) > e)
            {
                //intersection point
                intersectionPoint = A.colPivHouseholderQr().solve(b);

                //create new 2D variables
                unsigned int index1 = 0;
                unsigned int index2 = 0;
                vector<Vector2d> fractProjection1(size(fracture1));
                vector<Vector2d> fractProjection2(size(fracture2));
                Vector2d tanProjection1;
                Vector2d tanProjection2;
                Vector2d pointProjection1;
                Vector2d pointProjection2;
                Vector2d parFract1;
                Vector2d parFract2;
                vector<unsigned int> sideID1 = {};
                vector<unsigned int> sideID2 = {};


                for(unsigned int j = 0; j < 3; j++)
                {
                    if (j!=fracture.UtilsID[ID1].DimToIgnore)
                    {
                        for (unsigned int k=0; k < size(fracture1); k++)
                            fractProjection1[k][index1] = fracture1[k][j];
                        tanProjection1(index1) = t[j];
                        pointProjection1(index1) = intersectionPoint[j];
                        index1++;
                    }
                    if (j!=fracture.UtilsID[ID2].DimToIgnore)
                    {
                        for (unsigned int k=0; k < size(fracture2); k++)
                            fractProjection2[k][index2] = fracture2[k][j];
                        tanProjection2(index2) = t[j];
                        pointProjection2(index2) = intersectionPoint[j];
                        index2++;
                    }
                }

                //intersection between t and fracture ID1
                vector<Vector2d> intFract1 = {};
                SegmentIntersection(fractProjection1,
                                    tanProjection1,
                                    pointProjection1,
                                    e,
                                    intFract1,
                                    parFract1,
                                    sideID1);
                parFract1 = {min(parFract1[0], parFract1[1]), max(parFract1[0], parFract1[1])};




                //intersection between t and fracture ID2
                vector<Vector2d> intFract2 = {};
                SegmentIntersection(fractProjection2,
                                    tanProjection2,
                                    pointProjection2,
                                    e,
                                    intFract2,
                                    parFract2,
                                    sideID2);
                parFract2 = {min(parFract2[0], parFract2[1]), max(parFract2[0], parFract2[1])};


                if(size(intFract1) != 0 && size(intFract2) != 0)
                {
                    double maxIn = max(parFract1[0], parFract2[0]);
                    double minFin = min(parFract1[1], parFract2[1]);
                    Vector3d startTrace = {intersectionPoint[0] + maxIn*t[0],
                                           intersectionPoint[1] + maxIn*t[1],
                                           intersectionPoint[2] + maxIn*t[2]};
                    Vector3d endTrace = {intersectionPoint[0] + minFin*t[0],
                                         intersectionPoint[1] + minFin*t[1],
                                         intersectionPoint[2] + minFin*t[2]};

                    Vector3d differenza = startTrace - endTrace;
                    double moduleTrace = sqrt(differenza[0]*differenza[0] +
                                              differenza[1]*differenza[1] +
                                              differenza[2]*differenza[2]);


                    if(maxIn < minFin + e)
                    {
                        //insert traces in struct trace and intersection in struct fracture
                        traces.IDTraceFract[traceID] = {ID1, ID2};
                        traces.IDTraceCoord[traceID] = {startTrace, endTrace};


                        bool Tips;
                        if ((abs(parFract1[0] - maxIn) < e && abs(parFract1[1] - minFin) < e))
                            Tips = false;
                        else
                            Tips = true;

                        Vector3d startIntersection = {intersectionPoint[0] + parFract1[0]*t[0],
                                                      intersectionPoint[1] + parFract1[0]*t[1],
                                                      intersectionPoint[2] + parFract1[0]*t[2]};
                        Vector3d endIntersection = {intersectionPoint[0] + parFract1[1]*t[0],
                                                    intersectionPoint[1] + parFract1[1]*t[1],
                                                    intersectionPoint[2] + parFract1[1]*t[2]};

                        fracture.IntersectionsFracture[ID1].insert({traceID, {{startIntersection, endIntersection}, sideID1, Tips, moduleTrace}});


                        if ((abs(parFract2[0] - maxIn) < e && abs(parFract2[1] - minFin) < e))
                            Tips = false;
                        else
                            Tips = true;

                        startIntersection = {intersectionPoint[0] + parFract2[0]*t[0],
                                             intersectionPoint[1] + parFract2[0]*t[1],
                                             intersectionPoint[2] + parFract2[0]*t[2]};
                        endIntersection = {intersectionPoint[0] + parFract2[1]*t[0],
                                           intersectionPoint[1] + parFract2[1]*t[1],
                                           intersectionPoint[2] + parFract2[1]*t[2]};

                        fracture.IntersectionsFracture[ID2].insert({traceID, {{startIntersection, endIntersection}, sideID2, Tips, moduleTrace}});

                        traceID++;

                    }

                }
            }
        }
    }
    return true;

}


bool ExportTraces(Traces& traces)
{
    ofstream file;
    file.open("TR_data.txt");

    if (file.fail())
    {
        cerr << "file open failed" << endl;
        return false;
    }

    unsigned int sz = size(traces.IDTraceCoord);
    if (sz == 0)
        cout << "No intersection found" << endl;
    else
    {
        file << "# Number of Traces\n" << sz << endl;
        file << "# TraceId; FractureId1; FractureId2; X1; Y1; Z1; X2; Y2; Z2" << endl;

        for (unsigned int i = 0; i < sz; i++)
        {
            file << i << "; " << traces.IDTraceFract[i][0] << "; " << traces.IDTraceFract[i][1] << "; " <<
                scientific << setprecision(16) <<
                traces.IDTraceCoord[i][0][0] << "; " << traces.IDTraceCoord[i][0][1] << "; " << traces.IDTraceCoord[i][0][2] << "; " <<
                traces.IDTraceCoord[i][1][0] << "; " << traces.IDTraceCoord[i][1][1] << "; " << traces.IDTraceCoord[i][1][2] << endl;
        }
    }

    file.close();

    return true;
}


bool ExportFractTraces(Fractures& fracture)
{
    ofstream file;
    file.open("FR_sortTR.txt");

    if (file.fail())
    {
        cerr << "file open failed" << endl;
        return false;
    }

    if (size(fracture.IntersectionsFracture) == 0)
        cout << "No fracture has intersections" << endl;
    else
    {
        // double sumMerge = 0;
        // double sumBubble = 0;

        for(map <unsigned int, map<unsigned int, Intersections>>::iterator it1 = fracture.IntersectionsFracture.begin();
             it1 != fracture.IntersectionsFracture.end(); it1++)
        {
            file << "# FractureId; NumTraces" << endl;
            file << it1->first << "; " << size(it1->second) << endl;
            file << "# TraceId; Tips; Length" << endl;

            //vector of struct (contains informations to order and print)
            vector<SortingTraces> sortingTrace = {};
            for(map<unsigned int, Intersections>::iterator it2 = it1->second.begin();
                 it2 != it1->second.end(); it2++)
            {
                sortingTrace.push_back({it2->first, it2->second.Tips, it2->second.Length});
            }

            // //comparison sorting altorithms
            // vector<SortingTraces> sortingTrace1 = sortingTrace;
            // vector<SortingTraces> sortingTrace2 = sortingTrace;

            // std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
            // SortLibrary::MergeSort(sortingTrace1);
            // std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
            // SortLibrary::BubbleSort(sortingTrace2);
            // std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();


            // unsigned int duration_MergeSort = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            // //cout << "MergeSort duration: " << duration_MergeSort << endl;
            // sumMerge += duration_MergeSort;

            // unsigned int duration_BubbleSort = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
            // //cout << "BubbleSort duration: " << duration_BubbleSort << endl;
            // sumBubble += duration_BubbleSort;

            //sorting
            SortLibrary::BubbleSort(sortingTrace);
            fracture.FractureSortTraces[it1->first] = sortingTrace;

            for (unsigned int i = size(sortingTrace); i > 0; i--)
                file << sortingTrace[i-1].TraceID << "; " << sortingTrace[i-1].Tips << "; " << scientific << setprecision(16) << sortingTrace[i-1].Length << endl;
        }
        // //comparing durations
        // cout << "Merge: " << sumMerge << endl;
        // cout << "Bubble: " << sumBubble << endl;
        // if (sumMerge < sumBubble)
        //     cout << "MergeSort faster than BubbleSort\n" << endl;
        // else if (sumMerge > sumBubble)
        //     cout << "BubbleSort faster than MergeSort\n" << endl;
        // else
        //     cout << "Same duration\n" << endl;
    }

    file.close();

    return true;
}


bool CalculateSubPolygons(Fractures& fracture,
                          Traces& traces,
                          const double& e)
{
    //loop on fractures
    for(map<unsigned int, vector<SortingTraces>>::iterator it = fracture.FractureSortTraces.begin();
         it != fracture.FractureSortTraces.end(); it++)
    {
        //initialization of the polygonal mesh with the entire fracture
        unsigned int idCell2d = 0;
        unsigned int idCoord = 0;
        unsigned int idEdg = 0;
        unsigned int numvert = size(fracture.IDFracture[it->first]);
        fracture.Polygons[it->first].NumberCell0D = numvert;
        fracture.Polygons[it->first].NumberCell1D = numvert;
        fracture.Polygons[it->first].NumberCell2D = 1;
        for(unsigned int j = 0; j < numvert; j++)
        {
            Vector3d coordinates = {fracture.IDFracture[it->first][idCoord][0],
                                    fracture.IDFracture[it->first][idCoord][1],
                                    fracture.IDFracture[it->first][idCoord][2]};
            fracture.Polygons[it->first].IdCoord0D[idCoord] = coordinates;
            fracture.Polygons[it->first].VerticesCell2D[idCell2d].push_back(j);
            fracture.Polygons[it->first].EdgesCell2D[idCell2d].push_back(j);
            if (j != numvert - 1)
            {
                fracture.Polygons[it->first].IdEdg1D[idEdg] = {idCoord,
                                           idCoord + 1};
            }
            else
                fracture.Polygons[it->first].IdEdg1D[idEdg] = {idCoord,
                                           idCoord - numvert + 1};
            idCoord++;
            idEdg++;

        }
        //loop on traces
        vector<SortingTraces> vecTraces = fracture.FractureSortTraces[it->first];
        for(int i = size(vecTraces) - 1; i > -1 ; i--)
        {
            //projection of trace in 2D to find intersections
            unsigned int j = 0;
            Vector2d coordIn;
            Vector2d coordFin;
            for (unsigned int k = 0; k < 3; k++)
            {
                if (k != fracture.UtilsID[it->first].DimToIgnore)
                {
                    coordIn[j] = traces.IDTraceCoord[vecTraces[i].TraceID][0][k];
                    coordFin[j] = traces.IDTraceCoord[vecTraces[i].TraceID][1][k];
                    j++;
                }
            }
            Vector2d dirTrace = {coordFin[0]-coordIn[0],
                                 coordFin[1]-coordIn[1]};

            auto lastPolygon = prev(fracture.Polygons[it->first].VerticesCell2D.end())->first;
            //loop to project subpolygon in 2D
            for(map<unsigned int, vector<unsigned int>>::iterator it2 = fracture.Polygons[it->first].VerticesCell2D.begin();
                 it2->first <= lastPolygon;)
            {
                vector<Vector2d> fractProj;
                for (unsigned int v = 0; v < size(it2->second); v++)
                {
                    Vector2d vert2d = {};
                    j = 0;
                    for (unsigned int k = 0; k < 3; k++)
                    {
                        if (k != fracture.UtilsID[it->first].DimToIgnore)
                        {
                            vert2d[j] = fracture.Polygons[it->first].IdCoord0D[it2->second[v]][k];
                            j++;
                        }
                    }
                    fractProj.push_back(vert2d);
                }
                vector<Vector2d> coordInters;
                Vector2d par;
                vector<unsigned int> sideId;


                //intersection between fracture and subpolygon
                SegmentIntersection(fractProj,
                                    dirTrace,
                                    coordIn,
                                    e,
                                    coordInters,
                                    par,
                                    sideId);
                //check if intersections found
                if (size(coordInters) == 2)
                {
                    //check if intersection is inside subpolygon
                    double maxPar = max(par[0], par[1]);
                    double minPar = min(par[0], par[1]);
                    if (minPar <= 1. && maxPar > -e)
                    {
                        Vector3d directionTrace = {traces.IDTraceCoord[vecTraces[i].TraceID][1][0] - traces.IDTraceCoord[vecTraces[i].TraceID][0][0],
                                                   traces.IDTraceCoord[vecTraces[i].TraceID][1][1] - traces.IDTraceCoord[vecTraces[i].TraceID][0][1],
                                                   traces.IDTraceCoord[vecTraces[i].TraceID][1][2] - traces.IDTraceCoord[vecTraces[i].TraceID][0][2]};
                        Vector3d startIntersection = {traces.IDTraceCoord[vecTraces[i].TraceID][0][0] + par[0]*directionTrace[0],
                                                      traces.IDTraceCoord[vecTraces[i].TraceID][0][1] + par[0]*directionTrace[1],
                                                      traces.IDTraceCoord[vecTraces[i].TraceID][0][2] + par[0]*directionTrace[2]};
                        Vector3d endIntersection = {traces.IDTraceCoord[vecTraces[i].TraceID][0][0] + par[1]*directionTrace[0],
                                                    traces.IDTraceCoord[vecTraces[i].TraceID][0][1] + par[1]*directionTrace[1],
                                                    traces.IDTraceCoord[vecTraces[i].TraceID][0][2] + par[1]*directionTrace[2]};

                        //update cell 0D
                        fracture.Polygons[it->first].NumberCell0D += 2;
                        fracture.Polygons[it->first].IdCoord0D[idCoord] = startIntersection;
                        fracture.Polygons[it->first].IdCoord0D[idCoord + 1] = endIntersection;

                        //update cell1D
                        fracture.Polygons[it->first].NumberCell1D += 5;
                        fracture.Polygons[it->first].IdEdg1D[idEdg] = {idCoord, idCoord + 1};
                        //edges to divide
                        unsigned int idEdg1 = fracture.Polygons[it->first].EdgesCell2D[it2->first][sideId[0]];
                        unsigned int idEdg2 = fracture.Polygons[it->first].EdgesCell2D[it2->first][sideId[1]];
                        //new edges from division
                        fracture.Polygons[it->first].IdEdg1D[idEdg + 1] = {fracture.Polygons[it->first].IdEdg1D[idEdg1][0], idCoord};
                        fracture.Polygons[it->first].IdEdg1D[idEdg + 2] = {idCoord, fracture.Polygons[it->first].IdEdg1D[idEdg1][1]};
                        fracture.Polygons[it->first].IdEdg1D[idEdg + 3] = {fracture.Polygons[it->first].IdEdg1D[idEdg2][0], idCoord + 1};
                        fracture.Polygons[it->first].IdEdg1D[idEdg + 4] = {idCoord + 1, fracture.Polygons[it->first].IdEdg1D[idEdg2][1]};


                        //update cell2D
                        fracture.Polygons[it->first].NumberCell2D += 2;
                        vector<unsigned int> edgSubPol1 = {};
                        vector<unsigned int> edgSubPol2 = {};
                        vector<unsigned int> verSubPol1 = {fracture.Polygons[it->first].VerticesCell2D[it2->first][0]};
                        vector<unsigned int> verSubPol2 = {};

                        //loop on the sides of polygon
                        unsigned int szEdg = size(fracture.Polygons[it->first].EdgesCell2D[it2->first]);
                        for (unsigned int m = 0; m < szEdg; m++)
                        {
                            //check if the considered side contains the intersection with the trace
                            if (m < sideId[0])
                            {
                                edgSubPol1.push_back(fracture.Polygons[it->first].EdgesCell2D[it2->first][m]);
                                if (m + 1 < szEdg)
                                    verSubPol1.push_back(fracture.Polygons[it->first].VerticesCell2D[it2->first][m+1]);
                            }
                            else if (m == sideId[0])
                            {
                                if (fracture.Polygons[it->first].VerticesCell2D[it2->first][m] ==
                                    fracture.Polygons[it->first].IdEdg1D[fracture.Polygons[it->first].EdgesCell2D[it2->first][m]][0])
                                {
                                    edgSubPol1.push_back(idEdg + 1);
                                    edgSubPol1.push_back(idEdg);

                                    edgSubPol2.push_back(idEdg + 2);

                                    verSubPol1.push_back(idCoord);
                                    verSubPol1.push_back(idCoord + 1);

                                    verSubPol2.push_back(idCoord);
                                    verSubPol2.push_back(fracture.Polygons[it->first].IdEdg1D[fracture.Polygons[it->first].EdgesCell2D[it2->first][m]][1]);
                                }
                                //if the vertex correspond to the second end point of the edge, the two subsegments must be taken in reverse order
                                else
                                {
                                    edgSubPol1.push_back(idEdg + 2);
                                    edgSubPol1.push_back(idEdg);

                                    edgSubPol2.push_back(idEdg + 1);

                                    verSubPol1.push_back(idCoord);
                                    verSubPol1.push_back(idCoord + 1);

                                    verSubPol2.push_back(idCoord);
                                    verSubPol2.push_back(fracture.Polygons[it->first].IdEdg1D[fracture.Polygons[it->first].EdgesCell2D[it2->first][m]][0]);
                                }
                            }
                            else if (m < sideId[1])
                            {
                                edgSubPol2.push_back(fracture.Polygons[it->first].EdgesCell2D[it2->first][m]);
                                if (m + 1 < szEdg)
                                    verSubPol2.push_back(fracture.Polygons[it->first].VerticesCell2D[it2->first][m+1]);
                            }
                            else if (m == sideId[1])
                            {
                                //the vertex
                                if (fracture.Polygons[it->first].VerticesCell2D[it2->first][m] ==
                                    fracture.Polygons[it->first].IdEdg1D[fracture.Polygons[it->first].EdgesCell2D[it2->first][m]][0])
                                {
                                    edgSubPol2.push_back(idEdg + 3);
                                    edgSubPol2.push_back(idEdg);

                                    edgSubPol1.push_back(idEdg + 4);

                                    verSubPol2.push_back(idCoord + 1);

                                    if (m + 1 < szEdg)
                                        verSubPol1.push_back(fracture.Polygons[it->first].IdEdg1D[fracture.Polygons[it->first].EdgesCell2D[it2->first][m]][1]);
                                }
                                //if the vertex correspond to the second end point of the edge, the two subsegments must be taken in reverse order
                                else
                                {
                                    edgSubPol2.push_back(idEdg + 4);
                                    edgSubPol2.push_back(idEdg);

                                    verSubPol2.push_back(idCoord + 1);

                                    edgSubPol1.push_back(idEdg + 3);

                                    if (m + 1 < szEdg)
                                        verSubPol1.push_back(fracture.Polygons[it->first].IdEdg1D[fracture.Polygons[it->first].EdgesCell2D[it2->first][m]][0]);
                                }
                            }
                            else
                            {
                                edgSubPol1.push_back(fracture.Polygons[it->first].EdgesCell2D[it2->first][m]);
                                if (m + 1 < szEdg)
                                    verSubPol1.push_back(fracture.Polygons[it->first].VerticesCell2D[it2->first][m+1]);
                            }
                        }

                        //create new vertices and edges ids
                        fracture.Polygons[it->first].VerticesCell2D[idCell2d + 1] = verSubPol1;
                        fracture.Polygons[it->first].EdgesCell2D[idCell2d + 1] = edgSubPol1;
                        fracture.Polygons[it->first].VerticesCell2D[idCell2d + 2] = verSubPol2;
                        fracture.Polygons[it->first].EdgesCell2D[idCell2d + 2] = edgSubPol2;

                        //increase pointer
                        auto currPol = it2;
                        it2 = next(currPol);

                        //delete polygon that has been divided into two subpolygons
                        fracture.Polygons[it->first].EdgesCell2D.erase(currPol->first);
                        fracture.Polygons[it->first].VerticesCell2D.erase(currPol->first);
                        fracture.Polygons[it->first].NumberCell2D--;


                        //increase counters
                        idCoord += 2;
                        idEdg += 5;
                        idCell2d += 2;
                    }
                    else
                        it2 = next(it2);
                }
                else
                    it2 = next(it2);
            }
        }
    }
    return true;
}

bool ExportPolygonalMesh(Fractures& fracture)
{
    //open file Cell0D
    ofstream file0D;
    file0D.open("Cell0D.csv");

    if (file0D.fail())
    {
        cerr << "file open failed" << endl;
        return false;
    }

    //open file Cell1D
    ofstream file1D;
    file1D.open("Cell1D.csv");

    if (file1D.fail())
    {
        cerr << "file open failed" << endl;
        return false;
    }

    //open file Cell2D
    ofstream file2D;
    file2D.open("Cell2D.csv");

    if (file2D.fail())
    {
        cerr << "file open failed" << endl;
        return false;
    }

    //print in file output
    if (size(fracture.Polygons) == 0)
        cout << "There are no polygons" << endl;
    else
    {
        for(map <unsigned int, PolygonalMesh>::iterator it = fracture.Polygons.begin();
             it != fracture.Polygons.end(); it++)
        {
            file0D << "#FractureId" << endl;
            file1D << "#FractureId" << endl;
            file2D << "#FractureId" << endl;
            file0D << it->first << endl;
            file1D << it->first << endl;
            file2D << it->first << endl;

            //file0D
            file0D << "Id,X,Y,Z" << endl;
            for(map <unsigned int, Vector3d>::iterator it0D = fracture.Polygons[it->first].IdCoord0D.begin();
                 it0D != fracture.Polygons[it->first].IdCoord0D.end(); it0D++)
            {
                file0D << it0D->first << ";" << scientific << setprecision(16) <<
                    it0D->second[0] << ";" << it0D->second[1] << ";" << it0D->second[2] << endl;
            }
            file0D << endl;

            //file1D
            file1D << "Id;Origin;End" << endl;
            for(map <unsigned int, Vector2i>::iterator it1D = fracture.Polygons[it->first].IdEdg1D.begin();
                 it1D != fracture.Polygons[it->first].IdEdg1D.end(); it1D++)
            {
                file1D << it1D->first << ";" << it1D->second[0] << ";" << it1D->second[1] << endl;
            }
            file1D << endl;

            //file2D
            file2D << "Id;NumVertices;Vertices;NumEdges;Edges" << endl;
            for(map <unsigned int, vector<unsigned int>>::iterator it2D = fracture.Polygons[it->first].VerticesCell2D.begin();
                 it2D != fracture.Polygons[it->first].VerticesCell2D.end(); it2D++)
            {
                file2D << it2D->first << ";" << size(it2D->second) << ";";
                for (unsigned int i = 0; i < size(it2D->second); i++)
                    file2D << it2D->second[i] << ";";
                file2D << size(it2D->second) << ";";
                for (unsigned int i = 0; i < size(it2D->second); i++)
                    file2D << fracture.Polygons[it->first].EdgesCell2D[it2D->first][i] << ";";
                file2D << endl;
            }
            file2D << endl;
        }

    }
    file0D.close();
    file1D.close();
    file2D.close();

    return true;


}

}


