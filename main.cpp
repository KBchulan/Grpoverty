#include<graphics.h>
#include<conio.h>
#include<Windows.h>
#include<vector>
#include<iostream>
#include<time.h>
#include<fstream>
#include<sstream>
#include<list>
#include<stdio.h>
#include<string.h>
#include<tchar.h>

bool running = true;
const int Window_width = 1280;
const int Window_height = 720;
int rando1 = 100;
int rando2 = 150;
int rando3 = 200;
int manba = 300;
int bgmnum;
int gamechangenum;
int flag = 1;//标志界面
int flag_name = 0;//是否输入昵称
POINT cur_position = { 500,500 };

/*
1为菜单
2为排行榜
3为音乐
4为设置界面
5为游戏界面
6为游戏选项
7为制作名单
8为暂停界面
*/

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")
#pragma warning(disable:4996)
#define filename "rank.txt"

//储存玩家信息
struct  paiHang {
	char id[30];//名字
	int   score;//分数
};
struct paiHang PLAYER;
char player_name[30];
std::list <paiHang> RANK;

inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg, UINT transparentcolor)
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	TransparentBlt(dstDC, x, y, w, h, srcDC, 0, 0, w, h, transparentcolor);
}

class Animation {
public:
	Animation(LPCTSTR path, int num, int interval) {
		interval_ms = interval;
		TCHAR path_file[256];

		for (auto i = 0; i < num; i++) {
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}

	~Animation() {
		for (auto i = 0; i < frame_list.size(); i++)delete frame_list[i];
	}

	void Play(int x, int y, int delta) {
		timer += delta;
		if (timer >= interval_ms) {
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}

		putimage_alpha(x, y, frame_list[idx_frame]);
	}


private:
	int timer = 0;//动画计时器
	int idx_frame = 0;//动画帧索引
	int interval_ms = 0;
	std::vector<IMAGE*> frame_list;
};

class Player {//图集类
public:
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;

public:
	Player() {
		loadimage(&img_shadow, _T("img/shadow_player.png"));
		anim_left = new Animation(_T("img/player_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/player_right_%d.png"), 6, 45);
	}

	~Player() {
		delete anim_left;
		delete anim_right;
	}

	void processevent(const ExMessage& msg) {
		switch (msg.message) {
		case WM_KEYDOWN:
			switch (msg.vkcode) {
			case VK_UP:is_move_up = true; break;
			case VK_DOWN:is_move_down = true; break;
			case VK_LEFT:is_move_left = true; break;
			case VK_RIGHT:is_move_right = true; break;
			}
			break;

		case WM_KEYUP:
			switch (msg.vkcode) {
			case VK_UP:is_move_up = false; break;
			case VK_DOWN:is_move_down = false; break;
			case VK_LEFT:is_move_left = false; break;
			case VK_RIGHT:is_move_right = false; break;
			}
			break;
		}
	}

	void Move() {
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir) {
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}

		if (position.x < 0)position.x = 0;
		if (position.y < 0)position.y = 0;
		if (position.x + FRAME_WIDTH > Window_width)position.x = Window_width - FRAME_WIDTH;
		if (position.y + FRAME_HEIGHT > Window_height)position.y = Window_height - FRAME_HEIGHT;
	}

	void Draw(int delta) {
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = position.y + FRAME_HEIGHT - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0) {
			facing_left = true;
		}
		else if (dir_x > 0) {
			facing_left = false;
		}

		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);
	}

	const POINT& GetPosition() const {
		return position;
	}

private:
	const int SPEED = 6;
	const int SHADOW_WIDTH = 32;
private:
	IMAGE img_shadow;
	POINT position = { 500,500 };
	Animation* anim_left;
	Animation* anim_right;
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
};

class Player1 {
public:
	const int FRAME_WIDTH = 90;
	const int FRAME_HEIGHT = 80;
	const int grade = 1;

public:
	Player1() {
		loadimage(&img_player, _T("img/player_test.png"), 90, 80);
	}

	~Player1() = default;

	void processevent(const ExMessage& msg) {
		switch (msg.message) {
		case WM_KEYDOWN:
			switch (msg.vkcode) {
			case VK_UP:is_move_up = true; break;
			case VK_DOWN:is_move_down = true; break;
			case VK_LEFT:is_move_left = true; break;
			case VK_RIGHT:is_move_right = true; break;
			}
			break;

		case WM_KEYUP:
			switch (msg.vkcode) {
			case VK_UP:is_move_up = false; break;
			case VK_DOWN:is_move_down = false; break;
			case VK_LEFT:is_move_left = false; break;
			case VK_RIGHT:is_move_right = false; break;
			}
			break;
		}
	}

	void Move() {
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir) {
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			cur_position.x += (int)(SPEED * normalized_x);
			cur_position.y += (int)(SPEED * normalized_y);
		}

		if (cur_position.x < 0)cur_position.x = 0;
		if (cur_position.y < 0)cur_position.y = 0;
		if (cur_position.x + FRAME_WIDTH > Window_width)cur_position.x = Window_width - FRAME_WIDTH;
		if (cur_position.y + FRAME_HEIGHT > Window_height)cur_position.y = Window_height - FRAME_HEIGHT;
	}

	void Draw(int delta) {
		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0) {
			facing_left = true;
		}
		else if (dir_x > 0) {
			facing_left = false;
		}
		putimage_alpha(cur_position.x, cur_position.y, &img_player);
	}

private:
	const int SPEED = 6;
private:
	//POINT position = cur_position;
	IMAGE img_player;
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
};

class Player2 {
public:
	const int FRAME_WIDTH = 140;
	const int FRAME_HEIGHT = 130;
	const int grade = 2;

public:
	Player2() {
		loadimage(&img_player, _T("img/player_test.png"), 140, 130);
	}

	~Player2() = default;

	void processevent(const ExMessage& msg) {
		switch (msg.message) {
		case WM_KEYDOWN:
			switch (msg.vkcode) {
			case VK_UP:is_move_up = true; break;
			case VK_DOWN:is_move_down = true; break;
			case VK_LEFT:is_move_left = true; break;
			case VK_RIGHT:is_move_right = true; break;
			}
			break;

		case WM_KEYUP:
			switch (msg.vkcode) {
			case VK_UP:is_move_up = false; break;
			case VK_DOWN:is_move_down = false; break;
			case VK_LEFT:is_move_left = false; break;
			case VK_RIGHT:is_move_right = false; break;
			}
			break;
		}
	}

