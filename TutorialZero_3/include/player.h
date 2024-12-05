#pragma once

#include "animation.h"
#include "camera.h"
#include "vector2.h"
#include "platform.h"
#include "playerid.h"

#include "graphics.h"
#include "iostream"
#include <algorithm>

extern std::vector<Platform> platform_list;

class Player
{
public:
    Player()
    {
        current_animation = &animation_run_right;
    }

    ~Player() = default;

    virtual void on_update(int delta)
    {
        int direction = is_right_key_down - is_left_key_down;

        if (direction != 0) {
            is_facing_right = direction > 0;
            current_animation = is_facing_right ? &animation_run_right : &animation_run_left;
            float distance = direction * run_velocity * delta;
            on_run(distance);
        } else {
            current_animation = is_facing_right ? &animation_idle_right : &animation_idle_left;
        }

        current_animation->on_update(delta);

        move_and_collide(delta);
    }

    virtual void on_draw(const Camera& camera)
    {
        current_animation->on_draw(camera, (int)position.x, (int)position.y);
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
        position.x += distance;
    }

    virtual void on_jump()
    {
        if (velocity.y != 0)
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
    }

protected:
    const float gravity = 1.6e-3f;
    const float run_velocity = 0.55f;
    const float jump_velocity = -0.85f;

protected:
    Vector2 size;
    Vector2 position;
    Vector2 velocity;

    Animation animation_idle_left;
    Animation animation_idle_right;
    Animation animation_run_left;
    Animation animation_run_right;

    Animation* current_animation = nullptr;

    PlayerID id = PlayerID::P1;

    bool is_left_key_down = false;
    bool is_right_key_down = false;

    bool is_facing_right = true;
};