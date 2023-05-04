#include "engine.cpp"

using e = engine;

engine en;

int fps, t, last_fps;

void UI() {
    en.draw_text("fps: " + to_string(fps), 0, 0, e::vec3(255, 255, 255));
}

int main() {
    en.fog = 1000.f;

    e::mesh global_mesh;
    e::node main_node;
    main_node.add("cube", e::object());
    en.camera.z -= 4.f;

    main_node.objects["cube"].m.LoadFromObjectFile("box.obj", TEXTURED);
    main_node.objects["cube"].m.LoadFromPNG(en.renderer, "block.png");
    main_node.objects["cube"].m.colorize(e::vec3(255, 255, 255));

    main_node.pos = e::vec3(0.f, 0.f, 0.f);
    main_node.rot = e::vec3(0.f, 3.14f / 4.f, 0.f);

    while (en.run) {
        SDL_SetRenderDrawColor(en.renderer, en.bg_color.x, en.bg_color.y, en.bg_color.z, 0);
        SDL_RenderClear(en.renderer);
        //main_node.rot.y += 0.001f * (float)en.delta;
        e::mesh cube_global_mesh = main_node.get_global_mesh();
        for (int i = 0; i < cube_global_mesh.t.size(); i++) {
            en.z_clip(global_mesh, cube_global_mesh.t[i]);
        }

        en.update(global_mesh);
        UI();


        SDL_RenderPresent(en.renderer);

        last_fps++;
        if (t < SDL_GetTicks() - 1000)
        {
           t = SDL_GetTicks();
           fps = last_fps;
           last_fps = 0;
        }
    }
    return 0;
}