	void Move() {
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir) {
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			cur_position.x += (int)(SPEED * normalized_x);
			cur_position.y += (int)(SPEED * normalized_y);
		}

		if (cur_position.x < 0)cur_position.x = 0;
		if (cur_position.y < 0)cur_position.y = 0;
		if (cur_position.x + FRAME_WIDTH > Window_width)cur_position.x = Window_width - FRAME_WIDTH;
		if (cur_position.y + FRAME_HEIGHT > Window_height)cur_position.y = Window_height - FRAME_HEIGHT;
	}

	void Draw(int delta) {
		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0) {
			facing_left = true;
		}
		else if (dir_x > 0) {
			facing_left = false;
		}
		putimage_alpha(cur_position.x, cur_position.y, &img_player);
	}

private:
	const int SPEED = 5;
private:
	//POINT position = cur_position;
	IMAGE img_player;
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
};

class Player3 {
public:
	const int FRAME_WIDTH = 190;
	const int FRAME_HEIGHT = 180;
	const int grade = 3;

public:
	Player3() {
		loadimage(&img_player, _T("img/player_test.png"), 190, 180);
	}

	~Player3() = default;

	void processevent(const ExMessage& msg) {
		switch (msg.message) {
		case WM_KEYDOWN:
			switch (msg.vkcode) {
			case VK_UP:is_move_up = true; break;
			case VK_DOWN:is_move_down = true; break;
			case VK_LEFT:is_move_left = true; break;
			case VK_RIGHT:is_move_right = true; break;
			}
			break;

		case WM_KEYUP:
			switch (msg.vkcode) {
			case VK_UP:is_move_up = false; break;
			case VK_DOWN:is_move_down = false; break;
			case VK_LEFT:is_move_left = false; break;
			case VK_RIGHT:is_move_right = false; break;
			}
			break;
		}
	}

	void Move() {
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir) {
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			cur_position.x += (int)(SPEED * normalized_x);
			cur_position.y += (int)(SPEED * normalized_y);
		}

		if (cur_position.x < 0)cur_position.x = 0;
		if (cur_position.y < 0)cur_position.y = 0;
		if (cur_position.x + FRAME_WIDTH > Window_width)cur_position.x = Window_width - FRAME_WIDTH;
		if (cur_position.y + FRAME_HEIGHT > Window_height)cur_position.y = Window_height - FRAME_HEIGHT;
	}

	void Draw(int delta) {
		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0) {
			facing_left = true;
		}
		else if (dir_x > 0) {
			facing_left = false;
		}
		putimage_alpha(cur_position.x, cur_position.y, &img_player);
	}

private:
	const int SPEED = 5;
private:
	//POINT position = cur_position;
	IMAGE img_player;
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
};

class Player4 {
public:
	const int FRAME_WIDTH = 210;
	const int FRAME_HEIGHT = 200;
	const int grade = 4;

public:
	Player4() {
		loadimage(&img_player, _T("img/player_test.png"), 210, 200);
	}

	~Player4() = default;

	void processevent(const ExMessage& msg) {
		switch (msg.message) {
		case WM_KEYDOWN:
			switch (msg.vkcode) {
			case VK_UP:is_move_up = true; break;
			case VK_DOWN:is_move_down = true; break;
			case VK_LEFT:is_move_left = true; break;
			case VK_RIGHT:is_move_right = true; break;
			}
			break;

		case WM_KEYUP:
			switch (msg.vkcode) {
			case VK_UP:is_move_up = false; break;
			case VK_DOWN:is_move_down = false; break;
			case VK_LEFT:is_move_left = false; break;
			case VK_RIGHT:is_move_right = false; break;
			}
			break;
		}
	}

	void Move() {
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir) {
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			cur_position.x += (int)(SPEED * normalized_x);
			cur_position.y += (int)(SPEED * normalized_y);
		}

		if (cur_position.x < 0)cur_position.x = 0;
		if (cur_position.y < 0)cur_position.y = 0;
		if (cur_position.x + FRAME_WIDTH > Window_width)cur_position.x = Window_width - FRAME_WIDTH;
		if (cur_position.y + FRAME_HEIGHT > Window_height)cur_position.y = Window_height - FRAME_HEIGHT;
	}

	void Draw(int delta) {
		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0) {
			facing_left = true;
		}
		else if (dir_x > 0) {
			facing_left = false;
		}
		putimage_alpha(cur_position.x, cur_position.y, &img_player);
	}

private:
	const int SPEED = 5;
private:
	//POINT position = cur_position;
	IMAGE img_player;
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
};

class Enemy {
public:
	const int grade = 0;
public:
	Enemy() {
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/enemy_right_%d.png"), 6, 45);
		enum class SpawnEdge {
			Left = 0,
			Right
		};

		SpawnEdge edge = (SpawnEdge)(rand() % 2);
		switch (edge)
		{
		case SpawnEdge::Left:
			facing_left = false;
			position.x = -FRAME_WIDTH;
			position.y = rand() % Window_height;
			break;
		case SpawnEdge::Right:
			facing_left = true;
			position.x = Window_width;
			position.y = rand() % Window_height;
			break;
		default:
			break;
		}
	}

	bool checkplayercollision1(const Player1& player) {
		if (position.x + FRAME_WIDTH < cur_position.x || position.y + FRAME_HEIGHT < cur_position.y)
			return false;
		if (position.x > cur_position.x + 90 || position.y > cur_position.y + 80)
			return false;
		return true;
	}

	bool checkplayercollision2(const Player2& player) {
		if (position.x + FRAME_WIDTH < cur_position.x || position.y + FRAME_HEIGHT < cur_position.y)
			return false;
		if (position.x > cur_position.x + 140 || position.y > cur_position.y + 130)
			return false;
		return true;
	}

	bool checkplayercollision3(const Player3& player) {
		if (position.x + FRAME_WIDTH < cur_position.x || position.y + FRAME_HEIGHT < cur_position.y)
			return false;
		if (position.x > cur_position.x + 190 || position.y > cur_position.y + 180)
			return false;
		return true;
	}

	bool checkplayercollision4(const Player4& player) {
		if (position.x + FRAME_WIDTH < cur_position.x || position.y + FRAME_HEIGHT < cur_position.y)
			return false;
		if (position.x > cur_position.x + 210 || position.y > cur_position.y + 200)
			return false;
		return true;
	}

	void Move() {
		if (facing_left)position.x -= SPEED;
		else position.x += SPEED;
	}

