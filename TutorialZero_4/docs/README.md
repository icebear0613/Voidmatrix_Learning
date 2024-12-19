# Easyx 空洞骑士 -by: Voidmatrix

## Chapter 1： 游戏资产处理与程序动画框架

### Vector2.h:
便于后续坐标的计算
```cpp
#pragma once

#include <cmath>

class Vector2
{
public:
    float x = 0;
    float y = 0;

public:
    Vector2() = default;
    ~Vector2() = default;

    Vector2(float x, float y) : x(x), y(y) {}

    Vector2 operator+(const Vector2& vec) const
    {
        return Vector2(x + vec.x, y + vec.y);
    }

    void operator+=(const Vector2& vec)
    {
        x += vec.x;
        y += vec.y;
    }

    Vector2 operator-(const Vector2& vec) const
    {
        return Vector2(x - vec.x, y - vec.y);
    }

    void operator-=(const Vector2& vec)
    {
        x -= vec.x;
        y -= vec.y;
    }

    float operator*(const Vector2& vec) const
    {
        return x * vec.x + y * vec.y;
    }

    Vector2 operator*(float val) const
    {
        return Vector2(x * val, y * val);
    }

    void operator*=(float val)
    {
        x *= val;
        y *= val;
    }

    float length()
    {
        return sqrt(x * x + y * y);
    }

    Vector2 normalize()
    {
        float len = length();

        if (len == 0)
            return Vector2(0, 0);

        return Vector2(x / len, y / len);
    }

private:

};
````

### timer.h:
定时器功能
```cpp
#pragma once

#include <functional>

class Timer
{
public:
    Timer() = default;
    ~Timer() = default;

    void restart()
    {
        pass_time = 0;
        shotted = false;
    }

    void set_wait_time(float val)
    {
        wait_time = val;
    }

    void set_one_shot(bool flag)
    {
        one_shot = flag;
    }

    void set_on_timeout(std::function<void()> on_timeout)
    {
        this->on_timeout = on_timeout;
    }

    void pause()
    {
        paused = true;
    }

    void resume()
    {
        paused = false;
    }

    void on_update(float delta)
    {
        if (paused)
            return;

        pass_time += delta;
        if (pass_time >= wait_time)
        {
            bool can_shot = (!one_shot || (one_shot && !shotted));
            shotted = true;
            if (can_shot && on_timeout)
                on_timeout();
            pass_time -= wait_time;
        }
    }

private:
    float pass_time = 0;
    float wait_time = 0;
    bool shotted = false;
    bool paused = false;
    bool one_shot = false;
    std::function<void()> on_timeout;

};
```

## atlas.h
```cpp
#pragma once

#include <vector>
#include <graphics.h>

#include <stdio.h>

class Atlas
{
public:
    Atlas() = default;
    ~Atlas() = default;

    void load(LPCTSTR path_template, int num)
    {
        img_list.clear();
        img_list.resize(num);

        TCHAR path_file[256];
        for (int i = 0; i < num; i++)
        {
            _stprintf_s(path_file, path_template, i + 1);
            loadimage(&img_list[i], path_file);
        }
    }

    void clear()
    {
        img_list.clear();
    }

    int get_size() const
    {
        return (int)img_list.size();
    }

    IMAGE* get_image(int idx)
    {
        if (idx < 0 || idx >= img_list.size())
            return nullptr;

        return &img_list[idx];
    }

    void add_image(const IMAGE& img)
    {
        img_list.push_back(img);
    }

private:
    std::vector<IMAGE> img_list;
};
```