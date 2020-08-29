#include <SDL.h>

const int eye_size = 1500;

const int center_x = 400;
const int center_y = 400;

const int flash_x = 5;
const int flash_y = 5;
const int flash_z = -10;

struct polygon
{
    int n;
    int *pt;
};

struct point_2d
{
    float x;
    int y;
};

struct point_3d
{
    float x, y, z;
};

struct line_3d
{
    int p1, p2;
};

class matrix
{
private:
    float polygon_pixel(int, int);
    int eye_z;
    point_3d *i_point;
    point_3d *r_point;
    point_2d *screen_pt;
    int point_num;
    float **screen_z;
    int **screen_p;
    int line_num;
    line_3d *i_line;
    int polygon_num, hPgnCount, poly_space;
    polygon *i_polygon;
    float pgn_a, pgn_b, pgn_c, pgn_d;
    float y_rotation, x_rotation;
    void get_pgn_constants(int);
    bool render_polygon(int, SDL_Renderer *);

public:
    matrix();
    void rotate_mat(float, float);
    void freeze_mat(float, float);
    void render_wireframe(SDL_Renderer *);
    void render_surfaces(SDL_Renderer *);
    void get_surface(int, int);
    void load_pgn_model();
    bool polish_surface();
    void clear_mat();
    ~matrix();
};