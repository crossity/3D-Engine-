#include "engine.h"

using namespace std;

unsigned char int_to_uint8(int value) {
    unsigned char ret = value;
    return ret;
}

engine::engine() {
    window = SDL_CreateWindow("3D Eingine", 20, 20, Width, Height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    run = true;
    bg_color = vec3(100, 100, 210);

    light_direction = norm(vec3(0.f, 0.f, -1.f));

    camera = vec3();
    camera_angle = vec3();
}

engine::~engine() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void engine::hello_world() {
    cout << "Hello, World" << endl;
}

void engine::update(mesh &global_mesh) {
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch(event.type) {
            case SDL_QUIT:
                run = false;
                break;
            default:
                break;
        }
    }

    heapSort(global_mesh.t, global_mesh.t.size());

    for (int i = 0; i < global_mesh.t.size(); i++) {
        vec3 color = global_mesh.t[i].color;
        triangle trans_triangle = to_camera(global_mesh.t[i]);
        vec3 n = norm(normal(trans_triangle));
        vec3 camera_ray = trans_triangle.p[0] - camera;
        if (dot_product(n, camera_ray) < 0.f) {
            float dp = min(max(0.1f, dot_product(light_direction, n)), 1.f);

            color = color * dp;

            int_vec2 points[3];
            vec2 uv[3];
            for (int j = 0; j < 3; j++) {
                points[j] = render(trans_triangle.p[j]);
                uv[j] = vec2(global_mesh.t[i].p[j].u, global_mesh.t[i].p[j].v);
            }
            if (global_mesh.t[i].textured) {
                draw_textured_triangle(points, uv, *global_mesh.t[i].texture, 100, 100, color);
                //global_mesh.t[i].p[0].print();
            }
            else
                draw_triangle(points, color);
        }
    }

    global_mesh.t.clear();

}

engine::int_vec2 engine::render(vec3 p) {
    if (p.z == 0) {
        p.z = 0.0001f;
    }
    return engine::int_vec2(Width * (p.x / aspect + 0.75f * p.z) / (1.5f * p.z), Height * (0.75f * p.z - p.y) / (1.5f * p.z));
}

void engine::z_clip(mesh &output, triangle t) {
  short clip_points = 0;
  int cliped_points[3];
  triangle trans_triangle = to_camera(t);
  for (int i = 0; i < 3; i++) {
    if (trans_triangle.p[i].z < FarPlane) {
      cliped_points[clip_points] = i;
      clip_points++;
    }
  }
  if (clip_points == 0)
    output.t.push_back(t);
  else if (clip_points == 1) {
    vec3 p1, p2;
    triangle ret1, ret2;
    float kx1, ky1, kx2, ky2;
    float bx1, by1, bx2, by2;
    kx1 = (t.p[cliped_points[0]].x - t.p[(cliped_points[0] + 1) % 3].x)/(t.p[cliped_points[0]].z - t.p[(cliped_points[0] + 1) % 3].z);
    bx1 = t.p[cliped_points[0]].x - kx1 * t.p[cliped_points[0]].z;
    ky1 = (t.p[cliped_points[0]].y - t.p[(cliped_points[0] + 1) % 3].y)/(t.p[cliped_points[0]].z - t.p[(cliped_points[0] + 1) % 3].z);
    by1 = t.p[cliped_points[0]].y - ky1 * t.p[cliped_points[0]].z;
    p1.z = FarPlane + camera.z;
    p1.x = kx1 * p1.z + bx1;
    p1.y = ky1 * p1.z + by1;

    kx2 = (t.p[cliped_points[0]].x - t.p[(cliped_points[0] + 2) % 3].x)/(t.p[cliped_points[0]].z - t.p[(cliped_points[0] + 2) % 3].z);
    bx2 = t.p[cliped_points[0]].x - kx2 * t.p[cliped_points[0]].z;
    ky2 = (t.p[cliped_points[0]].y - t.p[(cliped_points[0] + 2) % 3].y)/(t.p[cliped_points[0]].z - t.p[(cliped_points[0] + 2) % 3].z);
    by2 = t.p[cliped_points[0]].y - ky2 * t.p[cliped_points[0]].z;
    p2.z = FarPlane + camera.z;
    p2.x = kx2 * p2.z + bx2;
    p2.y = ky2 * p2.z + by2;

    output.t.push_back(triangle(p1, t.p[(cliped_points[0] + 1) % 3], t.p[(cliped_points[0] + 2) % 3], t.texture));
    output.t.push_back(triangle(p1, t.p[(cliped_points[0] + 2) % 3], p2, t.texture));
    output.t[output.t.size() - 1].textured = false;
    output.t[output.t.size() - 2].textured = false;
    output.t[output.t.size() - 1].color = vec3(255, 255, 255);
    output.t[output.t.size() - 2].color = vec3(255, 255, 255);
  }
  else if (clip_points == 2) {
    vec3 p1, p2;
    vec3 p3 = t.p[3 - cliped_points[0] - cliped_points[1]];
    
    triangle ret;

    float kx1, ky1, kx2, ky2;
    float bx1, by1, bx2, by2;

    kx1 = (t.p[cliped_points[0]].x - p3.x)/(t.p[cliped_points[0]].z - p3.z);
    bx1 = p3.x - kx1 * p3.z;
    ky1 = (t.p[cliped_points[0]].y - p3.y)/(t.p[cliped_points[0]].z - p3.z);
    by1 = p3.y - ky1 * p3.z;

    p1.z = FarPlane + camera.z;
    p1.x = kx1 * p1.z + bx1;
    p1.y = ky1 * p1.z + by1;

    kx2 = (t.p[cliped_points[1]].x - p3.x)/(t.p[cliped_points[1]].z - p3.z);
    bx2 = p3.x - kx2 * p3.z;
    ky2 = (t.p[cliped_points[1]].y - p3.y)/(t.p[cliped_points[1]].z - p3.z);
    by2 = p3.y - ky2 * p3.z;

    p2.z = FarPlane + camera.z;
    p2.x = kx2 * p2.z + bx2;
    p2.y = ky2 * p2.z + by2;

    ret.textured = false;
    ret.color = vec3(255, 255, 255);

    ret.p[3 - cliped_points[0] - cliped_points[1]] = p3;
    ret.p[cliped_points[0]] = p1;
    ret.p[cliped_points[1]] = p2;

    output.t.push_back(ret);
  }
  // else
  //   output.push_back(t);
}

