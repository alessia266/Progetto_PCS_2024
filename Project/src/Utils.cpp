#include "DFN.hpp"
#include "SortingAlgorithm.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

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
                         Vector2d& parameter)
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

        }

    }

    if (size(coordIntersection) == 2)
    {
        double p1 = (coordIntersection[0][ind_v] - pointProjection[ind_v])/tanProjection[ind_v];
        double p2 = (coordIntersection[1][ind_v] - pointProjection[ind_v])/tanProjection[ind_v];
        parameter = {min(p1, p2), max(p1, p2)};
    }
    if (size(coordIntersection) > 2)
        cerr << "There are more than two intersections in a fracture" << endl;

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

        vector<Vector3d>& fracture1 = fracture.IDFracture[ID1]; //fracture's coord
        vector<Vector3d>& fracture2 = fracture.IDFracture[ID2]; //fracture's coord

        //normal plane
        Vector3d norm1 = {0, 0, 0};
        Vector3d norm2 = {0, 0, 0};
        Vector3d point1 = fracture1[1];
        Vector3d point2 = fracture2[1];

        unsigned int k = 2;
        while (norm1.norm() < e && k < size(fracture1))
        {
            norm1 = (fracture1[1] - fracture1[0]).cross(fracture1[k] - fracture1[0]); //vector product
            k++;
        }
        if (k == size(fracture1))
        {
            cout << "Fracture " << ID1 << " is degenerate";
        }

        k = 2;
        while (norm2.norm() < e && k < size(fracture2))
        {
            norm2 = (fracture2[1] - fracture2[0]).cross(fracture2[k] - fracture2[0]); //vector product
            k++;
        }
        if (k == size(fracture2))
        {
            cout << "Fracture " << ID2 << " is degenerate";
        }

        //direction of the intersection line between the two plane
        Vector3d t = norm1.cross(norm2);

        unsigned int ptrMin1;
        unsigned int ptrMin2;
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


                //projection fracture on axes
                //find the dimension to ignore (ptrMax1, ptrMax2)
                ptrMin1 = 0;
                ptrMin2 = 0;
                vector<Vector3d> unitvector = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
                double valMin1 = norm1.cross(unitvector[0]).norm();
                double valMin2 = norm2.cross(unitvector[0]).norm();
                for (unsigned int j = 1; j < 3; j++)
                {
                    double tmp = norm1.cross(unitvector[j]).norm();
                    if (tmp < valMin1)
                    {
                        valMin1 = tmp;
                        ptrMin1 = j;
                    }
                    tmp = norm2.cross(unitvector[j]).norm();
                    if (tmp < valMin2)
                    {
                        valMin2 = tmp;
                        ptrMin2 = j;
                    }
                }


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


                for(unsigned int j = 0; j < 3; j++)
                {
                    if (j!=ptrMin1)
                    {
                        for (unsigned int k=0; k < size(fracture1); k++)
                            fractProjection1[k][index1] = fracture1[k][j];
                        tanProjection1(index1) = t[j];
                        pointProjection1(index1) = intersectionPoint[j];
                        index1++;
                    }
                    if (j!=ptrMin2)
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
                                    parFract1);




                //intersection between t and fracture ID2
                vector<Vector2d> intFract2 = {};
                SegmentIntersection(fractProjection2,
                                    tanProjection2,
                                    pointProjection2,
                                    e,
                                    intFract2,
                                    parFract2);


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

                        fracture.IntersectionsFracture[ID1].insert({traceID, {{startIntersection, endIntersection}, Tips, moduleTrace}});


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

                        fracture.IntersectionsFracture[ID2].insert({traceID, {{startIntersection, endIntersection}, Tips, moduleTrace}});

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
        for(std::map <unsigned int, map<unsigned int, Intersections>>::iterator it1 = fracture.IntersectionsFracture.begin();
             it1 != fracture.IntersectionsFracture.end(); ++it1)
        {
            file << "# FractureId; NumTraces" << endl;
            file << it1->first << "; " << size(it1->second) << endl;
            file << "# TraceId; Tips; Length" << endl;

            //vector of struct (contains informations to order and print)
            vector<SortingTraces> sortingTrace = {};
            for(std::map<unsigned int, Intersections>::iterator it2 = it1->second.begin();
                 it2 != it1->second.end(); ++it2)
            {
                sortingTrace.push_back({it2->first, it2->second.Tips, it2->second.Length});
            }

            //sorting
            SortLibrary::BubbleSort(sortingTrace);

            fracture.FractureSortTraces[it1->first] = sortingTrace;

            for (unsigned int i = size(sortingTrace); i > 0; i--)
                file << sortingTrace[i-1].TraceID << "; " << sortingTrace[i-1].Tips << "; " << scientific << setprecision(16) << sortingTrace[i-1].Length << endl;
        }

    file.close();

    return true;
}


}


