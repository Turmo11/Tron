#include <iostream>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>


#include "SDL/include/SDL.h"
#include "SDL2_image-2.0.4/include/SDL_image.h"
//#include "SDL2_mixer-2.0.4/include/SDL_mixer.h"

#pragma comment(lib, "SDL/lib1/w86/SDL2.lib")
#pragma comment(lib, "SDL/lib1/w86/SDL2main.lib")
#pragma comment(lib, "SDL2_image-2.0.4/lib1/w86/SDL2_image.lib")
//#pragma comment(lib, "SDL2_mixer-2.0.4/lib1/w86/SDL2_mixer.lib")

using namespace std;




enum Key_State
{
	KEY_IDLE,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Surface* surface = nullptr;
SDL_Surface* loadedImage = nullptr;

//Textures
SDL_Texture* bg_texture = nullptr;
SDL_Texture* ship_texture1 = nullptr;
SDL_Texture* ship_texture2 = nullptr;

//Input
Key_State keys[300];

//Rects
SDL_Rect bg_rect = { 0, 0, 1920, 1080 };
SDL_Rect ship_rect1 = {};
SDL_Rect ship_rect2 = {};
bool render = true;

//Music
//Mix_Music *bgmusic = NULL;


bool InitSDL()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		cout << "Wrong initialization" << endl;
		return false;
	}
	else
	{
		window = SDL_CreateWindow("SDLTest", 0, 0, 1920, 1080, SDL_WINDOW_SHOWN);
		if (window == nullptr)
		{
			cout << "Window not created" << endl;
			return false;
		}
		else
		{
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			//Initialize SDL_Image
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				return false;
			}
			else
			{
				surface = SDL_GetWindowSurface(window);
			}

			//int flags = MIX_INIT_OGG | MIX_INIT_MOD;
			//int initted = Mix_Init(flags);

			//if (initted&flags != flags) {
			//	printf("Mix_Init: Failed to init required ogg and mod support!\n");
			//	printf("Mix_Init: %s\n", Mix_GetError());
			//	// handle error
			//	SDL_Quit();
			//}
			//else
			//{
			//	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);

			//}
		}


	}

	return true;
}

void CleanupSDL()
{
	SDL_Quit();
	IMG_Quit();
	//Mix_Quit();
}

void InitVariables()
{
	bg_texture = IMG_LoadTexture(renderer, "Textures/background.png");
	ship_texture1 = IMG_LoadTexture(renderer, "Textures/spaceship-green.png");
	ship_texture2 = IMG_LoadTexture(renderer, "Textures/spaceship-purple.png");
	//bgmusic = Mix_LoadMUS("Music/NeonRunner.ogg");


}

bool ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case(SDL_QUIT):
		{
			return false;
		}
		}
	}

	const Uint8* state = SDL_GetKeyboardState(nullptr);
	for (int i = 0; i < 300; ++i)
	{
		if (state[i])
		{
			keys[i] = (keys[i] == KEY_IDLE ? KEY_DOWN : KEY_REPEAT);
		}
		else
		{
			keys[i] = (keys[i] == KEY_REPEAT ? KEY_UP : KEY_IDLE);
		}
	}

	return true;
}

void Draw()
{
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, bg_texture, nullptr, &bg_rect);
	
	if (render) {

		SDL_RenderCopy(renderer, ship_texture1, nullptr, &ship_rect1);
		SDL_RenderCopy(renderer, ship_texture2, nullptr, &ship_rect2);
	}


	SDL_RenderPresent(renderer);
}

bool check_collision(SDL_Rect a, SDL_Rect b)
{
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	leftA = a.x + 10;
	rightA = a.x + a.w - 10;
	topA = a.y + 10;
	bottomA = a.y + a.h - 10;
	
	leftB = b.x + 10;
	rightB = b.x + b.w - 10;
	topB = b.y + 10;
	bottomB = b.y + b.h - 10;

	if (bottomA <= topB) {
		return false;
	}
	if (topA >= bottomB) {
		return false;
	}
	if (rightA <= leftB) {
		return false;
	}
	if (leftA >= rightB) {
		return false;
	}
	return true;
}

void UpdateLogic()
{

	if (keys[SDL_SCANCODE_D] == KEY_REPEAT)
	{
		ship_rect1.x += 3;
		if (ship_rect1.x > 1920 - ship_rect1.w) ship_rect1.x = 1920 - ship_rect1.w;

	}
	if (keys[SDL_SCANCODE_A] == KEY_REPEAT)
	{
		ship_rect1.x -= 3;
		if (ship_rect1.x < 0) ship_rect1.x = 0;
	}
	if (keys[SDL_SCANCODE_W] == KEY_REPEAT)
	{
		ship_rect1.y -= 3;
		if (ship_rect1.y < 0) ship_rect1.y = 0;

	}
	if (keys[SDL_SCANCODE_S] == KEY_REPEAT)
	{
		ship_rect1.y += 3;
		if (ship_rect1.y > 1080 - ship_rect1.h) ship_rect1.y = 1080 - ship_rect1.h;

	}
	if (keys[SDL_SCANCODE_SPACE] == KEY_DOWN)
	{

	}
}

int main(int argc, char* argv[])
{
	srand(time(NULL));

	ship_rect1 = { (rand() % 828) + 116 , (rand() % 798) + 141, 141, 116 };
	ship_rect2 = { (rand() % 728) + 1076, (rand() % 798) + 141, 141, 116 };

	if (InitSDL())
	{
		InitVariables();
		while (ProcessInput())
		{
			UpdateLogic();
			if (check_collision(ship_rect1, ship_rect2)) {
				
				SDL_Delay(10);
				render = false;
				
			}
				Draw();

			if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN) {

				SDL_Quit();
				break;
			}
		}
	}

	CleanupSDL();
	return 0;
}
