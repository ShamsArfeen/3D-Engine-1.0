#include <iostream>
#include "3d_matrix.hpp"
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

void matrix::load_pgn_model()
{
    ifstream objFile;
    this->clear_mat();
    string inputFile;
    cout << ">> Enter any obj File (e.g. Cessna.obj) : ";
    cin >> inputFile;
    objFile.open(inputFile);
    cout << "[*] Reading file" << endl;
    int pt_no = 1;
    float max_x = 0, min_x = 0, max_y = 0, min_y = 0, max_z = 0, min_z = 0;
    hPgnCount = 0;
    poly_space = 1;
    if (objFile)
    {
        i_point = new point_3d[1];
        i_polygon = new polygon[1];
        cout << "... [*] File Found" << endl;
        string iData, objElm;
        while (!objFile.eof())
        {
            getline(objFile, iData);
            istringstream iSStream(iData);

            iSStream >> objElm;
            if (iSStream.eof())
            {
                iSStream.ignore();
            }
            else if (objElm == "v")
            {
                if (point_num == pt_no)
                {
                    point_3d* temp = i_point;
                    i_point = new point_3d[2 * pt_no];
                    for (int i = 0; i < pt_no; i++)
                    {
                        i_point[i] = temp[i];
                    }
                    pt_no = pt_no * 2;
                }
                float x, y, z;
                while (iSStream.peek() == ' ')
                    iSStream.ignore();
                iSStream >> x;
                while (iSStream.peek() == ' ')
                    iSStream.ignore();
                iSStream >> y;
                while (iSStream.peek() == ' ')
                    iSStream.ignore();
                iSStream >> z;

                if (point_num == 0)
                {
                    max_x = x;
                    min_x = x;
                    max_y = y;
                    min_y = y;
                    max_z = z;
                    min_z = z;
                }

                i_point[point_num].x = x;
                i_point[point_num].y = y;
                i_point[point_num].z = z;
                if (max_x < x)
                    max_x = x;
                if (min_x > x)
                    min_x = x;
                if (max_y < y)
                    max_y = y;
                if (min_y > y)
                    min_y = y;
                if (max_z < z)
                    max_z = z;
                if (min_z > z)
                    min_z = z;
                    
                point_num = point_num + 1;
            }
            else if (objElm == "f")
            {
                if (polygon_num == poly_space)
                {
                    polygon* temp = i_polygon;
                    i_polygon = new polygon[2 * poly_space];
                    for (int i = 0; i < poly_space; i++)
                    {
                        i_polygon[i] = temp[i];
                    }
                    poly_space = poly_space * 2;
                }
                int n = 0, count = 3;
                i_polygon[polygon_num].pt = new int[3];
                while (iSStream.peek() == ' ')
                    iSStream.ignore();
                while (!iSStream.eof())
                {
                    if (count == n)
                    {
                        int* temp = i_polygon[polygon_num].pt;
                        i_polygon[polygon_num].pt = new int[2 * n];
                        for (int i = 0; i < n; i++)
                        {
                            i_polygon[polygon_num].pt[i] = temp[i];
                        }
                        count = count * 2;
                    }
                    int PtIndex;
                    string fParam;
                    iSStream >> fParam;
                    stringstream ss(fParam);
                    ss >> PtIndex;
                    if (PtIndex == 0)
                        goto INVALID_FACE;
                    i_polygon[polygon_num].pt[n] = PtIndex - 1;
                    n = n + 1;
                    while (iSStream.peek() == ' ')
                        iSStream.ignore();
                }
                INVALID_FACE:
                i_polygon[polygon_num].n = n;
                polygon_num = polygon_num + 1;
                if (n > 3)
                    polish_surface();
            }
        }
        r_point = new point_3d[point_num];
        screen_pt = new point_2d[point_num];
        float avg_x = (min_x + max_x) * 0.5;
        float avg_y = (min_y + max_y) * 0.5;
        float avg_z = (min_z + max_z) * 0.5;
        for(int i = 0; i < point_num; i++)
        {
            i_point[i].x = i_point[i].x - avg_x;
            i_point[i].y = i_point[i].y - avg_y;
            i_point[i].z = i_point[i].z - avg_z;
        }
        avg_x = (min_x - max_x) * 0.5;
        avg_y = (min_y - max_y) * 0.5;
        avg_z = (min_z - max_z) * 0.5;
        eye_z = 7 * sqrt( avg_x * avg_x + avg_y * avg_y + avg_z * avg_z );
    }
    cout << "... [*] Read Complete" << endl;
    cout << "[*] Vertex Count : " << point_num << endl;
    cout << "[*] Face Count : " << polygon_num << endl;
    cout << "... [*] Triangle Count : " << polygon_num - hPgnCount << endl;
    cout << "... [*] Polygon Count : " << hPgnCount << endl;
    objFile.close();
}

