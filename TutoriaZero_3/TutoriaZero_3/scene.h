#pragma once

#include <graphics.h>

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	virtual void on_entry() {};
	virtual void on_update() {};
	virtual void on_draw() {};
	virtual void on_input(const ExMessage& msg) {};
	virtual void on_exit() {};

private:


};