	void Draw(int delta) {
		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);
	}

	~Enemy() {
		delete anim_left;
		delete anim_right;
	}

	void Hurt() {
		alive = false;
	}

	bool CheckAlive() {
		return alive;
	}

private:
	const int SPEED = 3;
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;

private:
	POINT position = { 0,0 };
	Animation* anim_left;
	Animation* anim_right;
	bool facing_left = false;
	bool alive = true;
};

class Enemy2 {
public:
	const int grade = 1;
public:
	Enemy2() {
		loadimage(&basketball, _T("img/basketball.jpg"), 120, 120);
		enum class SpawnEdge {
			Left = 0,
			Right
		};

		SpawnEdge edge = (SpawnEdge)(rand() % 2);
		switch (edge)
		{
		case SpawnEdge::Left:
			facing_left = false;
			position.x = -FRAME_WIDTH;
			position.y = rand() % Window_height;
			break;
		case SpawnEdge::Right:
			facing_left = true;
			position.x = Window_width;
			position.y = rand() % Window_height;
			break;
		default:
			break;
		}
	}

	bool checkplayercollision1(const Player1& player) {
		if (position.x + FRAME_WIDTH - 40 < cur_position.x || position.y + FRAME_HEIGHT - 40 < cur_position.y)
			return false;
		if (position.x + 40 > cur_position.x + 90 || position.y + 40 > cur_position.y + 80)
			return false;
		return true;
	}

	bool checkplayercollision2(const Player2& player) {
		if (position.x + FRAME_WIDTH - 40 < cur_position.x || position.y + FRAME_HEIGHT - 40 < cur_position.y)
			return false;
		if (position.x + 40 > cur_position.x + 140 || position.y + 40 > cur_position.y + 130)
			return false;
		return true;
	}

	bool checkplayercollision3(const Player3& player) {
		if (position.x + FRAME_WIDTH - 40 < cur_position.x || position.y + FRAME_HEIGHT - 40 < cur_position.y)
			return false;
		if (position.x + 40 > cur_position.x + 190 || position.y + 40 > cur_position.y + 180)
			return false;
		return true;
	}

	bool checkplayercollision4(const Player4& player) {
		if (position.x + FRAME_WIDTH < cur_position.x || position.y + FRAME_HEIGHT < cur_position.y)
			return false;
		if (position.x > cur_position.x + 210 || position.y > cur_position.y + 200)
			return false;
		return true;
	}

	void Move() {
		if (facing_left)position.x -= SPEED;
		else position.x += SPEED;
	}

	void Draw(int delta) {
		transparentimage(NULL, position.x, position.y, &basketball, WHITE);
	}

	~Enemy2() = default;

	void Hurt() {
		alive = false;
	}

	bool CheckAlive() {
		return alive;
	}

private:
	const int SPEED = 2;
	const int FRAME_WIDTH = 120;
	const int FRAME_HEIGHT = 120;

private:
	POINT position = { 0,0 };
	IMAGE basketball;
	bool facing_left = false;
	bool alive = true;
};

class Enemy3 {
public:
	const int grade = 2;
public:
	Enemy3() {
		loadimage(&img_girl, _T("img/girl.png"), 150, 150);
		enum class SpawnEdge {
			Left = 0,
			Right
		};

		SpawnEdge edge = (SpawnEdge)(rand() % 2);
		switch (edge)
		{
		case SpawnEdge::Left:
			facing_left = false;
			position.x = -FRAME_WIDTH;
			position.y = rand() % Window_height;
			break;
		case SpawnEdge::Right:
			facing_left = true;
			position.x = Window_width;
			position.y = rand() % Window_height;
			break;
		default:
			break;
		}
	}

	bool checkplayercollision1(const Player1& player) {
		if (position.x + FRAME_WIDTH - 60 < cur_position.x || position.y + FRAME_HEIGHT - 60 < cur_position.y)
			return false;
		if (position.x + 60 > cur_position.x + 90 || position.y + 60 > cur_position.y + 80)
			return false;
		return true;
	}

	bool checkplayercollision2(const Player2& player) {
		if (position.x + FRAME_WIDTH - 60 < cur_position.x || position.y + FRAME_HEIGHT - 60 < cur_position.y)
			return false;
		if (position.x + 60 > cur_position.x + 140 || position.y + 60 > cur_position.y + 130)
			return false;
		return true;
	}

	bool checkplayercollision3(const Player3& player) {
		if (position.x + FRAME_WIDTH - 60 < cur_position.x || position.y + FRAME_HEIGHT - 60 < cur_position.y)
			return false;
		if (position.x + 60 > cur_position.x + 190 || position.y + 60 > cur_position.y + 180)
			return false;
		return true;
	}

	bool checkplayercollision4(const Player4& player) {
		if (position.x + FRAME_WIDTH < cur_position.x || position.y + FRAME_HEIGHT < cur_position.y)
			return false;
		if (position.x > cur_position.x + 210 || position.y > cur_position.y + 200)
			return false;
		return true;
	}

	void Move() {
		if (facing_left)position.x -= SPEED;
		else position.x += SPEED;
	}

	void Draw(int delta) {
		//putimage(position.x, position.y, &img_girl);
		transparentimage(NULL, position.x, position.y, &img_girl, WHITE);
	}

	~Enemy3() = default;

	void Hurt() {
		alive = false;
	}

	bool CheckAlive() {
		return alive;
	}

private:
	const int SPEED = 2;
	const int FRAME_WIDTH = 150;
	const int FRAME_HEIGHT = 150;

private:
	POINT position = { 0,0 };
	IMAGE img_girl;
	bool facing_left = false;
	bool alive = true;
};

class Enemy4 {
public:
	const int grade = 3;
public:
	Enemy4() {
		loadimage(&img_qingque, _T("img/qingque.jpg"), 130, 130);
		enum class SpawnEdge {
			Left = 0,
			Right
		};

		SpawnEdge edge = (SpawnEdge)(rand() % 2);
		switch (edge)
		{
		case SpawnEdge::Left:
			facing_left = false;
			position.x = -FRAME_WIDTH;
			position.y = rand() % Window_height;
			break;
		case SpawnEdge::Right:
			facing_left = true;
			position.x = Window_width;
			position.y = rand() % Window_height;
			break;
		default:
			break;
		}
	}

	bool checkplayercollision1(const Player1& player) {
		if (position.x + FRAME_WIDTH - 40 < cur_position.x || position.y + FRAME_HEIGHT - 25 < cur_position.y)
			return false;
		if (position.x + 60 > cur_position.x + 90 || position.y + 60 > cur_position.y + 80)
			return false;
		return true;
	}

