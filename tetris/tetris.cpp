#include <stdlib.h>
#include <time.h>
#include <SDL.h>

constexpr int WIDTH = 10;            // 宽和高;
constexpr int HEIGHT = 20;

enum blockType {
	nothing, blockp, blockk, block, block1, block2, block3, block4, block5, block6, block7,
} map[WIDTH + 2][HEIGHT + 3]; // 地图

int socre = 0;                  // 分数
int nexttype, nowtype;                   // 下一组方块类型
SDL_Window * window;
SDL_Renderer *b;

int testandbuild(int x, int y, int type, int direction); // 测试在目标位置是否能放置目标类型的万块
void display();                 // 显示
int testtheline(int line);
clock_t mark;

const short blockdate[7][4][4][2] = {
	0, 0, 1, 0, 1, 1, 2, 0,
	1, 1, 1, 2, 0, 1, 1, 0,
	1, 1, 0, 1, 1, 0, 2, 1,
	0, 1, 0, 0, 1, 1, 0, 2,       // 
	0, 0, 0, 1, 1, 1, 1, 2,
	0, 1, 1, 1, 1, 0, 2, 0,
	0, 0, 0, 1, 1, 1, 1, 2,
	0, 1, 1, 1, 1, 0, 2, 0,       // 
	0, 0, 1, 0, 2, 0, 2, 1,
	1, 0, 1, 1, 1, 2, 0, 2,
	0, 0, 0, 1, 1, 1, 2, 1,
	1, 0, 0, 0, 0, 1, 0, 2,       // 
	0, 0, 1, 0, 0, 1, 2, 0,
	0, 0, 1, 0, 1, 1, 1, 2,
	0, 1, 1, 1, 2, 1, 2, 0,
	0, 0, 0, 1, 0, 2, 1, 2,       // 
	0, 1, 1, 1, 2, 1, 3, 1,
	1, 0, 1, 1, 1, 2, 1, 3,
	0, 1, 1, 1, 2, 1, 3, 1,
	1, 0, 1, 1, 1, 2, 1, 3,       // 
	0, 0, 0, 1, 1, 0, 1, 1,
	0, 0, 0, 1, 1, 0, 1, 1,
	0, 0, 0, 1, 1, 0, 1, 1,
	0, 0, 0, 1, 1, 0, 1, 1,       // 
	0, 0, 1, 0, 1, 1, 2, 1,
	1, 0, 1, 1, 0, 1, 0, 2,
	0, 0, 1, 0, 1, 1, 2, 1,
	1, 0, 1, 1, 0, 1, 0, 2,       // 这里早就分不清了
};

int main(int args, char ** argv) {
	/*******************************************声明变量************************************************************/
	bool quit = false;
	int  dire, dot, x, y, socretemp = 0; // dot是下落间隔的时间，毫秒
	SDL_Event event;//事件队列
	/*********************************************SDL初始化*****************************************************/
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	window = SDL_CreateWindow("tetris.by starstory", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH * 20 + 180, HEIGHT * 20 + 60, SDL_WINDOW_SHOWN);//创建窗口
	b = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(b, 255, 255, 255, 255);
	SDL_RenderClear(b);
	SDL_RenderPresent(b);
	/**************************************地图边框***********************************************/
	for (int a = 0; a <= HEIGHT + 2; a++) {
		map[0][a] = blockk;
		map[WIDTH + 1][a] = blockk;
	}
	for (int a = 1; a <= WIDTH; a++)
		map[a][HEIGHT + 2] = blockk;    // 填充边框
	/*************************随机数*****************************************/
	srand((unsigned)time(NULL));
	nexttype = rand() % 7;
	socre = 0;
	mark = clock();
	dot = CLOCKS_PER_SEC / (mark / CLOCKS_PER_SEC / 60 + 1);
	/****************************游戏运行主框架************************************/
	while (1) {
		if (testandbuild(x = 4, y = 0, nowtype = nexttype, dire = 0) == -1)
			break;                   // 输了，返回
		nexttype = rand() % 7;
		display();
		while (1) {//游戏下落部分
			if (clock() - mark > dot) {

				mark = clock();         // 重新标记时间
				if (testandbuild(x, y + 1, nowtype, dire) == 0) // 本句下落
					y++;                  // 如果成功 将标记位置下移
				else {
					for (int a = 0; a < HEIGHT + 2; a++)
						for (int b = 1; b <= WIDTH; b++)
							if (map[b][a] == blockp)
								switch (nowtype)
								{
								case 0:map[b][a] = block1;
									break;
								case 1:map[b][a] = block2;
									break;
								case 2:map[b][a] = block3;
									break;
								case 3:map[b][a] = block4;
									break;
								case 4:map[b][a] = block5;
									break;
								case 5:map[b][a] = block6;
									break;
								case 6:map[b][a] = block7;
									break;
								}
					break;                // 没有成动下落 退出下落循环
										  // 固化临时方块
				}
			}
			while (SDL_PollEvent(&event)) {
				switch (event.type)
				{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_w:             // 旋转
						if (testandbuild(x, y, nowtype, (dire + 1) % 4) == 0)
							dire = (dire + 1) % 4;
						break;
					case SDLK_a:             // 左移
						if (testandbuild(x - 1, y, nowtype, dire) == 0)
							x--;
						break;
					case SDLK_d:            // 右移
						if (testandbuild(x + 1, y, nowtype, dire) == 0)
							x++;
						break;
					case SDLK_s:           // 快速向下
						dot = CLOCKS_PER_SEC / 39; // X这里采用的是减小间隔时间的方法
						break;
					case SDLK_SPACE:
						dot = 0; // X这里采用的是减小间隔时间的方法
						break;
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
					case SDLK_s:           // 快速向的还原
						dot = CLOCKS_PER_SEC / (mark / CLOCKS_PER_SEC / 10 + 2);
						break;
					}
				}
				display();
			}
			if (quit)break;
		}
		for (int i = 0; i < HEIGHT + 2; i++)
			socretemp += testtheline(i); // 计算消去的行数
		socre += socretemp * socretemp; // 一次消去的行数越多 额外奖历越大
		socretemp = 0;
		dot = CLOCKS_PER_SEC / (mark / CLOCKS_PER_SEC / 10 + 2); // 恢复间隔时间
		if (quit)break;
	}
	/*********************************游戏结束************************************************/
	while (1)
	{
		SDL_PollEvent(&event);
		if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) || event.type == SDL_QUIT)
			break;
	}
	SDL_Quit();
	return 0;
}

