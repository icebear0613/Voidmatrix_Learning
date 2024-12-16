#include <iostream>

#include <graphics.h>

int main()
{
    initgraph(600, 400); // 初始化图形窗口
    setbkcolor(WHITE); // 设置背景颜色
    cleardevice(); // 清空屏幕

    while (true)
    {
        std::cout << "working..." << std::endl;
    }

    return 0;
}
