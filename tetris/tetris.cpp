//#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
#include <time.h>
#include <SDL.h>
// 包含头文件

#define ROLL 'w'                // 旋转 左移 右移
// 下落分别对应的键
#define LIFT 'a'
#define RIGHT 'd'
#define DOTTOM 's'

#define WIDTH 10            // 宽和高
#define HEIGHT 20

enum blockType { nothing, block, blockp, blockk } map[WIDTH + 2][HEIGHT + 1]; // 地图

int socre = 0;                  // 分数
int nexttype;                   // 下一组方块类型
SDL_Window * window;
SDL_Renderer *b;


//void initmap();
void rungame();
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
//int fps = 0;


int main(int args, char ** argv) {
	//wellcome();
	////_getch();                      // 暂停
	//initmap();                    // 初始化地图
	//rungame();                    // 游戏运行
	//outtextxy(0, 0, "game over");
	//_getch();
	bool quit = false;

	socre = 0;
	for (int a = 0; a <= HEIGHT; a++) {
		map[0][a] = blockk;
		map[WIDTH + 1][a] = blockk;
	}
	//map = (blockType**)malloc(sizeof(blockType)*(WIDTH + 2)*(HEIGHT + 1));

	for (int a = 1; a <= WIDTH; a++)
		map[a][HEIGHT] = blockk;    // 填充边框
	srand((unsigned)time(NULL));
	nexttype = rand() % 7;

	mark = clock();
	int nowtype, dire, dot = 333, x, y, socretemp = 0; // dot是下落间隔的时间，毫秒

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	window = SDL_CreateWindow("tetris.by starstory", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);//创建窗口
	SDL_Event event;//事件队列
	b = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(b, 255, 255, 255, 255);
	SDL_RenderClear(b);
	SDL_RenderPresent(b);

	while (1) {
		if (testandbuild(x = 4, y = 0, nowtype = nexttype, dire = 0) == -1)
			return 0;                   // 输了，返回
		nexttype = rand() % 7;
		display();
		while (1) {
			if (clock() - mark > dot) {

				mark = clock();         // 重新标记时间
				if (testandbuild(x, y + 1, nowtype, dire) == 0) // 本句下落
					y++;                  // 如果成功 将标记位置下移
				else {
					for (int a = 0; a < HEIGHT; a++)
						for (int b = 1; b <= WIDTH; b++)
							if (map[b][a] == blockp)
								map[b][a] = block;

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
						dot = 0; // X这里采用的是减小间隔时间的方法
						break;
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;
				}
				display();
			}
			if (quit)break;
		}
		for (int i = 0; i < HEIGHT; i++)
			socretemp += testtheline(i); // 计算消去的行数
		socre += socretemp * socretemp * WIDTH; // 一次消去的行数越多 额外奖历越大
		socretemp = 0;
		dot = CLOCKS_PER_SEC / 3; // 恢复间隔时间
		if (quit)break;
	}


	SDL_Quit();
	return 0;
}

//void wellcome() {
//	//char str[20];
//	//InputBox(str, 20, "自定义宽度", "自定义选项", "10");
//	//WIDTH = atol(str);
//	//InputBox(str, 20, "自定义高度", "自定义选项", "20");
//	//HEIGHT = atol(str);
//
//	//printf("俄罗斯方块\n操作说明:\nw旋转\ns下落\na向左\nd向右\n按任意键继续...");
//}

//void initmap() {                // 初使化
//	//initgraph((WIDTH + 10)*20,(HEIGHT + 1)*20);
//	//setbkcolor(RGB(255, 255, 255));
//	//setfillcolor(RGB(0, 0, 0));
//	//setlinecolor(RGB(255, 255, 255));
//	//setlinestyle(PS_SOLID, 3);
//	//settextcolor(RGB(0, 0, 0));
//

void rungame() {

}



void display() {
	//										 // 用来放下一个方块的数据
	//system("cls");
	//printf("%d,%d\n", fps++, mark / CLOCKS_PER_SEC);
	//char str[10];
	//sprintf(str, "%d", socre);
	//BeginBatchDraw();
	//cleardevice();
	SDL_SetRenderDrawColor(b, 255, 255, 255, 255);
	SDL_RenderClear(b);
	//SDL_RenderPresent(b);

	SDL_Rect a;
	for (int y = 0; y < HEIGHT + 1; y++)  // 纵向多留出2个单位
		for (int x = 0; x < WIDTH + 2; x++)  // 横向多留出8个单位
			if (map[x][y] != nothing)
			{
				a.x = x * 20;
				a.y = y * 20;
				a.h = a.w = 20;
				SDL_SetRenderDrawColor(b, 0, 0, 0, 255);
				SDL_RenderFillRect(b, &a);
				//SDL_RenderPresent(b);
			}

	//fillrectangle(x * 20, y * 20, (x + 1) * 20, (y + 1) * 20);



	for (int i = 0; i < 4; i++)
	{
		a.x = blockdate[nexttype][0][i][0] * 20 + 280;
		a.y = blockdate[nexttype][0][i][1] * 20 + 60;
		a.h = a.w = 20;
		SDL_SetRenderDrawColor(b, 0, 0, 0, 255);
		SDL_RenderFillRect(b, &a);
		//SDL_RenderPresent(b);
	}
	//	fillrectangle(
	//		blockdate[nexttype][0][i][0] * 20 + 280,
	//		blockdate[nexttype][0][i][1] * 20 + 60,
	//		blockdate[nexttype][0][i][0] * 20 + 300,
	//		blockdate[nexttype][0][i][1] * 20 + 80);
	//outtextxy(250, 250, str);
	//EndBatchDraw();
	SDL_RenderPresent(b);
}



int testandbuild(int xb, int yb, int type, int direction) {
	int x[4], y[4];               // 四方块坐标
	for (int i = 0; i < 4; i++) { // 获得四方块坐标
		x[i] = xb + blockdate[type][direction][i][0];
		y[i] = yb + blockdate[type][direction][i][1];
		if (map[x[i]][y[i]] == blockk || map[x[i]][y[i]] == block)
			return -1;                // 检测该坐标上是否有方块
	}
	for (int a = 0; a < HEIGHT; a++) // 清除所有临时方块
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
		if (map[i][line] == block)
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
