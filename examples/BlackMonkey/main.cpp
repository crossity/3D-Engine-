#include "engine.cpp"

using e = engine;

engine en;

e::mesh main_mesh;

void UI() {
}

int main() {
    e::mesh global_mesh;

    en.camera.z -= 4.f;

    main_mesh.LoadFromObjectFile("monkey.obj", true);
    main_mesh.LoadFromPNG(en.renderer, "monkey.png");
    main_mesh.colorize(e::vec3(255, 255, 255));

    long long tick = SDL_GetTicks();
    float delta = 1;

    while (en.run) {
        SDL_SetRenderDrawColor(en.renderer, en.bg_color.x, en.bg_color.y, en.bg_color.z, 0);
        SDL_RenderClear(en.renderer);
    
        for (int i = 0; i < main_mesh.t.size(); i++) {
            en.z_clip(global_mesh, main_mesh.t[i]);
            for (int j = 0; j < 3; j++)
                main_mesh.t[i].p[j] = en.rotate_y(e::vec3(0.f, 0.f, 0.f), 0.001f * delta, main_mesh.t[i].p[j]);
        }
        //global_mesh = main_mesh;
        en.update(global_mesh);
        UI();

        SDL_RenderPresent(en.renderer);

        delta = SDL_GetTicks() - tick;
        tick = SDL_GetTicks();
    }
    return 0;
}