void engine::draw_triangle(int_vec2 p[3], vec3 color) {
    draw_triangle(p[0], p[1], p[2], color);
}

void engine::draw_triangle(int_vec2 a, int_vec2 b, int_vec2 c, vec3 color) {
    const std::vector< SDL_Vertex > verts =
    {
        { SDL_FPoint{ (float)a.x, (float)a.y }, SDL_Color{ int_to_uint8(color.x), int_to_uint8(color.y), int_to_uint8(color.z), 255 }, },
        { SDL_FPoint{ (float)b.x, (float)b.y }, SDL_Color{ int_to_uint8(color.x), int_to_uint8(color.y), int_to_uint8(color.z), 255 }, },
        { SDL_FPoint{ (float)c.x, (float)c.y }, SDL_Color{ int_to_uint8(color.x), int_to_uint8(color.y), int_to_uint8(color.z), 255 }, },
    };
    SDL_RenderGeometry( renderer, nullptr, verts.data(), verts.size(), nullptr, 0 );
}

void heapify(vector<engine::triangle> &t, int N, int i)
{
    // Find largest among root, left child and right child
 
    // Initialize largest as root
    int largest = i;
 
    // left = 2*i + 1
    int left = 2 * i + 1;
 
    // right = 2*i + 2
    int right = 2 * i + 2;
 
    // If left child is larger than root
    if (left < N && t[left].center().z < t[largest].center().z)
 
        largest = left;
 
    // If right child is larger than largest
    // so far
    if (right < N && t[right].center().z < t[largest].center().z)
 
        largest = right;
 
    // Swap and continue heapifying if root is not largest
    // If largest is not root
    if (largest != i) {
 
        swap(t[i], t[largest]);
 
        // Recursively heapify the affected
        // sub-tree
        heapify(t, N, largest);
    }
}
 
// Main function to do heap sort
void engine::heapSort(vector<triangle> &t, int N)
{
 
    // Build max heap
    for (int i = N / 2 - 1; i >= 0; i--)
 
        heapify(t, N, i);
 
    // Heap sort
    for (int i = N - 1; i >= 0; i--) {
 
        swap(t[0], t[i]);
 
        // Heapify root element to get highest element at
        // root again
        heapify(t, i, 0);
    }
}

engine::vec3 engine::normal(triangle t) {
    vec3 u, v;
    u = t.p[1] - t.p[0];
    v = t.p[2] - t.p[0];

    vec3 n;
    n.x = u.y * v.z - u.z * v.y;
    n.y = u.z * v.x - u.x * v.z;
    n.z = u.x * v.y - u.y * v.x;

    return n;
}

float engine::dot_product(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float length(engine::vec3 a, engine::vec3 b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + 
                (a.y - b.y)*(a.y - b.y) +
                (a.z - b.z)*(a.z - b.z));
}

