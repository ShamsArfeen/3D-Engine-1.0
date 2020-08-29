#include <iostream>
#include <math.h>
#include "3d_matrix.hpp"

using namespace std;

matrix::matrix()
{
    cout << "[*] Initiate MAT" << endl;
}

void matrix::clear_mat()
{
    cout << "[*] Empty MAT" << endl;
    point_num = 0;
    polygon_num = 0;
    x_rotation = 0;
    y_rotation = 0;
    eye_z = 0;
    screen_z = new float *[800];
    for (int i = 0; i < 800; i++)
    {
        screen_z[i] = new float[800];
    }
    screen_p = new int *[800];
    for (int i = 0; i < 800; i++)
    {
        screen_p[i] = new int[800];
    }
}

void matrix::freeze_mat(float x, float y)
{
    x_rotation += x;
    y_rotation += y;
}

void matrix::rotate_mat(float x, float y)
{

    float ptx, pty, ptz;
    float ini_x, ini_y;
    float hypotenuse, k;
    for (int i = 0; i < point_num; i++)
    {
        ptx = i_point[i].x;
        pty = i_point[i].y;
        ptz = i_point[i].z;

        ini_x = atan2(ptz, ptx) - x_rotation - x;
        hypotenuse = sqrt(ptx * ptx + ptz * ptz);
        ptx = cos(ini_x) * hypotenuse;
        ptz = sin(ini_x) * hypotenuse;

        ini_y = atan2(ptz, pty) + y_rotation + y;
        hypotenuse = sqrt(ptz * ptz + pty * pty);
        ptz = sin(ini_y) * hypotenuse;
        pty = cos(ini_y) * hypotenuse;

        r_point[i].x = ptx;
        r_point[i].y = pty;
        r_point[i].z = ptz;

        k = (float)eye_size / (float)(ptz + eye_z);
        screen_pt[i].x = (float)(center_x + k * ptx);
        screen_pt[i].y = (int)(center_y - k * pty);
    }
}

void matrix::render_wireframe(SDL_Renderer *g_renderer)
{
    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 0);
    SDL_RenderClear(g_renderer);
    SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
    int p1, p2, x1, x2, y1, y2;
    for (int i = 3; i < line_num; i++)
    {
        p1 = i_line[i].p1;
        p2 = i_line[i].p2;

        x1 = int(screen_pt[p1].x) + center_x;
        y1 = -int(screen_pt[p1].y) + center_y;

        x2 = int(screen_pt[p2].x) + center_x;
        y2 = -int(screen_pt[p2].y) + center_y;

        SDL_RenderDrawLine(g_renderer, x1, y1, x2, y2);
    }
    //Update screen
    SDL_RenderPresent(g_renderer);
}

matrix::~matrix()
{
    delete i_point;
    delete r_point;
    delete i_line;
    for (int i = 0; i < polygon_num; i++)
        delete i_polygon[i].pt;
    delete i_polygon;
    delete screen_pt;
    for (int i = 0; i < 800; i++)
        delete screen_z[i];
    delete screen_z;
}

void matrix::render_surfaces(SDL_Renderer *gRenderer)
{
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
    SDL_RenderClear(gRenderer);

    for (int i = 0; i < 800; i++)
    {
        for (int j = 0; j < 800; j++)
        {
            screen_z[i][j] = -100000;
        }
    }
    for (int i = 0; i < polygon_num; i++)
    {
        render_polygon(i, gRenderer);
    }
    // update screen
    SDL_RenderPresent(gRenderer);
}

void matrix::get_pgn_constants(int pgn)
{
    int i = 2;
    
    float pt1x, pt1y, pt1z;
    float pt2x, pt2y, pt2z;
    float pt3x, pt3y, pt3z;

    pt1x = r_point[i_polygon[pgn].pt[0]].x;
    pt1y = r_point[i_polygon[pgn].pt[0]].y;
    pt1z = r_point[i_polygon[pgn].pt[0]].z;
    pt2x = r_point[i_polygon[pgn].pt[1]].x;
    pt2y = r_point[i_polygon[pgn].pt[1]].y;
    pt2z = r_point[i_polygon[pgn].pt[1]].z;

    redo:
    pt3x = r_point[i_polygon[pgn].pt[i]].x;
    pt3y = r_point[i_polygon[pgn].pt[i]].y;
    pt3z = r_point[i_polygon[pgn].pt[i]].z;

    pgn_c = pt1x * pt3y - pt3x * pt1y - pt2x * pt3y + pt3x * pt2y + pt2x * pt1y - pt1x * pt2y;
    pgn_b = pt3x * pt1z - pt1x * pt3z + pt2x * pt3z - pt3x * pt2z - pt2x * pt1z + pt1x * pt2z;
    pgn_a = pt3z * pt1y - pt1z * pt3y + pt2z * pt3y - pt3z * pt2y - pt2z * pt1y + pt1z * pt2y;

    if (abs(pgn_a) < 0.000001 && abs(pgn_b) < 0.000001 && abs(pgn_c) < 0.000001 && i+1 != i_polygon[pgn].n)
    {    
        i += 1;
        goto redo;
    }

    pgn_d = pt1x * pt2z * pt3y - pt2z * pt3x * pt1y - pt1z * pt2x * pt3y + pt2y * pt1z * pt3x + pt2x * pt1y * pt3z - pt1x * pt2y * pt3z;
}

