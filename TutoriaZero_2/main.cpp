#include <graphics.h>
#include <string>
#include <vector>
#include <cmath> 
#include <iostream>

int idx_current_anim = 0;
const int PLAYER_ANIM_NUM = 6;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const int BUTTON_WIDTH = 192;
const int BUTTON_HEIGHT = 75;

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];

bool running = true;
bool is_game_started = false;

#pragma comment(lib, "MSIMG32.LIB")
#pragma comment(lib, "Winmm.lib")

inline void putimage_alpha(int x, int y, IMAGE *img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

class Atlas
{
public:
	Atlas(LPCTSTR path, int num)
	{
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}

	~Atlas()
	{
		for (size_t i = 0; i < frame_list.size(); i++)
		{
			delete frame_list[i];
		}
	}

public:
	std::vector<IMAGE*> frame_list;

private:
	
};

Atlas* atlas_player_left;
Atlas* atlas_player_right;
Atlas* atlas_enemy_left;
Atlas* atlas_enemy_right;

class Animation
{
public:
	Animation(Atlas* atlas, int interval)
	{
		anim_atlas = atlas;
		interval_ms = interval;
	}

	~Animation() = default;

	void Play(int x, int y, int delta)
	{
		timer += delta;
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
			timer = 0;
		}

		putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
	}
private:
	int timer = 0;
	int idx_frame = 0;
	int interval_ms = 0;
	Atlas* anim_atlas;
};

class Player
{
public:
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;

public:
	Player()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));
		anim_left = new Animation(atlas_player_left, 45);
		anim_right = new Animation(atlas_player_right, 45);
	}
	
	~Player()
	{
		delete anim_left;
		delete anim_right;
	}

	void ProcessEvent(const ExMessage& msg)
	{
		if (msg.message == WM_KEYDOWN)
		{
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = true;
				break;
			case VK_DOWN:
				is_move_down = true;
				break;
			case VK_LEFT:
				is_move_left = true;
				break;
			case VK_RIGHT:
				is_move_right = true;
				break;
			}
		}
		else if (msg.message == WM_KEYUP)
		{
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = false;
				break;
			case VK_DOWN:
				is_move_down = false;
				break;
			case VK_LEFT:
				is_move_left = false;
				break;
			case VK_RIGHT:
				is_move_right = false;
				break;
			}
		}
	}

	void Move()
	{
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			player_pos.x += (int)(normalized_x * PLAYER_SPEED);
			player_pos.y += (int)(normalized_y * PLAYER_SPEED);
		}

		if (player_pos.x < 0) player_pos.x = 0;
		if (player_pos.y < 0) player_pos.y = 0;
		if (player_pos.x + FRAME_WIDTH > WINDOW_WIDTH) player_pos.x = WINDOW_WIDTH - FRAME_WIDTH;
		if (player_pos.y + FRAME_HEIGHT > WINDOW_HEIGHT) player_pos.y = WINDOW_HEIGHT - FRAME_HEIGHT;
	}

	void Draw(int delta)
	{
		int pos_shadow_x = player_pos.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = player_pos.y + FRAME_HEIGHT - 8;


		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

		if (facing_left)
			anim_left->Play(player_pos.x, player_pos.y, delta);
		else
			anim_right->Play(player_pos.x, player_pos.y, delta);
	}

	const POINT& GetPosition () const
	{
		return player_pos;
	}

private:
	const int SHADOW_WIDTH = 32;
	const int PLAYER_SPEED = 8;

private:
	POINT player_pos = { 500, 500 };
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;


};

class Bullet
{
public:
	POINT position = { 0, 0 };
public:
	Bullet() = default;
	~Bullet() = default;

	void Draw() const
	{
		setlinecolor(RGB(255, 155, 50));
		setlinecolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}
private:
	int RADIUS = 10;
};

