#pragma once

#include "animation.h"
#include "camera.h"
#include "vector2.h"
#include "platform.h"
#include "playerid.h"

#include "graphics.h"
#include "iostream"
#include <algorithm>

extern bool is_debug;

extern std::vector<Platform> platform_list;
extern std::vector<Bullet*> bullet_list;

class Player
{
public:
    Player()
    {
        current_animation = &animation_idle_right;

        timer_attack_cd.set_wait_time(attack_cd);
        timer_attack_cd.set_one_shot(true);
        timer_attack_cd.set_callback([&]() {
            can_attack = true;
        });

        timer_invulnerable.set_wait_time(750);
        timer_invulnerable.set_one_shot(true);
        timer_invulnerable.set_callback([&]() {
            is_invulnerable = false;
        });

        timer_invulnerable_blink.set_wait_time(75);
        timer_invulnerable_blink.set_callback([&]() {
            is_showing_sketch_frame = !is_showing_sketch_frame;
        });
    }

    ~Player() = default;

    virtual void on_update(int delta)
    {
        int direction = is_right_key_down - is_left_key_down;

        if (direction != 0) {
            if (!is_attacking_ex)
                is_facing_right = direction > 0;

//            is_facing_right = direction > 0;
            current_animation = is_facing_right ? &animation_run_right : &animation_run_left;
            float distance = direction * run_velocity * delta;
            on_run(distance);
        } else {
            current_animation = is_facing_right ? &animation_idle_right : &animation_idle_left;
        }

        if (is_attacking_ex)
            current_animation = is_facing_right ? &animation_attack_ex_right : &animation_attack_ex_left;

        current_animation->on_update(delta);

        timer_attack_cd.on_update(delta);
        timer_invulnerable.on_update(delta);
        timer_invulnerable_blink.on_update(delta);

        if (is_showing_sketch_frame)
            sketch_image(current_animation->get_frame(), &img_sketch);

        move_and_collide(delta);
    }

    virtual void on_draw(const Camera& camera)
    {
        if (hp > 0 && is_invulnerable && is_showing_sketch_frame)
            putimage_alpha(camera, (int)position.x, (int)position.y, &img_sketch);
        else
            current_animation->on_draw(camera, (int)position.x, (int)position.y);

        if (is_debug)
        {
            setlinecolor(RGB(0, 125, 255));
            rectangle((int)position.x, (int)position.y, (int)(position.x + size.x), (int)(position.y + size.y));
        }
    }

    virtual void on_input(const ExMessage& msg)
    {
        switch (msg.message) {
            case WM_KEYDOWN:
                switch (id)
                {
                    case PlayerID::P1:
                        switch (msg.vkcode)
                        {
                            case 0x41:
                                is_left_key_down = true;
                                break;
                            case 0x44:
                                is_right_key_down = true;
                                break;
                            case 0x57:
                                on_jump();
                                break;
                            case 0x46:
                                if (can_attack)
                                {
                                    on_attack();
                                    can_attack = false;
                                    timer_attack_cd.restart();
                                }
                                break;
                            case 0x47:
                                if (mp >= 100)
                                {
                                    on_attack_ex();
                                    mp = 0;
                                }
                                break;
                        }
                        break;
                    case PlayerID::P2:
                        switch (msg.vkcode)
                        {
                            case VK_LEFT:
                                is_left_key_down = true;
                                break;
                            case VK_RIGHT:
                                is_right_key_down = true;
                                break;
                            case VK_UP:
                                on_jump();
                                break;
                            case VK_OEM_PERIOD:
                                if (can_attack)
                                {
                                    on_attack();
                                    can_attack = false;
                                    timer_attack_cd.restart();
                                }
                                break;
                            case VK_OEM_2:
                                if (mp >= 100)
                                {
                                    on_attack_ex();
                                    mp = 0;
                                }
                                break;
                        }
                        break;;
                    default:
                        break;
                }
                break;
            case WM_KEYUP:
                switch (id)
                {
                    case PlayerID::P1:
                        switch (msg.vkcode)
                        {
                            case 0x41:
                                is_left_key_down = false;
                                break;
                            case 0x44:
                                is_right_key_down = false;
                                break;
                        }
                        break;
                    case PlayerID::P2:
                        switch (msg.vkcode)
                        {
                            case VK_LEFT:
                                is_left_key_down = false;
                                break;
                            case VK_RIGHT:
                                is_right_key_down = false;
                                break;
                        }
                        break;;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    virtual void on_run(float distance)
    {
        if (is_attacking_ex)
            return;

        position.x += distance;
    }

    virtual void on_jump()
    {
        if (velocity.y != 0 || is_attacking_ex)
            return ;

        velocity.y += jump_velocity;
    }

    void set_id(PlayerID id)
    {
        this->id = id;
    }

    void set_position(float x, float y)
    {
        position.x = x;
        position.y = y;
    }

    void make_invulnerable()
    {
        is_invulnerable = true;
        timer_invulnerable.restart();
    }

    const Vector2& get_position() const
    {
        return position;
    }

    const Vector2& get_size() const
    {
        return size;
    }

    virtual void on_attack()
    {

    }

    virtual void on_attack_ex()
    {

    }

protected:
    void move_and_collide(int delta)
    {
        velocity.y += gravity * delta;
        position += velocity * (float)delta;

        if (velocity.y > 0)
        {
            for (const Platform& platform : platform_list)
            {
//               这里有关碰撞的监测还时有点不是很明白的的，后续可以回顾下：
//               https://www.bilibili.com/video/BV1rx4y1671B
//               具体为11集中的7分钟左右的位置
                const Platform::CollisionShape& shape = platform.shape;
                bool is_collide_x = (std::max(position.x + size.x, shape.right) - std::min(position.x, shape.left)
                        <= size.x + (shape.right -shape.left));
                bool is_collide_y = (shape.y >= position.y && shape.y <= position.y + size.y);

                if (is_collide_x && is_collide_y)
                {
                    float delta_pos_y = velocity.y * delta;
                    float last_tick_foot_pos_y = position.y + size.y - delta_pos_y;
                    if (last_tick_foot_pos_y <= shape.y)
                    {
                        position.y = shape.y - size.y;
                        velocity.y = 0;

                        break;
                    }
                }
            }
        }

        if (!is_invulnerable)
        {
            for (Bullet* bullet : bullet_list)
            {
                if (!bullet->get_valid() || bullet->get_collide_target() != id)
                    continue;

                if (bullet->check_collision(position, size))
                {
                    make_invulnerable();
                    bullet->on_collide();
                    bullet->set_valid(false);
                    hp -= bullet->get_damage();
                }
            }
        }
    }

protected:
    const float gravity = 1.6e-3f;
    const float run_velocity = 0.55f;
    const float jump_velocity = -0.85f;

protected:
    int hp = 100;
    int mp = 100;

    Vector2 size;
    Vector2 position;
    Vector2 velocity;

    Animation animation_idle_left;
    Animation animation_idle_right;
    Animation animation_run_left;
    Animation animation_run_right;
    Animation animation_attack_ex_left;
    Animation animation_attack_ex_right;

    Animation* current_animation = nullptr;

    PlayerID id = PlayerID::P1;

    bool is_left_key_down = false;
    bool is_right_key_down = false;

    bool is_facing_right = true;

    int attack_cd = 500;
    bool can_attack = true;
    Timer timer_attack_cd;

    bool is_attacking_ex = false;

    IMAGE img_sketch;
    bool is_invulnerable = false;
    bool is_showing_sketch_frame = false;
    Timer timer_invulnerable;
    Timer timer_invulnerable_blink;
};