void display() {
	SDL_SetRenderDrawColor(b, 255, 255, 255, 255);
	SDL_RenderClear(b);

	SDL_Rect a;
	for (int y = 2; y < HEIGHT + 3; y++)  // 纵向多留出2个单位
		for (int x = 0; x < WIDTH + 2; x++)  // 横向多留出8个单位
			if (map[x][y] != nothing)
			{
				a.x = x * 20;
				a.y = y * 20 - 40;
				a.h = a.w = 20;
				switch (map[x][y])
				{
				case blockk:
				case blockp:
					switch (nowtype)
					{
					case 0:SDL_SetRenderDrawColor(b, 0, 0, 0, 255);
						break;
					case 1:SDL_SetRenderDrawColor(b, 255, 0, 0, 255);
						break;
					case 2:SDL_SetRenderDrawColor(b, 0, 255, 0, 255);
						break;
					case 3:SDL_SetRenderDrawColor(b, 0, 0, 255, 255);
						break;
					case 4:SDL_SetRenderDrawColor(b, 255, 255, 0, 255);
						break;
					case 5:SDL_SetRenderDrawColor(b, 255, 0, 255, 255);
						break;
					case 6:SDL_SetRenderDrawColor(b, 0, 255, 255, 255);
						break;
					}
					break;
				case block1:SDL_SetRenderDrawColor(b, 0, 0, 0, 255);
					break;
				case block2:SDL_SetRenderDrawColor(b, 255, 0, 0, 255);
					break;
				case block3:SDL_SetRenderDrawColor(b, 0, 255, 0, 255);
					break;
				case block4:SDL_SetRenderDrawColor(b, 0, 0, 255, 255);
					break;
				case block5:SDL_SetRenderDrawColor(b, 255, 255, 0, 255);
					break;
				case block6:SDL_SetRenderDrawColor(b, 255, 0, 255, 255);
					break;
				case block7:SDL_SetRenderDrawColor(b, 0, 255, 255, 255);
					break;
				}
				SDL_RenderFillRect(b, &a);
			}



	for (int i = 0; i < 4; i++)
	{
		a.x = blockdate[nexttype][0][i][0] * 20 + WIDTH * 20 + 80;
		a.y = blockdate[nexttype][0][i][1] * 20 + 40;
		a.h = a.w = 20;
		switch (nexttype)
		{
		case 0:SDL_SetRenderDrawColor(b, 0, 0, 0, 255);
			break;
		case 1:SDL_SetRenderDrawColor(b, 255, 0, 0, 255);
			break;
		case 2:SDL_SetRenderDrawColor(b, 0, 255, 0, 255);
			break;
		case 3:SDL_SetRenderDrawColor(b, 0, 0, 255, 255);
			break;
		case 4:SDL_SetRenderDrawColor(b, 255, 255, 0, 255);
			break;
		case 5:SDL_SetRenderDrawColor(b, 255, 0, 255, 255);
			break;
		case 6:SDL_SetRenderDrawColor(b, 0, 255, 255, 255);
			break;
		}
		SDL_RenderFillRect(b, &a);
	}
	SDL_RenderPresent(b);
}

int testandbuild(int xb, int yb, int type, int direction) {
	int x[4], y[4];               // 四方块坐标
	for (int i = 0; i < 4; i++) { // 获得四方块坐标
		x[i] = xb + blockdate[type][direction][i][0];
		y[i] = yb + blockdate[type][direction][i][1];
		if (map[x[i]][y[i]] == blockk || map[x[i]][y[i]] >= block)
			return -1;                // 检测该坐标上是否有方块
	}
	for (int a = 0; a < HEIGHT + 2; a++) // 清除所有临时方块
		for (int b = 0; b <= WIDTH; b++)
			if (map[b][a] == blockp)
				map[b][a] = nothing;
	for (int i = 0; i < 4; i++)
		map[x[i]][y[i]] = blockp;

	display();
	return 0;
}

int testtheline(int line) {
	int num = 0;
	for (int i = 1; i <= WIDTH; i++)
		if (map[i][line] >= block)
			num++;
	if (num == WIDTH) {
		for (int a = line; a > 0; a--)
			for (int b = 1; b <= WIDTH; b++)
				map[b][a] = map[b][a - 1];
		for (int i = 1; i <= WIDTH; i++)
			map[i][0] = nothing;
		return 1;
	}
	else
		return 0;
}
