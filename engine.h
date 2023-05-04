#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <strstream>
#include <string>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define Height 256
#define Width  224

#define FullScreen false

#define TEXTURED true
#define UNTEXTURED false

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
        vec3 operator+(vec3 a) {
            return vec3(this->x + a.x, this->y + a.y, this->z + a.z);
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
        float sqr_dist(vec3 camera) {
            vec3 cent = center();
            float x, y, z;
            x = cent.x - camera.x;
            y = cent.y - camera.y;
            z = cent.z - camera.z;
            return x*x + y*y + z*z;
        }
        float dist(vec3 camera) {
            return sqrt(sqr_dist(camera));
        }
    };
    struct mesh {
        std::vector<triangle> t;
        SDL_Texture *texture;
        bool LoadFromObjectFile(std::string sFilename, bool Textured = false);
        void LoadFromPNG(SDL_Renderer *renderer, std::string file);
        void colorize(vec3 color) { 
            for (int i = 0; i < t.size(); i++) {
                t[i].color = color;
            }
        }
    };
    struct object {
        std::vector<std::string> groups;
        mesh m;
        vec3 pos;
        vec3 rot;
        object() {
            this->pos = vec3();
            this->rot = vec3();
        }
        object(vec3 a, vec3 b) {
            this->pos = a;
            this->rot = b;
        }
        bool in_group(std::string name) {
            bool in = false;
            for (int i = 0; i < groups.size(); i++)
                if (groups[i] == name) {
                    in = true;
                    break;
                }
            return in;
        }
        mesh get_global_mesh() {
            mesh ret;
            ret.texture = m.texture;
            for (int i = 0; i < m.t.size(); i++) {
                triangle tri;
                tri.color = m.t[i].color;
                tri.texture = m.t[i].texture;
                tri.textured = m.t[i].textured;
                for (int j = 0; j < 3; j++) {
                    tri.p[j] = local_rotate_x(pos, rot.x, m.t[i].p[j]);
                    tri.p[j] = local_rotate_y(pos, rot.y, tri.p[j]);
                    tri.p[j] = local_rotate_z(pos, rot.z, tri.p[j]);
                    tri.p[j] = tri.p[j] + pos;
                    tri.p[j].u = m.t[i].p[j].u;
                    tri.p[j].v = m.t[i].p[j].v;
                }
                ret.t.push_back(tri);
            }
            return ret;
        }
    private:
        vec3 local_rotate_x(vec3 c, float angle, vec3 p) {
            if (angle == 0)
                return p;
            float sn = sin(angle);
            float cs = cos(angle);

            p.z -= c.z;
            p.y -= c.y;

            float znew = p.z * cs - p.y * sn;
            float ynew = p.z * sn + p.y * cs;

            p.y = ynew + c.y;
            p.z = znew + c.z;
            return p;
        }
        vec3 local_rotate_y(vec3 c, float angle, vec3 p) {
            if (angle == 0)
                return p;
            float sn = sin(angle);
            float cs = cos(angle);

            p.z -= c.z;
            p.x -= c.x;

            float znew = p.z * cs - p.x * sn;
            float xnew = p.z * sn + p.x * cs;

            p.x = xnew + c.x;
            p.z = znew + c.z;
            return p;
        }
        vec3 local_rotate_z(vec3 c, float angle, vec3 p) {
            if (angle == 0)
                return p;            
            float sn = sin(angle);
            float cs = cos(angle);

            p.x -= c.x;
            p.y -= c.y;

            float xnew = p.x * cs - p.y * sn;
            float ynew = p.x * sn + p.y * cs;

            p.y = ynew + c.y;
            p.x = xnew + c.x;
            return p;
        }
    };
    struct node {
        std::map<std::string, object> objects;
        std::vector<std::string> object_names;
        vec3 pos, rot;

        mesh get_global_mesh() {
            mesh ret;
            for (int i = 0; i < object_names.size(); i++) {
                object obj = objects[object_names[i]];
                obj.pos = obj.pos + pos;
                obj.rot = obj.rot + rot;
                mesh m = obj.get_global_mesh();
                ret.t.insert(std::end(ret.t), std::begin(m.t), std::end(m.t));
            }
            return ret;
        }
        void add(std::string name, object obj) {
            objects.insert({name, obj});
            object_names.push_back(name);
        }
    };
public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool run;
    vec3 bg_color;
    float fog;

    std::vector<vec3> dir_lights;
    std::vector<vec3> lights;

    vec3 camera;
    vec3 camera_angle;

    TTF_Font *font;

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

    void heapify(std::vector<triangle> &t, int N, int i);
    void heapSort(std::vector<triangle> &t, int N);

    vec3 normal(triangle t);
    float dot_product(vec3 a, vec3 b);
    vec3 norm(vec3 a);

    vec3 rotate_x(vec3 c, float angle, vec3 p);
    vec3 rotate_y(vec3 c, float angle, vec3 p);
    vec3 rotate_z(vec3 c, float angle, vec3 p);

    triangle to_camera(triangle t);
    vec3 to_camera(vec3 p);

    vec3 far(triangle t);

    void draw_texture(SDL_Texture *texture, int width, int height);
    void draw_text(std::string text,int x, int y, vec3 color);

    void LoadFont(std::string path);

    vec3 bland(vec3 col1, vec3 col2, float per);

    float map(float value, float a1, float b1, float a2, float b2);

    int delta;
    long long tick;

private:
    SDL_Rect text_rectangle;
    SDL_Texture *text_texture;
};