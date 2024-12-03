#pragma once

#include "scene.h"
#include "scene_manager.h"

#include <iostream>

class GameScene : public Scene
{
public:
	GameScene() = default;
	~GameScene() = default;

	void on_entry()
	{
		std::cout << "游戏界面 " << std::endl;
	}

	void on_update(int delta)
	{
		std::cout << "游戏界面更新ing" << std::endl;
	}

	void on_draw(const Camera& camera)
	{
		outtextxy(10, 10, _T("游戏界面"));
	}

	void on_input(const ExMessage& msg)
	{
		if (msg.message == WM_KEYDOWN)
		{
			scene_manager.switch_to(SceneManager::SceneType::Menu);
		}
	}

	void on_exit()
	{
		std::cout << "退出了游戏界面" << std::endl;
	}

private:

};