	bool checkplayercollision2(const Player2& player) {
		if (position.x + FRAME_WIDTH - 40 < cur_position.x || position.y + FRAME_HEIGHT - 25 < cur_position.y)
			return false;
		if (position.x + 60 > cur_position.x + 140 || position.y + 60 > cur_position.y + 130)
			return false;
		return true;
	}

	bool checkplayercollision3(const Player3& player) {
		if (position.x + FRAME_WIDTH - 40 < cur_position.x || position.y + FRAME_HEIGHT - 25 < cur_position.y)
			return false;
		if (position.x + 60 > cur_position.x + 190 || position.y + 60 > cur_position.y + 180)
			return false;
		return true;
	}

	bool checkplayercollision4(const Player4& player) {
		if (position.x + FRAME_WIDTH - 40 < cur_position.x || position.y + FRAME_HEIGHT - 40 < cur_position.y)
			return false;
		if (position.x > cur_position.x + 210 || position.y > cur_position.y + 200)
			return false;
		return true;
	}

	void Move() {
		if (facing_left)position.x -= SPEED;
		else position.x += SPEED;
	}

	void Draw(int delta) {
		//putimage(position.x, position.y, &img_girl);
		transparentimage(NULL, position.x, position.y, &img_qingque, BLACK);
	}

	~Enemy4() = default;

	void Hurt() {
		alive = false;
	}

	bool CheckAlive() {
		return alive;
	}

private:
	const int SPEED = 2;
	const int FRAME_WIDTH = 130;
	const int FRAME_HEIGHT = 130;

private:
	POINT position = { 0,0 };
	IMAGE img_qingque;
	bool facing_left = false;
	bool alive = true;
};

