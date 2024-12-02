#pragma once

#include "vector2.h"

#include <iostream>

class Camera
{
public:
	Camera() = default;
	~Camera() = default;

	const Vector2& get_position() const
	{
		return position;
	}

	void reset()
	{
		position.x = 0;
		position.y = 0;
	}

	void on_update(int delta)
	{
		const Vector2 speed = { -0.35f, 0 };
		position += speed * (float)delta;
	}

private:
	Vector2 position;
};