engine::vec3 engine::norm(vec3 a) {
    return a / length(vec3(), a);
}

engine::vec3 engine::rotate_y(vec3 c, float angle, vec3 p) {
    float sn = sin(angle);
    float cs = cos(angle);

    p.x -= c.x;
    p.z -= c.z;

    float xnew = p.x * cs - p.z * sn;
    float znew = p.x * sn + p.z * cs;

    p.x = xnew + c.x;
    p.z = znew + c.z;
    return p;
}

engine::triangle engine::to_camera(triangle t) {
    for (int i = 0; i < 3; i++) {
        t.p[i] = rotate_y(camera, -camera_angle.y, t.p[i]);
        t.p[i] = t.p[i] - camera;
    }
    return t;
}

engine::vec3 engine::to_camera(vec3 p) {
    p = rotate_y(camera, -camera_angle.y, p);
    p = p - camera;
    return p;
}

engine::vec3 engine::far(triangle t) {
    int id = 0;
    triangle trans_triangle = to_camera(t);
    for (int i = 1; i < 3; i++)
        if (trans_triangle.p[i].z > trans_triangle.p[id].z)
            id = i;
    return t.p[id];
}

bool engine::mesh::LoadFromObjectFile(string sFilename, bool Textured)
{
    ifstream f(sFilename);
    if (!f.is_open())
        return false;

    // Local cache of verts
    vector<vec3> verts;
    vector<vec2> texts;

    string line;
    char junk;

    while (getline(f, line)) {
        strstream s;
        s << line;
        if (line[0] == 'v') {
            if (line[1] == 't') {
                s >> junk >> junk;
                float uv[2];
                for (int i = 0; i < 2; i++)
                    s >> uv[i];
                texts.push_back(vec2(uv[0], 1 - uv[1]));
            }
            else {
                float pos[3];
                s >> junk;
                for (int i = 0; i < 3; i++)
                    s >> pos[i];
                verts.push_back(vec3(pos[0], pos[1], pos[2]));
            }
        }
        else if (line[0] == 'f') {
            if (Textured) {
                s >> junk;
                int face[3];
                int uvs[3];
                for (int i = 0; i < 3; i++)
                    s >> face[i] >> junk >> uvs[i];
                t.push_back(triangle(
                    vec3(verts[face[0] - 1], texts[uvs[0] - 1].x, texts[uvs[0] - 1].y),
                    vec3(verts[face[1] - 1], texts[uvs[1] - 1].x, texts[uvs[1] - 1].y),
                    vec3(verts[face[2] - 1], texts[uvs[2] - 1].x, texts[uvs[2] - 1].y)
                    ));
            }
            else {
                s >> junk;
                int face[3];
                for (int i = 0; i < 3; i++)
                    s >> face[i];
                t.push_back(triangle(verts[face[0] - 1], verts[face[1] - 1], verts[face[2] - 1]));
            }
        }
    }

    return true;
}

void engine::mesh::LoadFromPNG(SDL_Renderer *renderer, string file) {
    texture = NULL;
    SDL_Surface *surface = NULL;
    surface = IMG_Load(file.c_str());
    if (surface == NULL)
        cout << "surface Error" << endl;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
        cout << "texture Error" << endl;
    textured = true;
    for (int i = 0; i < t.size(); i++) {
        t[i].textured = true;
        t[i].texture = &texture;
    }
    SDL_FreeSurface(surface);
}

void engine::draw_texture(SDL_Texture *texture, int width, int height) {
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = width;
    rect.h = height;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void engine::draw_textured_triangle(int_vec2 p[3], vec2 uv[3], SDL_Texture *texture, int w, int h, vec3 color) {
    SDL_Vertex verts[3];
    SDL_Color c{int_to_uint8(color.x), int_to_uint8(color.y), int_to_uint8(color.z), 255};

    verts[0].position.x = p[0].x;
    verts[0].position.y = p[0].y;
    verts[0].tex_coord.x = (uv[0].x);
    verts[0].tex_coord.y = (uv[0].y);
    verts[0].color = c;

    verts[1].position.x = p[1].x;
    verts[1].position.y = p[1].y;
    verts[1].tex_coord.x = (uv[1].x);
    verts[1].tex_coord.y = (uv[1].y);
    verts[1].color = c;

    verts[2].position.x = p[2].x;
    verts[2].position.y = p[2].y;
    verts[2].tex_coord.x = (uv[2].x);
    verts[2].tex_coord.y = (uv[2].y);
    verts[2].color = c;
    //draw_texture(texture, 100, 100);
    SDL_RenderGeometry(renderer, texture, verts, 3, NULL, 0);
}