float matrix::polygon_pixel(int x, int y)
{
    float line = pgn_a * (x - center_x) + pgn_b * (center_y - y);
    float denominator = line + pgn_c * eye_size;
    float numinator = eye_z * line - eye_size * pgn_d;
    if (denominator == 0)
        return -100000;
    else
        return numinator / denominator;
}

bool matrix::render_polygon(int pgn, SDL_Renderer *g_Renderer)
{
    get_pgn_constants(pgn);

    float flash = sqrt(flash_x * flash_x + flash_y * flash_y + flash_z * flash_z);
    float triangle = sqrt(pgn_a * pgn_a + pgn_b * pgn_b + pgn_c * pgn_c);
    float reflection = abs(flash_x * pgn_a + flash_y * pgn_b + flash_z * pgn_c);
    float shine = abs(asin(reflection / (triangle * flash)));


    int texture = sqrt(20000 * shine);
    SDL_SetRenderDrawColor(g_Renderer, texture, texture, texture, 255);

    int n = i_polygon[pgn].n;
    int y, min_y, max_y, pt;
    float xi[n], yi[n], min_z, max_z, z;

    pt = i_polygon[pgn].pt[0];
    y = screen_pt[pt].y;
    min_y = y;
    max_y = y;
    z = r_point[pt].z;
    min_z = z;
    max_z = z;
    for (int i = 0; i < n; i++)
    {
        pt = i_polygon[pgn].pt[i];
        y = screen_pt[pt].y;
        xi[i] = screen_pt[pt].x;
        yi[i] = y + 0.5;
        if (y > max_y)
            max_y = y;
        if (y < min_y)
            min_y = y;
        z = -r_point[pt].z;
        if (z > max_z)
            max_z = z;
        if (z < min_z)
            min_z = z;
    }
    int len = max_y - min_y + 1;
    int *region[len]; // = new int *[len];
    int region_len[len];
    int region_sp[len];
    for (int i = 0; i < len; i++)
    {
        region_len[i] = 0;
        region_sp[i] = 1;
        region[i] = new int[1];
    }

    float x1, y1, m;
    int x, lim_y, index, reg_len_i;
    float x2 = xi[n - 1];
    float y2 = yi[n - 1];
    int *arr;
    //int *array;
    int i = 0;
repeat:
    x1 = xi[i];
    y1 = yi[i];
    ///////////////
    if (y1 != y2)
    {
        m = (x2 - x1) / (y2 - y1);
        if (y1 > y2)
        {
            y = (int)y1;
            lim_y = (int)y2;
        }
        else
        {
            y = (int)y2;
            lim_y = (int)y1;
        }
    loop:
        /////////////////
        x = (y - y1) * m + x1;
        index = y - min_y;
        reg_len_i = region_len[index];
        arr = region[index];
        if (region_sp[index] == reg_len_i)
        {
            region_sp[index] = 2 * reg_len_i;
            region[index] = new int[2 * reg_len_i];
            for (int j = 0; j < reg_len_i; j++)
                region[index][j] = arr[j];
        }
        region[index][reg_len_i] = x;
        region_len[index] += 1;
        /////////////////
        y--;
        if (y != lim_y)
            goto loop;
    }
    ///////////////
    
    i++;
    if (i < n)
    {
        x2 = x1;
        y2 = y1;
        goto repeat;
    }

    float i_screen_z;
    int no;
    for (int i = 0; i < len; i++)
    {
        arr = region[i];
        no = region_len[i];

        if (no > 0)
        {
            for (int t = no; t > 1; t--)
            {
                bool flag = false;
                for (int m = 1; m < t; m++)
                {
                    if (arr[m-1] > arr[m])
                    {
                        flag = true;
                        int temp = arr[m];
                        arr[m] = arr[m-1];
                        arr[m-1] = temp;
                    }
                }
                if (flag == false)
                    goto sort_done;
            }
        }
    sort_done:
        y = min_y + i;
        for (int j = 0; j < no; j += 2)
        {
            x = arr[j];
        next_x:
            /////////
            i_screen_z = polygon_pixel(x, y);
            if (i_screen_z > max_z)
                i_screen_z = max_z;
            else if (i_screen_z < min_z)
                i_screen_z = min_z;
            if (screen_z[x][y] == -100000 || screen_z[x][y] < i_screen_z)
            {
                SDL_RenderDrawPoint(g_Renderer, x, y);
                screen_z[x][y] = i_screen_z;
                screen_p[x][y] = pgn;
            }
            /////////
            if (x != arr[j + 1])
            {
                x++;
                goto next_x;
            }
        }
        delete region[i];
    }
    return true;
}

void matrix::get_surface(int x, int y)
{
    cout << " ==== GETTING SURFACE < " << screen_p[x][y] << " > ==== " << endl;
}