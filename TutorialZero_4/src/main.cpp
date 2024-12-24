#include "util.h"
#include "resources_manager.h"
#include "collision_manager.h"
#include "character_manager.h"
#include "bullet_time_manager.h"

#include <chrono>
#include <thread>
#include <graphics.h>
// TODO: For debug, remove this line later
#include <iostream>

static void draw_background()
{
    static IMAGE* img_background = ResourcesManager::instance()->find_image("background");
    static Rect rect_dst =
    {
        (getwidth() - img_background->getwidth()) / 2,
        (getheight() - img_background->getheight()) / 2,
        img_background->getwidth(),
        img_background->getheight()
    };

    putimage_ex(img_background, &rect_dst);
}

static void draw_remain_hp()
{
    static IMAGE* img_ui_heart = ResourcesManager::instance()->find_image("ui_heart");
    Rect rect_dst = { 0, 10, img_ui_heart->getwidth(), img_ui_heart->getheight() };
    for (int i = 0; i < CharacterManager::instance()->get_player()->get_hp(); i++)
    {
        rect_dst.x = 10 + i * 40;
        putimage_ex(img_ui_heart, &rect_dst);
    }
}

int main(int argc, char** argv)
{
    using namespace std::chrono;

    HWND hwnd = initgraph(1280, 720, EW_SHOWCONSOLE);
    SetWindowText(hwnd, _T("Hollow Katana"));

    try
    {
        ResourcesManager::instance()->load();
    }
    catch (const LPCSTR id)
    {
        TCHAR err_msg[512];
        _stprintf_s(err_msg, _T("cannot load:  %s"), id);
        MessageBox(hwnd, err_msg, _T("error:"), MB_OK | MB_ICONERROR);
        return -1;
    }

    play_audio(_T("bgm"), true);

    const nanoseconds frame_duration(1000000000 / 144);
    steady_clock::time_point last_tick = steady_clock::now();

    ExMessage msg;
    bool is_quit = false;

    BeginBatchDraw();

    while (!is_quit)
    {
        while (peekmessage(&msg))
        {
            CharacterManager::instance()->on_input(msg);
        }

        steady_clock::time_point frame_start = steady_clock::now();
        duration<float> delta = duration<float>(frame_start - last_tick);

        float scaled_delta = BulletTimeManager::instance()->on_update(delta.count());
        CharacterManager::instance()->on_update(scaled_delta);
//        CharacterManager::instance()->on_update(delta.count());
        CollisionManager::instance()->process_collide();

        setbkcolor(RGB(0, 0, 0));
        cleardevice();

        draw_background();
        CharacterManager::instance()->on_render();
        CollisionManager::instance()->on_debug_render();
        draw_remain_hp();

        FlushBatchDraw();

        last_tick = frame_start;
        nanoseconds sleep_duration = frame_duration - (steady_clock::now() - frame_start);
        if (sleep_duration > nanoseconds(0))
            std::this_thread::sleep_for(sleep_duration);
    }

    EndBatchDraw();

    return 0;
}