bool matrix::polish_surface()
{
    int pgn = polygon_num - 1;

    float pt1x, pt1y, pt1z;
    float pt2x, pt2y, pt2z;
    float pt3x, pt3y, pt3z;

    pt1x = i_point[i_polygon[pgn].pt[0]].x;
    pt1y = i_point[i_polygon[pgn].pt[0]].y;
    pt1z = i_point[i_polygon[pgn].pt[0]].z;

    pt2x = i_point[i_polygon[pgn].pt[1]].x;
    pt2y = i_point[i_polygon[pgn].pt[1]].y;
    pt2z = i_point[i_polygon[pgn].pt[1]].z;

    pt3x = i_point[i_polygon[pgn].pt[2]].x;
    pt3y = i_point[i_polygon[pgn].pt[2]].y;
    pt3z = i_point[i_polygon[pgn].pt[2]].z;

    pgn_c = pt1x * pt3y - pt3x * pt1y - pt2x * pt3y + pt3x * pt2y + pt2x * pt1y - pt1x * pt2y;
    pgn_b = pt3x * pt1z - pt1x * pt3z + pt2x * pt3z - pt3x * pt2z - pt2x * pt1z + pt1x * pt2z;
    pgn_a = pt3z * pt1y - pt1z * pt3y + pt2z * pt3y - pt3z * pt2y - pt2z * pt1y + pt1z * pt2y;
    pgn_d = pt1x * pt2z * pt3y - pt2z * pt3x * pt1y - pt1z * pt2x * pt3y + pt2y * pt1z * pt3x + pt2x * pt1y * pt3z - pt1x * pt2y * pt3z;

    //cout << "A = " << pgn_a << ", B = " << pgn_b << ", C = " << pgn_c << ", D = " << pgn_d << endl;
    if (pgn_d != 0)
    {
        pgn_a = pgn_a / pgn_d;
        pgn_b = pgn_b / pgn_d;
        pgn_c = pgn_c / pgn_d;
        pgn_d = 1;
    }
    for (int i = 3; i < i_polygon[pgn].n; i++)
    {
        float x, y, z;

        x = i_point[i_polygon[pgn].pt[i]].x;
        y = i_point[i_polygon[pgn].pt[i]].y;
        z = i_point[i_polygon[pgn].pt[i]].z;

        if (abs(pgn_a * x + pgn_b * y + pgn_c * z - pgn_d) > 0.0001)
            goto split;
    }
    hPgnCount = hPgnCount + 1;
    //cout << "... NO SPLIT" << endl;
    return false;

    split:
    //cout << "... YES SPLIT" << endl;
  
    for (int i = 3; i < i_polygon[pgn].n; i++)
    {
        if (polygon_num == poly_space)
        {
            polygon* temp = i_polygon;
            i_polygon = new polygon[2 * poly_space];
            for (int i = 0; i < poly_space; i++)
            {
                i_polygon[i] = temp[i];
            }
            poly_space = poly_space * 2;
        }
        i_polygon[polygon_num].n = 3;
        i_polygon[polygon_num].pt = new int[3];
        i_polygon[polygon_num].pt[0] = i_polygon[pgn].pt[0];
        i_polygon[polygon_num].pt[1] = i_polygon[pgn].pt[i];
        i_polygon[polygon_num].pt[2] = i_polygon[pgn].pt[i-1];
        polygon_num = polygon_num + 1;
    }
    i_polygon[pgn].n = 3;
    return true;
}