class Enemy
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));
		anim_left = new Animation(atlas_enemy_left, 45);
		anim_right = new Animation(atlas_enemy_right, 45);

		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};

		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge)
		{
		case SpawnEdge::Up:
			position.x = rand() % WINDOW_WIDTH;
			position.y = -FRAME_HEIGHT;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = -FRAME_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x = WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		default:
			break;
		}
	}

	bool CheckBulletCollison(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + FRAME_WIDTH;
		bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + FRAME_HEIGHT;

		return is_overlap_x && is_overlap_y;
	}

	bool CheckPlayerCollsion(const Player& player)
	{
		POINT check_position = { position.x + FRAME_WIDTH / 2, position.y + FRAME_HEIGHT / 2 };
		const POINT& player_positon = player.GetPosition();
		bool is_overlap_x = check_position.x >= player_positon.x && check_position.x <= player_positon.x + player.FRAME_WIDTH;
		bool is_overlap_y = check_position.y >= player_positon.y && check_position.y <= player_positon.y + player.FRAME_HEIGHT;

		return is_overlap_x && is_overlap_y;
	}
	 
	void Move(const Player& player)
	{
		const POINT& player_position = player.GetPosition();
		int dir_x = player_position.x - position.x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}

		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;
	}

	void Hurt()
	{
		alive = false;
	}

	bool CheckAlive()
	{
		return alive;
	}

	void Draw(int delta)
	{
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = position.y + FRAME_HEIGHT - 35;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);
	}
	~Enemy()
	{
		delete anim_left;
		delete anim_right;
	}

private:
	const int SPEED = 5;
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;

private:
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0, 0 };
	bool facing_left = false;
	bool alive = true;
};

class Button
{
public:
	Button(RECT rect, LPCTSTR path_img_idle, LPCSTR path_img_hovered, LPCSTR path_img_pushed)
	{
		region = rect;

		loadimage(&img_idle, path_img_idle);
		loadimage(&img_hovered, path_img_hovered);
		loadimage(&img_idle, path_img_pushed);
	}
	
	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
				status = Status::Idle;
			else if (status == Status::Hovered && !CheckCursorHit(msg.x, msg.y))
				status = Status::Hovered;
			break;
		case WM_LBUTTONDOWN:
			if (CheckCursorHit(msg.x, msg.y))
				status = Status::Pushed;
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed)
				Onclick();
			break;
		default:
			break;
		}
	}

	void Draw()
	{
		switch (status)
		{
		case Status::Idle:
			putimage_alpha(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			putimage_alpha(region.left, region.top, &img_hovered);
			break;
		case Status::Pushed:
			putimage_alpha(region.left, region.top, &img_pushed);
			break; 
		}
	}

	~Button() = default;

protected:
	virtual void Onclick() = 0;

private:
	enum class Status
	{
		Idle = 0,
		Hovered,
		Pushed
	};

private:
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
	Status status = Status::Idle;

private:
	bool CheckCursorHit(int x, int y)
	{
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
};

class StartGameButton : public Button
{
public:
	StartGameButton(RECT rect, LPCSTR path_img_idle, LPCSTR path_img_hovered, LPCSTR path_img_pushed)
		: Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~StartGameButton() = default;

protected:
	void Onclick()
	{
		is_game_started = true;
		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}
};

class QuitGameButton : public Button
{
public:
	QuitGameButton(RECT rect, LPCSTR path_img_idle, LPCSTR path_img_hovered, LPCSTR path_img_pushed)
		: Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~QuitGameButton() = default;

protected:
	void Onclick()
	{
		running = false;
	}
};
std::string WStringToString(const std::wstring& ws)
{
	std::string str(ws.begin(), ws.end());
	return str;
}

void LoadAnimation()
{
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
		std::string pathStr = WStringToString(path);
		loadimage(&img_player_left[i], pathStr.c_str());

	}

	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_right_" + std::to_wstring(i) + L".png";
		std::string pathStr = WStringToString(path);
		loadimage(&img_player_right[i], pathStr.c_str());
	}

}

void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0)
		enemy_list.push_back(new Enemy());
}

void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player)
{
	const double RADIAL_SPEED = 0.0045;
	const double TANGENT_SPEED = 0.0055;
	double radain_interval = 2 * 3.14159 / bullet_list.size();
	POINT player_postion = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
	for (size_t i = 0; i < bullet_list.size(); i++)
	{
		double radian = GetTickCount() * TANGENT_SPEED + radain_interval * i;
		bullet_list[i].position.x = player_postion.x + player.FRAME_WIDTH / 2 + (int)(radius * cos(radian));
		bullet_list[i].position.y = player_postion.y + player.FRAME_HEIGHT / 2 + (int)(radius * sin(radian));
	}
}

