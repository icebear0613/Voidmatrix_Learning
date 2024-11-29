#pragma once

#include "scene.h"
#include "atlas.h"
#include "camera.h"
#include "scene_manager.h"
#include "animation.h"

#include <iostream>

extern Atlas atlas_peashooter_run_right;

extern SceneManager scene_manager;

class MenuScene: public Scene
{
public:
	MenuScene() = default;
	~MenuScene() = default;
	
	void on_entry() 
	{
		animation_peashooter_run_right.set_atlas(&atlas_peashooter_run_right);
		animation_peashooter_run_right.set_interval(75);
		animation_peashooter_run_right.set_loop(true);
			/*[]()
			{
				scene_manager.switch_to(SceneManager::SceneType::Game);
			};*/
	};

	void on_update(int delta) 
	{
		std::cout << "主菜单正在运行..." << std::endl;
		camera.on_update(delta);
		animation_peashooter_run_right.on_update(delta);
	};

	void on_draw() 
	{
		const Vector2& pos_camera = camera.get_position();
		animation_peashooter_run_right.on_draw((int)(100 - pos_camera.x), (int)(100 - pos_camera.y));
//		animation_peashooter_run_right.on_draw(100, 100);
	};

	void on_input(const ExMessage& msg) 
	{
		if (msg.message == WM_KEYDOWN)
		{
			scene_manager.switch_to(SceneManager::SceneType::Game);
		}
	};
	
	void on_exit() 
	{
		std::cout << "主菜单退出" << std::endl;
	};

private:
	Camera camera;
	Animation animation_peashooter_run_right;
};