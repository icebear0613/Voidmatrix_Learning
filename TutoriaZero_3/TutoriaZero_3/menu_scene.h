#pragma once

#include "scene.h";

#include <iostream>

class MenuScene: public Scene
{
public:
	MenuScene() = default;
	~MenuScene() = default;
	
	void on_entry() 
	{
		std::cout << "进入菜单" << std::endl;
	};

	void on_update() 
	{
		std::cout << "主菜单正在运行..." << std::endl;
	};

	void on_draw() 
	{
		outtextxy(10, 10, _T("主菜单绘图内容"));
	};

	void on_input(ExMessage& msg) {};
	
	void on_exit() 
	{
		std::cout << "主菜单退出" << std::endl;
	};

private:

	 
};