void DrawPlayerScore(int score)
{
	static TCHAR text[64];
	_stprintf_s(text, _T("当前玩家得分：%d"), score);

	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);
}

int main()
{
	initgraph(1280, 720);

	atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);
	atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);
	atlas_enemy_left = new Atlas(_T("img/enemy_left_%d.png"), 6);
	atlas_enemy_right = new Atlas(_T("img/enemy_right_%d.png"), 6);

	mciSendString(_T("open mus/bgm.mp3 alis bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alis hit"), NULL, 0, NULL);

	LoadAnimation();

	int score = 0;
	Player player;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(3);
	ExMessage msg;
	IMAGE img_background;
	IMAGE img_menu;
	RECT region_btn_start_game, region_btn_quit_game;

	region_btn_start_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
	region_btn_start_game.right = region_btn_start_game.left + BUTTON_WIDTH;
	region_btn_start_game.top = 430;
	region_btn_start_game.bottom = region_btn_start_game.top + BUTTON_HEIGHT;

	region_btn_quit_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
	region_btn_quit_game.right = region_btn_start_game.left + BUTTON_WIDTH;
	region_btn_quit_game.top = 550;
	region_btn_quit_game.bottom = region_btn_start_game.top + BUTTON_HEIGHT;

	StartGameButton btn_start_game = StartGameButton(region_btn_start_game,
		_T("img/ui_start_idle.png"), _T("img/ui_start_hovered.png"), _T("img/ui_start_pushed.png"));
	QuitGameButton btn_quit_game = QuitGameButton(region_btn_quit_game,
		_T("img/ui_quit_idle.png"), _T("img/ui_quit_hovered.png"), _T("img/ui_quit_pushed.png"));

	loadimage(&img_menu, _T("img/menu.png"));
	loadimage(&img_background, _T("img/background.png"));

	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg))
		{
			if (is_game_started)
				player.ProcessEvent(msg);
			else
			{
				btn_start_game.ProcessEvent(msg);
				btn_quit_game.ProcessEvent(msg);
			}
		}
	
		if (is_game_started)
		{
			player.Move();
			UpdateBullets(bullet_list, player);
			TryGenerateEnemy(enemy_list);
			for (Enemy* enemy : enemy_list)
				enemy->Move(player);

			for (Enemy* enemy : enemy_list)
			{
				if (enemy->CheckPlayerCollsion(player))
				{
					static TCHAR text[128];
					_stprintf_s(text, "最终得分： %d!", score);
					MessageBox(GetHWnd(), text, _T("游戏结束"), MB_OK);
					running = false;
					break;
				}
			}

			for (Enemy* enemy : enemy_list)
			{
				for (const Bullet& bullet : bullet_list)
				{
					if (enemy->CheckBulletCollison(bullet))
					{
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
						enemy->Hurt();
						score++;
					}
				}
			}

			for (size_t i = 0; i < enemy_list.size(); i++)
			{
				Enemy* enemy = enemy_list[i];
				if (!enemy->CheckAlive())
				{
					std::swap(enemy_list[i], enemy_list.back());
					enemy_list.pop_back();
					delete enemy;
				}
			}
		}
		
		cleardevice();
		
		if (is_game_started)
		{
			putimage(0, 0, &img_background);
			player.Draw(1000 / 144);
			for (Enemy* enemy : enemy_list)
				enemy->Draw(1000 / 144);
			for (const Bullet& bullet : bullet_list)
				bullet.Draw();
			DrawPlayerScore(score);
		}
		else
		{
			putimage(0, 0, &img_menu);
			btn_start_game.Draw();
			btn_quit_game.Draw();
		}

		FlushBatchDraw();
		
		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}
	}

	delete atlas_player_left;
	delete atlas_player_right;
	delete atlas_enemy_left;
	delete atlas_enemy_right;

	EndBatchDraw();

	return 0;
}