class Button {
public:
	Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed) {
		region = rect;
		loadimage(&img_idle, path_img_idle, 180, 75);
		loadimage(&img_hovered, path_img_hovered, 180, 75);
		loadimage(&img_pushed, path_img_pushed, 180, 75);
	}

	~Button() = default;

	void Processment(const ExMessage& msg) {
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && checkcursorhit(msg.x, msg.y))
				status = Status::Hovered;
			else if (status == Status::Hovered && !checkcursorhit(msg.x, msg.y))
				status = Status::Idle;
			break;
		case WM_LBUTTONDOWN:
			if (checkcursorhit(msg.x, msg.y))
				status = Status::Pushed;
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed && checkcursorhit(msg.x, msg.y))
				OnClick();
				status = Status::Idle;
			break;
		default:
			break;
		}
	}

	void Draw() {
		switch (status) {
		case Status::Idle:
			putimage(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			putimage(region.left, region.top, &img_hovered);
			break;
		case Status::Pushed:
			putimage(region.left, region.top, &img_pushed);
			break;
		}
	}

protected:
	virtual void OnClick() = 0;

private:
	enum class Status {
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
	bool checkcursorhit(int x, int y) {
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
};

class Button2 {
public:
	Button2(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed) {
		region = rect;
		loadimage(&img_idle, path_img_idle, 40, 40);
		loadimage(&img_hovered, path_img_hovered, 40, 40);
		loadimage(&img_pushed, path_img_pushed, 40, 40);
	}

	~Button2() = default;

	void Processment(const ExMessage& msg) {
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && checkcursorhit(msg.x, msg.y))
				status = Status::Hovered;
			else if (status == Status::Hovered && !checkcursorhit(msg.x, msg.y))
				status = Status::Idle;
			break;
		case WM_LBUTTONDOWN:
			if (checkcursorhit(msg.x, msg.y))
				status = Status::Pushed;
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed && checkcursorhit(msg.x, msg.y))
				OnClick();
			status = Status::Idle;
			break;
		default:
			break;
		}
	}

	void Draw() {
		switch (status) {
		case Status::Idle:
			putimage(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			putimage(region.left, region.top, &img_hovered);
			break;
		case Status::Pushed:
			putimage(region.left, region.top, &img_pushed);
			break;
		}
	}

protected:
	virtual void OnClick() = 0;

private:
	enum class Status {
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
	bool checkcursorhit(int x, int y) {
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
};

int search_people(char temp_name[]) {
	std::list<paiHang>::iterator it = RANK.begin();
	for (; it != RANK.end(); it++) {
		if (strcmp(it->id, temp_name) == 0) {
			return -1;
		}
	}
	return 1;
}

bool cmp(paiHang a, paiHang b) {
	return a.score > b.score;
}

void rank(int score) {
	PLAYER.score = score;
	RANK.push_front(PLAYER);
	RANK.sort(cmp);
}

void Get_Rank() {
	bool empty_a = false;	//判断文件是否为空 
	std::ifstream p;
	p.open(filename, std::ios::in);
	if (!p.is_open() || p.eof()) {		//检查文件是否为空 
		empty_a = true;
		p.close();
	}
	if (empty_a == false) {			//文件不为空，则读取文件 
		std::string line;
		while (std::getline(p, line)) {
			std::istringstream iss(line);
			paiHang newNode;
			iss >> newNode.id;
			iss >> newNode.score;
			RANK.push_front(newNode);
		}
		p.close();
	}
}

void Save_Rank() {
	std::ofstream q;
	q.open(filename, std::ios::out | std::ios::trunc);
	std::list<paiHang>::iterator it = RANK.begin();
	for (; it != RANK.end(); it++) {
		q << it->id << " " << it->score << std::endl;
	}
	q.close();
}

class StartGamebutton :public Button {
public:
	StartGamebutton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed){}
	~StartGamebutton() = default;
protected:
	void OnClick() {
		running = true;
		if (flag_name == 0) {
			InputBox(player_name, 10, 0, "请输入您的昵称", 0, 0, 0, false);
			if (search_people(player_name) == 1) {
				strcpy(PLAYER.id, player_name);
				flag_name = 1;
			}
		}
		flag = 5;
	}
};

class QuitGamebutton :public Button {
public:
	QuitGamebutton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~QuitGamebutton() = default;
protected:
	void OnClick() {
		RANK.clear();
		running = false;
	}
};

class Setbutton :public Button {//主菜单的
public:
	Setbutton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Setbutton() = default;
protected:
	void OnClick() {
		flag = 4;
	}
};

class Ranklist :public Button {
public:
	Ranklist(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Ranklist() = default;
protected:
	void OnClick() {
		flag = 2;
	}
};

class Gameselector :public Button {
public:
	Gameselector(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Gameselector() = default;
protected:
	void OnClick() {
		flag = 6;
	}
};

class Housebutton :public Button {
public:
	Housebutton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Housebutton() = default;
protected:
	void OnClick() {
		flag = 1;
	}
};

class Controlbgmopen :public Button {
public:
	Controlbgmopen(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Controlbgmopen() = default;
protected:
	void OnClick() {
		mciSendString(_T("stop all"), NULL, 0, NULL);
		mciSendString(_T("play bgm2 repeat"), NULL, 0, NULL);
	}
};

class Controlbgmquit :public Button {
public:
	Controlbgmquit(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Controlbgmquit() = default;
protected:
	void OnClick() {
		/*switch (bgmnum) {
		case 1:
			mciSendString(_T("stop bgm1"), 0, 0, 0);
			break;
		case 2:
			mciSendString(_T("stop bgm2"), 0, 0, 0);
			break;
		case 3:
			mciSendString(_T("stop bgm2"), 0, 0, 0);
			break;
		default:break;
		}*/
		mciSendString(_T("stop all"), NULL, 0, NULL);
	}
};

class Makerlist :public Button {
public:
	Makerlist(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Makerlist() = default;
protected:
	void OnClick() {
		flag = 9;
	}
};

class Musicbutton :public Button {
public:
	Musicbutton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Musicbutton() = default;
protected:
	void OnClick() {
		flag = 3;
	}
};

class Setupbutton :public Button2 {//游戏内的
public:
	Setupbutton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button2(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Setupbutton() = default;
protected:
	void OnClick() {
		flag = 8;
	}
};

class Continuegame :public Button {
public:
	Continuegame(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Continuegame() = default;
protected:
	void OnClick() {
		flag = 5;
	}
};

class Gameintroduction :public Button {
public:
	Gameintroduction(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Gameintroduction() = default;
protected:
	void OnClick() {
		flag = 0;
	}
};

class Gamechange :public Button {
public:
	Gamechange(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Gamechange() = default;
protected:
	void OnClick() {
		if (gamechangenum % 3 == 1 ) {//1的倍数为原版，此后依次增一倍
			rando1 = 100;
			rando2 = 150;
			rando3 = 200;
			manba = 300;
		}
		else if (gamechangenum % 3 == 2) {
			rando1 = 200;
			rando2 = 300;
			rando3 = 400;
			manba = 600;
		}
		else {
			rando1 = 200;
			rando2 = 400;
			rando3 = 800;
			manba = 1400;
		}
	}
};

class Musicselector1 :public Button {
public:
	Musicselector1(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Musicselector1() = default;
protected:
	void OnClick() {
		mciSendString(_T("stop all"), NULL, 0, NULL);
		mciSendString(_T("play bgm1 repeat"), NULL, 0, NULL);
	}
};

class Musicselector2 :public Button {
public:
	Musicselector2(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Musicselector2() = default;
protected:
	void OnClick() {
		mciSendString(_T("stop all"), NULL, 0, NULL);
		mciSendString(_T("play bgm2 repeat"), NULL, 0, NULL);
	}
};

class Musicselector3 :public Button {
public:
	Musicselector3(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~Musicselector3() = default;
protected:
	void OnClick() {
		mciSendString(_T("stop all"), NULL, 0, NULL);
		mciSendString(_T("play bgm3 repeat"), NULL, 0, NULL);
	}
};

void Trygenerateenemy(std::vector<Enemy*>& enemy_list) {
	const int INTERVAL = 150;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0) {
		enemy_list.push_back(new Enemy());
	}
}

void Trygenerateenemy2(std::vector<Enemy2*>& enemy_list) {
	const int INTERVAL = 300;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0) {
		enemy_list.push_back(new Enemy2());
	}
}

void Trygenerateenemy3(std::vector<Enemy3*>& enemy_list) {
	const int INTERVAL = 400;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0) {
		enemy_list.push_back(new Enemy3());
	}
}

void Trygenerateenemy4(std::vector<Enemy4*>& enemy_list) {
	const int INTERVAL = 200;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0) {
		enemy_list.push_back(new Enemy4());
	}
}

void DrawPlayerscore(int score) {
	static TCHAR text[64];
	_stprintf_s(text, _T("当前得分:%d"), score);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);
}

void DrawPlayernext(int score) {
	static TCHAR text[64];
	_stprintf_s(text, _T("进入下一阶段目标得分:%d"), score);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(595, 10, text);
}

void Drawattention() {
	static TCHAR text[64];
	_stprintf_s(text, _T("初始难度为1，共有三个难度"));
	setbkmode(TRANSPARENT);
	settextcolor(RGB(0, 0, 0));
	outtextxy(735, 440, text);
}

int main() {
	Get_Rank();
	srand(time(NULL));

	initgraph(1280, 720, EX_SHOWCONSOLE);

	mciSendString(_T("open mus/bgm1.mp3 alias bgm1"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
	mciSendString(_T("open mus/bgm2.mp3 alias bgm2"), NULL, 0, NULL);
	mciSendString(_T("open mus/bgm3.mp3 alias bgm3"), NULL, 0, NULL);
	mciSendString(_T("open mus/eat.mp3 alias eat"), NULL, 0, NULL);

	int score = 0;
	Player1 player_test1;
	Player2 player_test2;
	Player3 player_test3;
	Player4 player_test4;
	ExMessage msg;
	IMAGE img_game_background;
	IMAGE img_music_background;
	IMAGE img_menu_background;
	IMAGE img_rank_background;
	IMAGE img_selector_background;
	IMAGE img_zanting_background;
	IMAGE img_introduction_background;
	IMAGE img_passage;
	IMAGE img_victory;
	IMAGE img_makerlist;
	IMAGE img_title;
	std::vector<Enemy*> enemy_list;
	std::vector<Enemy2*> enemy_list2;
	std::vector<Enemy3*> enemy_list3;
	std::vector<Enemy4*> enemy_list4;
	std::list<paiHang>::iterator ii = RANK.begin();  //定义排行榜迭代器
	RECT region_gamechange,region_game_introduction,region_continuegame_button,region_setup_button,region_maker_list,region_bgmopen,region_bgmquit,region_game_selector,region_ranklist ,region_start_game, region_quit_game, region_set_button, region_music_button, region_music_selector1, region_music_selector2, region_music_selector3, region_house_button;

	region_gamechange.left = 550;
	region_gamechange.right = 730;
	region_gamechange.top = 400;
	region_gamechange.bottom = 475;

	region_continuegame_button.left = 550;
	region_continuegame_button.right = 730;
	region_continuegame_button.top = 500;
	region_continuegame_button.bottom = 575;

	region_setup_button.left = 1240;
	region_setup_button.right = 1280;
	region_setup_button.top = 0;
	region_setup_button.bottom = 40;

	region_bgmopen.left = 550;
	region_bgmopen.right = 730;
	region_bgmopen.top = 200;
	region_bgmopen.bottom = 275;
	
	region_game_introduction.left = 550;
	region_game_introduction.right = 730;
	region_game_introduction.top = 500;
	region_game_introduction.bottom = 575;

	region_maker_list.left = 550;
	region_maker_list.right = 730;
	region_maker_list.top = 400;
	region_maker_list.bottom = 475;

	region_bgmquit.left = 550;
	region_bgmquit.right = 730;
	region_bgmquit.top = 300;
	region_bgmquit.bottom = 375;

	region_start_game.left = 550;
	region_start_game.right = 730;
	region_start_game.top = 210;
	region_start_game.bottom = 285;

	region_quit_game.left = 550;
	region_quit_game.right = 730;
	region_quit_game.top = 570;
	region_quit_game.bottom = 645;

	region_game_selector.left = 550;
	region_game_selector.right = 730;
	region_game_selector.top = 480;
	region_game_selector.bottom = 555;

	region_set_button.left = 550;
	region_set_button.right = 730;
	region_set_button.top = 390;
	region_set_button.bottom = 465;

	region_music_button.left = 550;
	region_music_button.right = 730;
	region_music_button.top = 300;
	region_music_button.bottom = 375;

	region_music_selector1.left = 820;
	region_music_selector1.right = 1000;
	region_music_selector1.top = 400;
	region_music_selector1.bottom = 475;

	region_music_selector2.left = 280;
	region_music_selector2.right = 460;
	region_music_selector2.top = 400;
	region_music_selector2.bottom = 475;

	region_music_selector3.left = 280;
	region_music_selector3.right = 460;
	region_music_selector3.top = 275;
	region_music_selector3.bottom = 350;

	region_ranklist.left = 550;
	region_ranklist.right = 730;
	region_ranklist.top = 300;
	region_ranklist.bottom = 375;

	region_house_button.left = 1100;
	region_house_button.right = 1280;
	region_house_button.top = 645;
	region_house_button.bottom = 720;

	StartGamebutton start_game = StartGamebutton(region_start_game, _T("img/ui_start_idle.png"), _T("img/ui_start_hovered.png"), _T("img/ui_start_pushed.png"));
	QuitGamebutton quit_game = QuitGamebutton(region_quit_game, _T("img/ui_quit_idle.png"), _T("img/ui_quit_hovered.png"), _T("img/ui_quit_pushed.png"));
	Setbutton set_button = Setbutton(region_set_button, _T("img/ui_set_idle.jpg"), _T("img/ui_set_hovered.jpg"), _T("img/ui_set_pushed.jpg"));
	Musicbutton music_button = Musicbutton(region_music_button, _T("img/ui_music_idle.jpg"), _T("img/ui_music_hovered.jpg"), _T("img/ui_music_pushed.jpg"));
	Housebutton house_button = Housebutton(region_house_button, _T("img/ui_house_idle.jpg"), _T("img/ui_house_hovered.jpg"), _T("img/ui_house_pushed.jpg"));
	Musicselector1 music_selector1 = Musicselector1(region_music_selector1, _T("img/ui_music1_idle.jpg"), _T("img/ui_music1_hovered.jpg"), _T("img/ui_music1_pushed.jpg"));
	Musicselector2 music_selector2 = Musicselector2(region_music_selector2, _T("img/ui_music2_idle.png"), _T("img/ui_music2_hovered.png"), _T("img/ui_music2_pushed.png"));
	Musicselector3 music_selector3 = Musicselector3(region_music_selector3, _T("img/ui_music3_idle.png"), _T("img/ui_music3_hovered.png"), _T("img/ui_music3_pushed.png"));
	Ranklist rank_list = Ranklist(region_ranklist, _T("img/ui_rank_idle.jpg"), _T("img/ui_rank_hovered.jpg"), _T("img/ui_rank_pushed.jpg"));
	Gameselector game_selector = Gameselector(region_game_selector, _T("img/ui_gameselector_idle.jpg"), _T("img/ui_gameselector_hovered.jpg"), _T("img/ui_gameselector_pushed.jpg"));
	Controlbgmopen bgmopen = Controlbgmopen(region_bgmopen, _T("img/ui_bgm_open_idle.jpg"), _T("img/ui_bgm_open_hovered.jpg"), _T("img/ui_bgm_open_pushed.jpg"));
	Controlbgmquit bgmquit = Controlbgmquit(region_bgmquit, _T("img/ui_bgm_quit_idle.jpg"), _T("img/ui_bgm_quit_hovered.jpg"), _T("img/ui_bgm_quit_pushed.jpg"));
	Makerlist maker_list = Makerlist(region_maker_list, _T("img/ui_maker_list_idle.jpg"), _T("img/ui_maker_list_hovered.jpg"), _T("img/ui_maker_list_pushed.jpg"));
	Setupbutton setup_button = Setupbutton(region_setup_button, _T("img/setup.png"), _T("img/setup.png"), _T("img/setup.png"));
	Continuegame continue_game = Continuegame(region_continuegame_button, _T("img/ui_continue_game_idle.jpg"), _T("img/ui_continue_game_hovered.jpg"), _T("img/ui_continue_game_pushed.jpg"));
	Gameintroduction game_introduction = Gameintroduction(region_game_introduction, _T("img/ui_game_introduction_idle.jpg"), _T("img/ui_game_introduction_hovered.jpg"), _T("img/ui_game_introduction_pushed.jpg"));
	Gamechange game_change = Gamechange(region_gamechange, _T("img/ui_game_change_idle.jpg"), _T("img/ui_game_change_hovered.jpg"), _T("img/ui_game_change_pushed.jpg"));

	loadimage(&img_music_background, _T("img/music_background.png"), 1280, 720);
	loadimage(&img_game_background, _T("img/game_background.png"), 1280, 720);
	loadimage(&img_menu_background, _T("img/menu_background.jpg"), 1280, 720);
	loadimage(&img_rank_background, _T("img/rank_background.jpg"), 1280, 720);
	loadimage(&img_selector_background, _T("img/selector_background.jpg"), 1280, 720);
	loadimage(&img_introduction_background, _T("img/introduction.jpg"), 1280, 720);
	loadimage(&img_zanting_background, _T("img/zanting.jpg"), 1280, 720);
	loadimage(&img_makerlist, _T("img/maker_list.png"), 1280, 720);
	loadimage(&img_passage, _T("img/passage.png"), 1200, 700);
	loadimage(&img_victory, _T("img/victory.png"),800,540);
	loadimage(&img_title, _T("img/title.jpg"), 680, 160);
	flag = 1;
	int ranking = 1;//定义排位

	BeginBatchDraw();

	while (running) {
		if (score >= manba) {
			flag = 7;
		}
		DWORD starttime = GetTickCount();
		switch (flag) {
		case 1://菜单
			while (peekmessage(&msg)) {
				start_game.Processment(msg);
				quit_game.Processment(msg);
				rank_list.Processment(msg);
				set_button.Processment(msg);
				game_selector.Processment(msg);
			}
			cleardevice();
			putimage(0, 0, &img_menu_background);
			transparentimage(NULL, 300, 25, &img_title, WHITE);
			start_game.Draw();
			rank_list.Draw();
			set_button.Draw();
			quit_game.Draw();
			game_selector.Draw();
			FlushBatchDraw();
			break;

		case 2://排行榜界面
			while (peekmessage(&msg)) {
				house_button.Processment(msg);
			}
			cleardevice();
			//putimage(0, 0, &img_rank_background);
			ranking = 1;
			for (ii = RANK.begin(); ii != RANK.end(); ii++) {
				static TCHAR text[64], text2[64], text3[64];
				_stprintf_s(text, _T("排名"));
				_stprintf_s(text2, _T("名称"));
				_stprintf_s(text3, _T("得分"));
				settextstyle(30, 0, ANSI_CHARSET);
				settextcolor(RGB(240, 248, 255));
				outtextxy(200, 130, text);
				outtextxy(600, 130, text2);
				outtextxy(1000, 130, text3);
				static TCHAR str1[64], str[64], str2[64];
				_stprintf_s(str1, _T("%d"), ranking);//将数字格式化输出为字符串
				_stprintf_s(str, _T("%hs"), ii->id);
				if (ii->score) {
					outtextxy(200, 130 + ranking * textheight(str1) + 50, str1);
					outtextxy(600, 130 + ranking * textheight(str) + 50, str);
					_stprintf_s(str2, _T("%d"), ii->score);//将数字格式化输出为字符串
					outtextxy(1000, 130 + ranking * textheight(str2) + 50, str2);
					ranking++;
				}
			}
			house_button.Draw();
			FlushBatchDraw();
			break;

		case 3://选择音乐界面
			while (peekmessage(&msg)) {
				music_selector1.Processment(msg);
				music_selector2.Processment(msg);
				music_selector3.Processment(msg);
				house_button.Processment(msg);
			}
			cleardevice();
			putimage(0, 0, &img_music_background);
			music_selector1.Draw();
			music_selector2.Draw();
			music_selector3.Draw();
			house_button.Draw();
			FlushBatchDraw();
			break;

		case 4://设置界面
			while (peekmessage(&msg)) {
				music_button.Processment(msg);
				house_button.Processment(msg);
				game_change.Processment(msg);
			}
			cleardevice();
			putimage(0, 0, &img_menu_background);
			house_button.Draw();
			game_change.Draw();
			Drawattention();
			music_button.Draw();
			FlushBatchDraw();
			break;

		case 5://游戏界面
			while (peekmessage(&msg)) {
				setup_button.Processment(msg);
				if (score >= rando3)
					player_test4.processevent(msg);
				else if (score >= rando2 && score < rando3)
					player_test3.processevent(msg);
				else if (score >= rando1 && score < rando2)
					player_test2.processevent(msg);
				else player_test1.processevent(msg);
			}
			if (score >= rando3)
				player_test4.Move();
			else if (score >= rando2 && score < rando3)
				player_test3.Move();
			else if (score >= rando1 && score < rando2)
				player_test2.Move();
			else player_test1.Move();
			Trygenerateenemy(enemy_list);
			Trygenerateenemy2(enemy_list2);
			Trygenerateenemy3(enemy_list3);
			Trygenerateenemy4(enemy_list4);
			for (Enemy* enemy : enemy_list) {
				enemy->Move();
			}
			for (Enemy2* enemy : enemy_list2) {
				enemy->Move();
			}
			for (Enemy3* enemy : enemy_list3) {
				enemy->Move();
			}
			for (Enemy4* enemy : enemy_list4) {
				enemy->Move();
			}

			for (Enemy* enemy : enemy_list) {
				if (score > rando3) {
					if (enemy->checkplayercollision4(player_test4)) {
						if (enemy->grade < player_test4.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 10;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								flag_name = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else if (score >= rando2) {
					if (enemy->checkplayercollision3(player_test3)) {
						if (enemy->grade < player_test3.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 10;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								flag_name = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else if (score >= rando1 && score < rando2) {
					if (enemy->checkplayercollision2(player_test2)) {
						if (enemy->grade < player_test2.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 10;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								cur_position.x = 500;
								cur_position.y = 500;
								flag = 1;
								score = 0;
								flag_name = 0;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else {
					if (enemy->checkplayercollision1(player_test1)) {
						if (enemy->grade < player_test1.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 10;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								cur_position.x = 500;
								cur_position.y = 500;
								flag_name = 0;
								score = 0;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
			}

			for (Enemy2* enemy : enemy_list2) {
				if (score > rando3) {
					if (enemy->checkplayercollision4(player_test4)) {
						if (enemy->grade < player_test4.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 10;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								flag_name = 0;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else if (score >= rando2) {
					if (enemy->checkplayercollision3(player_test3)) {
						if (enemy->grade < player_test3.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 20;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								cur_position.x = 500;
								cur_position.y = 500;
								flag_name = 0;
								score = 0;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else if (score >= rando1 && score < rando2) {
					if (enemy->checkplayercollision2(player_test2)) {
						if (enemy->grade < player_test2.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 20;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								cur_position.x = 500;
								cur_position.y = 500;
								flag_name = 0;
								score = 0;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else {
					if (enemy->checkplayercollision1(player_test1)) {
						if (enemy->grade < player_test1.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 20;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								cur_position.x = 500;
								cur_position.y = 500;
								flag = 1;
								flag_name = 0;
								score = 0;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
			}

			for (Enemy3* enemy : enemy_list3) {
				if (score > rando3) {
					if (enemy->checkplayercollision4(player_test4)) {
						if (enemy->grade < player_test4.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 10;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								flag_name = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else if (score >= rando2) {
					if (enemy->checkplayercollision3(player_test3)) {
						if (enemy->grade < player_test3.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 30;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								flag_name = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else if (score >= rando1 && score < rando2) {
					if (enemy->checkplayercollision2(player_test2)) {
						if (enemy->grade < player_test2.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 30;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								flag_name = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else {
					if (enemy->checkplayercollision1(player_test1)) {
						if (enemy->grade < player_test1.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 30;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								flag_name = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
								
							}
							else running = false;
							break;
						}
					}
				}
			}

			for (Enemy4* enemy : enemy_list4) {
				if (score > rando3) {
					if (enemy->checkplayercollision4(player_test4)) {
						if (enemy->grade < player_test4.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 10;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								flag_name = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else if (score >= rando2) {
					if (enemy->checkplayercollision3(player_test3)) {
						if (enemy->grade < player_test3.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 30;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								flag_name = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else if (score >= rando1 && score < rando2) {
					if (enemy->checkplayercollision2(player_test2)) {
						if (enemy->grade < player_test2.grade) {
							enemy->Hurt();
							mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 30;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								flag_name = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();
							}
							else running = false;
							break;
						}
					}
				}
				else {
					if (enemy->checkplayercollision1(player_test1)) {
						if (enemy->grade < player_test1.grade) {
							enemy->Hurt();
							//mciSendString(_T("play hit from 0"), NULL, 0, NULL);
							score += 30;
						}
						else {
							static TCHAR text[124];
							_stprintf_s(text, _T("当前得分:%d,是否要返回主菜单?"), score);
							int temp = MessageBox(GetHWnd(), text, _T("游戏结束"), MB_YESNO);
							rank(score);
							Save_Rank();
							if (temp == 6) {
								flag = 1;
								score = 0;
								flag_name = 0;
								cur_position.x = 500;
								cur_position.y = 500;
								enemy_list.clear();
								enemy_list2.clear();
								enemy_list3.clear();
								enemy_list4.clear();
								flushmessage();

							}
							else running = false;
							break;
						}
					}
				}
			}

			for (auto i = 0; i < enemy_list.size(); i++) {
				Enemy* enemy = enemy_list[i];
				if (!enemy->CheckAlive()) {
					std::swap(enemy_list[i], enemy_list.back());
					enemy_list.pop_back();
					delete enemy;
				}
			}
			for (auto i = 0; i < enemy_list2.size(); i++) {
				Enemy2* enemy = enemy_list2[i];
				if (!enemy->CheckAlive()) {
					std::swap(enemy_list2[i], enemy_list2.back());
					enemy_list2.pop_back();
					delete enemy;
				}
			}
			for (auto i = 0; i < enemy_list3.size(); i++) {
				Enemy3* enemy = enemy_list3[i];
				if (!enemy->CheckAlive()) {
					std::swap(enemy_list3[i], enemy_list3.back());
					enemy_list3.pop_back();
					delete enemy;
				}
			}
			for (auto i = 0; i < enemy_list4.size(); i++) {
				Enemy4* enemy = enemy_list4[i];
				if (!enemy->CheckAlive()) {
					std::swap(enemy_list4[i], enemy_list4.back());
					enemy_list4.pop_back();
					delete enemy;
				}
			}

			cleardevice();
			putimage(0, 0, &img_game_background);
			setup_button.Draw();

			if (score >= rando3) {
				player_test4.Draw(1000 / 165);
				DrawPlayernext(manba);
			}
			else if (score >= rando2 && score < rando3) {
				player_test3.Draw(1000 / 165);
				DrawPlayernext(rando3);
			}
			else if (score >= rando1 && score < rando2) {
				player_test2.Draw(1000 / 165);
				DrawPlayernext(rando2);
			}
			else {
				player_test1.Draw(1000 / 165);
				DrawPlayernext(rando1);
			}

			for (Enemy* enemy : enemy_list) {
				enemy->Draw(1000 / 165);
			}
			for (Enemy2* enemy : enemy_list2) {
				enemy->Draw(1000 / 165);
			}
			for (Enemy3* enemy : enemy_list3) {
				enemy->Draw(1000 / 165);
			}
			for (Enemy4* enemy : enemy_list4) {
				enemy->Draw(1000 / 165);
			}
			DrawPlayerscore(score);
			FlushBatchDraw();
			break;

		case 6://游戏选项界面
			while (peekmessage(&msg)) {
				house_button.Processment(msg);
				bgmopen.Processment(msg);
				bgmquit.Processment(msg);
				maker_list.Processment(msg);
				game_introduction.Processment(msg);
			}
			cleardevice();
			putimage(0, 0, &img_selector_background);
			bgmopen.Draw();
			bgmquit.Draw();
			maker_list.Draw();
			game_introduction.Draw();
			house_button.Draw();
			FlushBatchDraw();
			break;

		case 7://胜利界面
			while (peekmessage(&msg)) {
				house_button.Processment(msg);
			}
			rank(score);
			Save_Rank();
			score = 0;
			cur_position.x = 500;
			cur_position.y = 500;
			enemy_list.clear();
			enemy_list2.clear();
			flag_name = 0;
			enemy_list3.clear();
			enemy_list4.clear();
			cleardevice();
			transparentimage(NULL, 240, 50, &img_victory, WHITE);
			house_button.Draw();
			FlushBatchDraw();
			break;

		case 8://暂停界面
			while (peekmessage(&msg)) {
				house_button.Processment(msg);
				continue_game.Processment(msg);
			}
			cleardevice();
			putimage(0, 0, &img_zanting_background);
			continue_game.Draw();
			house_button.Draw();
			FlushBatchDraw();
			break;

		case 9://制作名单界面
			while (peekmessage(&msg)) {
				house_button.Processment(msg);
			}
			cleardevice();
			putimage(0, 0, &img_makerlist);
			house_button.Draw();
			FlushBatchDraw();
			break;

		case 0://游戏功能介绍界面
			while (peekmessage(&msg)) {
				house_button.Processment(msg);
			}
			cleardevice();
			putimage(0, 0, &img_introduction_background);
			transparentimage(NULL, 10, 10, &img_passage, BLACK);
			house_button.Draw();
			FlushBatchDraw();
			break;

		default:
			break;
		}
		DWORD endtime = GetTickCount();
		DWORD delaytime = endtime - starttime;
		if (delaytime < 1000 / 165) {
			Sleep(1000 / 165 - delaytime);
		}
	}
	EndBatchDraw();
}