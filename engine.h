#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <strstream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define Height 500
#define Width  500

#define FarPlane 0.01f

float aspect = (float)Width / (float)Height;

class engine {
public:
    struct vec2 {
        float x, y;
        vec2() {
            this->x = 0.f;
            this->y = 0.f;
        }
        vec2(float a, float b) {
            this->x = a;
            this->y = b;
        }
        void print() {
            std::cout << this->x << ", " << this->y << std::endl;
        }
    };
    struct int_vec2 {
        int x, y;
        int_vec2() {
            this->x = 0;
            this->y = 0;
        }
        int_vec2(int a, int b) {
            this->x = a;
            this->y = b;
        }
    };
    struct vec3 {
        float x, y, z;
        float u, v;
        vec3() {
            this->x = 0.f;
            this->y = 0.f;
            this->z = 0.f;
        }
        vec3(float a, float b, float c) {
            this->x = a;
            this->y = b;
            this->z = c;
        }
        vec3(float a, float b, float c, float d, float e) {
            this->x = a;
            this->y = b;
            this->z = c;
            this->u = d;
            this->v = e;
        }
        vec3(vec3 a, float b, float c) {
            this->x = a.x;
            this->y = a.y;
            this->z = a.z;
            this->u = b;
            this->v = c;
        }
        void print() {
            std::cout << this->x << ", " << this->y << ", " << this->z << "(" << this->u << ", " << this->v << ")" << std::endl;
        }
        vec3 operator-(vec3 a) {
            return vec3(this->x - a.x, this->y - a.y, this->z - a.z);
        }
        vec3 operator-(float a) {
            return vec3(this->x - a, this->y - a, this->z - a);
        }
        vec3 operator/(float a) {
            return vec3(this->x / a, this->y / a, this->z / a);
        }
        vec3 operator*(float a) {
            return vec3(this->x * a, this->y * a, this->z * a);
        }
    };
    struct triangle {
        vec3 p[3];
        vec3 color;
        bool textured = false;
        SDL_Texture **texture;
        triangle() {
            this->p[0] = vec3();
            this->p[1] = vec3();
            this->p[2] = vec3();
            this->color = vec3();
        }
        triangle(vec3 a, vec3 b, vec3 c, vec3 d) {
            this->p[0] = a;
            this->p[1] = b;
            this->p[2] = c;
            this->color = d;
        }
        triangle(vec3 a, vec3 b, vec3 c) {
            this->p[0] = a;
            this->p[1] = b;
            this->p[2] = c;
            this->color = vec3();
        }
        triangle(vec3 a, vec3 b, vec3 c, SDL_Texture **d) {
            this->p[0] = a;
            this->p[1] = b;
            this->p[2] = c;
            this->texture = d;
        }
        vec3 center() {
            return vec3((this->p[0].x + this->p[1].x + this->p[2].x) / 3.f, 
                        (this->p[0].y + this->p[1].y + this->p[2].y) / 3.f,
                        (this->p[0].z + this->p[1].z + this->p[2].z) / 3.f);
        }
    };
    struct mesh {
        std::vector<triangle> t;
        SDL_Texture *texture;
        bool textured = false;
        bool LoadFromObjectFile(std::string sFilename, bool Textured = false);
        void LoadFromPNG(SDL_Renderer *renderer, std::string file);
        void colorize(vec3 color) { 
            for (int i = 0; i < t.size(); i++) {
                t[i].color = color;
            }
        }
    };
public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool run;
    vec3 bg_color;

    vec3 light_direction;

    vec3 camera;
    vec3 camera_angle;

public:
    engine();
    ~engine();

    void hello_world();
    void update(mesh &global_mesh);

    int_vec2 render(vec3 p);
    void z_clip(mesh &output, triangle t);

    void draw_triangle(int_vec2 p[3], vec3 color);
    void draw_triangle(int_vec2 a, int_vec2 b, int_vec2 c, vec3 color);
    void draw_textured_triangle(int_vec2 p[3], vec2 uv[3], SDL_Texture *texture, int w, int h, vec3 color);

    void heapSort(std::vector<triangle> &t, int N);

    vec3 normal(triangle t);
    float dot_product(vec3 a, vec3 b);
    vec3 norm(vec3 a);

    vec3 rotate_y(vec3 c, float angle, vec3 p);

    triangle to_camera(triangle t);
    vec3 to_camera(vec3 p);

    vec3 far(triangle t);

    void draw_texture(SDL_Texture *texture, int width, int height);
};