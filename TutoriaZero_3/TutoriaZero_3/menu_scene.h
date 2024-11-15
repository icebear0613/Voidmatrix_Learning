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
		std::cout << "����˵�" << std::endl;
	};

	void on_update() 
	{
		std::cout << "���˵���������..." << std::endl;
	};

	void on_draw() 
	{
		outtextxy(10, 10, _T("���˵���ͼ����"));
	};

	void on_input(ExMessage& msg) {};
	
	void on_exit() 
	{
		std::cout << "���˵��˳�" << std::endl;
	};

private:

	 
};