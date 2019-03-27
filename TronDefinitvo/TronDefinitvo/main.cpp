#include <iostream>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>



#include "SDL/include/SDL.h"
#include "SDL2_image-2.0.4/include/SDL_image.h"
#include "SDL2_mixer-2.0.4/include/SDL_mixer.h"

#pragma comment(lib, "SDL/lib1/w86/SDL2.lib")
#pragma comment(lib, "SDL/lib1/w86/SDL2main.lib")
#pragma comment(lib, "SDL2_image-2.0.4/lib1/w86/SDL2_image.lib")
#pragma comment(lib, "SDL2_mixer-2.0.4/lib1/w86/SDL2_mixer.lib")

using namespace std;

const int WIDTH = 1920;
const int HEIGHT = 1080;




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
SDL_Texture* bg_texture2 = nullptr;
SDL_Texture* bg_texture3 = nullptr;
SDL_Texture* bg_texture4 = nullptr;
SDL_Texture* bg_texture5 = nullptr;
SDL_Texture* bg_texture6 = nullptr;

SDL_Texture* ship_texture1 = nullptr;
SDL_Texture* ship_texture2 = nullptr;

SDL_Texture* gbullet_texture = nullptr;
SDL_Texture* pbullet_texture = nullptr;

SDL_Texture* gbeam_texture = nullptr;
SDL_Texture* pbeam_texture = nullptr;

//Input
Key_State keys[300];

//Rects
SDL_Rect bg_rect = { 0, 0, WIDTH, HEIGHT };
SDL_Rect ship_rect1 = {};
SDL_Rect ship_rect2 = {};
SDL_Rect gbullet = {};
SDL_Rect pbullet = {};

//Trail
SDL_Rect gPastpos[5000]{};
SDL_Rect pPastpos[5000]{};
SDL_Rect gtrail[5000] = {};
SDL_Rect ptrail[5000] = {};


bool gshot = false;
bool pshot = false;

bool gameRestart = false;
bool gamestarted = false;
bool gameEnd = false;

bool purpleWin = false;
bool greenWin = false;
bool draw = false;


bool render1 = true;
bool render2 = true;

float angle1 = 100.0f;
float angle2 = 280.0f;
float gshotAngle;
float pshotAngle;


const float speed = 2.0f;
const float bspeed = speed * 2;

float velx1;
float vely1;

float velx2;
float vely2;

float bvelx1;
float bvely1;

float bvelx2;
float bvely2;

int index = 0;

struct Vector2 { float x, y; };

Vector2 pos{ ship_rect1.x, ship_rect1.y };
Vector2 pos2{ ship_rect2.x, ship_rect2.y };
Vector2 bpos{ gbullet.x, gbullet.y };
Vector2 bpos2{ pbullet.x, pbullet.y };

SDL_RendererFlip flip = SDL_FLIP_NONE;
SDL_Point center = { 70/2, 58/2 };
SDL_Point centerb = { 16/2, 4/2 };

//Music
Mix_Music *bgmusic = nullptr;

//SFX
Mix_Chunk *sbeam = nullptr;
Mix_Chunk *explosion = nullptr;



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
		window = SDL_CreateWindow("SDLTest", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
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
			 
			int flags = MIX_INIT_OGG;
			if (!(Mix_Init(flags) & flags)) {

				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				return false;
			}
			else
			{
				Mix_Init(MIX_INIT_OGG);
				Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
				

			}
		}
	}
	return true;
}





void CleanupSDL()
{
	SDL_Quit();
	IMG_Quit();
	Mix_Quit();
}

void InitVariables()
{
	gamestarted = false;

	bg_texture = IMG_LoadTexture(renderer, "Textures/LOADING_SCREEN2.png");
	bg_texture2 = IMG_LoadTexture(renderer, "Textures/background.png");
	bg_texture3 = IMG_LoadTexture(renderer, "Textures/purple-wins.png");
	bg_texture4 = IMG_LoadTexture(renderer, "Textures/green-wins.png");
	bg_texture5 = IMG_LoadTexture(renderer, "Textures/LOADING_SCREEN.png");
	bg_texture6 = IMG_LoadTexture(renderer, "Textures/draw.png");

	ship_texture1 = IMG_LoadTexture(renderer, "Textures/spaceship-green.png");
	ship_texture2 = IMG_LoadTexture(renderer, "Textures/spaceship-purple.png");

	gbullet_texture = IMG_LoadTexture(renderer, "Textures/gbullet.png");
	pbullet_texture = IMG_LoadTexture(renderer, "Textures/pbullet.png");

	gbeam_texture = IMG_LoadTexture(renderer, "Textures/green-beams.png");
	pbeam_texture = IMG_LoadTexture(renderer, "Textures/purple-beams.png");
	

	bgmusic = Mix_LoadMUS("Music/bgmusic.ogg");
	Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	Mix_PlayMusic(bgmusic, -1);

	sbeam = Mix_LoadWAV("Music/sbeam.ogg");
	explosion = Mix_LoadWAV("SoundFX/explosion.wav");

	Mix_Volume(1, 20);

	ship_rect1 = { 117, (rand() % (HEIGHT - 300)) + 150, 141 / 2, 116 / 2 };
	ship_rect2 = { WIDTH - 300, (rand() % (HEIGHT - 141)) + 141, 141 / 2, 116 / 2 };


	pos = { (float)ship_rect1.x, (float)ship_rect1.y };
	pos2 = { (float)ship_rect2.x, (float)ship_rect2.y };
	bpos = { (float)gbullet.x, (float)gbullet.y };
	bpos2 = { (float)pbullet.x, (float)pbullet.y };


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

bool check_border(SDL_Rect a, SDL_Rect b)
{
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	leftA = a.x + 5;
	rightA = a.x + a.w - 5;
	topA = a.y + 5;
	bottomA = a.y + a.h - 5;

	leftB = 0;
	rightB = WIDTH;
	topB = 0;
	bottomB = HEIGHT;

	if (bottomA > bottomB) {
		return true;
	}
	if (topA < topB) {
		return true;
	}
	if (rightA > rightB) {
		return true;
	}
	if (leftA < leftB) {
		return true;
	}
	return false;
}
void setTrail() {
	if (index >= 2000)
	{
		index = 0;
	}

	for (int i = 0; i < 2000; i++) // Loop through all the n past positions of the player
	{
		if (!check_collision(gtrail[i], ship_rect2)) {

			gtrail[i] = { gPastpos[i].x, gPastpos[i].y, 12/2, 12/2 };
			SDL_RenderCopy(renderer, gbeam_texture, nullptr, &gtrail[i]);	
		}
		else {
			
			Mix_PlayChannel(-1, explosion, 0);
			render1 = false;
			render2 = false;
			purpleWin = false;
			greenWin = true;
			gameEnd = true;
		}
		if (!check_collision(ptrail[i], ship_rect1)) {

			ptrail[i] = { pPastpos[i].x, pPastpos[i].y, 12/2, 12/2 };
			SDL_RenderCopy(renderer, pbeam_texture, nullptr, &ptrail[i]);
		}
		else {
			
			Mix_PlayChannel(-1, explosion, 0);
			render1 = false;
			render2 = false;
			purpleWin = true;
			greenWin = false;
			gameEnd = true;
		}
		while (check_collision(gtrail[i], pbullet)) {

			gtrail[i] = { 0, 0, 0, 0 };
		}
		while (check_collision(ptrail[i], gbullet)) {

			ptrail[i] = { 0, 0, 0, 0 };
		}
	}

	SDL_Rect gtempRect = { ship_rect1.x, ship_rect1.y, 0, 0 };
	gPastpos[index] = gtempRect;

	SDL_Rect ptempRect = { ship_rect2.x, ship_rect2.y, 0, 0 };
	pPastpos[index] = ptempRect;

	index++; // This is to cycle through the array to store the new position

}
void Draw()
{
	SDL_RenderClear(renderer);


	if (!gamestarted) {

		SDL_RenderCopy(renderer, bg_texture, nullptr, &bg_rect);
		
	}
	else {
		SDL_RenderCopy(renderer, bg_texture2, nullptr, &bg_rect);
		

		if (purpleWin) {

			SDL_RenderCopy(renderer, bg_texture3, nullptr, &bg_rect);

		}
		else if (greenWin) {

			SDL_RenderCopy(renderer, bg_texture4, nullptr, &bg_rect);


		}
		else if (draw) {

			SDL_RenderCopy(renderer, bg_texture6, nullptr, &bg_rect);


		}
		else {
			
			setTrail();
			if (render1) {

				SDL_RenderCopyEx(renderer, ship_texture1, nullptr, &ship_rect1, angle1, &center, flip);
			}
			if (render2) {

				SDL_RenderCopyEx(renderer, ship_texture2, nullptr, &ship_rect2, angle2, &center, flip);
			}
			if (gshot) {
				SDL_RenderCopyEx(renderer, gbullet_texture, nullptr, &gbullet, gshotAngle, &centerb, flip);
			}
			if (pshot) {
				SDL_RenderCopyEx(renderer, pbullet_texture, nullptr, &pbullet, pshotAngle, &centerb, flip);
			}
		}

	}


	SDL_RenderPresent(renderer);
}
 



void Movement1() {

	if (angle1 >= 360)
	{
		angle1 = 0;
	}
	if (angle1 < 0)
	{
		angle1 = 359;
	}
	if (angle1 == 0)
	{
		vely1 = -speed;
		ship_rect1.y += vely1;
	}
	if (angle1 > 0 && angle1 < 90) {

		vely1 = speed * cos(angle1 * 0.01745329251);
		velx1 = speed * sin(angle1 * 0.01745329251);

		pos.x += velx1;
		pos.y -= vely1;
		ship_rect1.x = (int)pos.x;
		ship_rect1.y = (int)pos.y;
	}
	if (angle1 == 90)
	{
		velx1 = speed;
		ship_rect1.x += velx1;
	}

	if (angle1 > 90 && angle1 < 180) {

		vely1 = speed * -cos(angle1 * 0.01745329251);
		velx1 = speed * sin(angle1 * 0.01745329251);

		pos.x += velx1;
		pos.y += vely1;
		ship_rect1.x = (int)pos.x;
		ship_rect1.y = (int)pos.y;


	}
	if (angle1 == 180)
	{
		velx1 = 0;
		vely1 = speed;
		ship_rect1.x += velx1;
		ship_rect1.y += vely1;

	}
	if (angle1 > 180 && angle1 < 270) {

		vely1 = speed * -cos(angle1 * 0.01745329251);
		velx1 = speed * -sin(angle1 * 0.01745329251);

		pos.x -= velx1;
		pos.y += vely1;
		ship_rect1.x = (int)pos.x;
		ship_rect1.y = (int)pos.y;
	}
	if (angle1 == 270)
	{
		velx1 = -speed;
		vely1 = 0;
		ship_rect1.x += velx1;
		ship_rect1.y += vely1;

	}
	if (angle1 > 270 && angle1 < 360) {

		vely1 = speed * cos(angle1 * 0.01745329251);
		velx1 = speed * -sin(angle1 * 0.01745329251);

		pos.x -= velx1;
		pos.y -= vely1;
		ship_rect1.x = (int)pos.x;
		ship_rect1.y = (int)pos.y;
	}


}
void Movement2() {

	if (angle2 >= 360)
	{
		angle2 = 0;
	}
	if (angle2 < 0)
	{
		angle2 = 359;
	}
	if (angle2 == 0)
	{
		vely2 = -speed;
		ship_rect2.y += vely2;
	}
	if (angle2 > 0 && angle2 < 90) {

		vely2 = speed * cos(angle2 * 0.01745329251);
		velx2 = speed * sin(angle2 * 0.01745329251);

		pos2.x += velx2;
		pos2.y -= vely2;
		ship_rect2.x = (int)pos2.x;
		ship_rect2.y = (int)pos2.y;
	}
	if (angle2 == 90)
	{
		velx2 = speed;
		ship_rect2.x += velx2;
	}

	if (angle2 > 90 && angle2 < 180) {

		vely2 = speed * -cos(angle2 * 0.01745329251);
		velx2 = speed * sin(angle2 * 0.01745329251);

		pos2.x += velx2;
		pos2.y += vely2;
		ship_rect2.x = (int)pos2.x;
		ship_rect2.y = (int)pos2.y;


	}
	if (angle2 == 180)
	{
		velx2 = 0;
		vely2 = speed;
		ship_rect2.x += velx2;
		ship_rect2.y += vely2;

	}
	if (angle2 > 180 && angle2 < 270) {

		vely2 = speed * -cos(angle2 * 0.01745329251);
		velx2 = speed * -sin(angle2 * 0.01745329251);

		pos2.x -= velx2;
		pos2.y += vely2;
		ship_rect2.x = (int)pos2.x;
		ship_rect2.y = (int)pos2.y;
	}
	if (angle2 == 270)
	{
		velx2 = -speed;
		ship_rect2.x += velx2;
	}
	if (angle2 > 270 && angle2 < 360) {

		vely2 = speed * cos(angle2 * 0.01745329251);
		velx2 = speed * -sin(angle2 * 0.01745329251);

		pos2.x -= velx2;
		pos2.y -= vely2;
		ship_rect2.x = (int)pos2.x;
		ship_rect2.y = (int)pos2.y;
	}


}
void UpdateLogic()
{


	if (keys[SDL_SCANCODE_D] == KEY_REPEAT)
	{
		angle1 += (speed/2);
	}
	if (keys[SDL_SCANCODE_A] == KEY_REPEAT)
	{
		angle1 -= (speed / 2);
	}
	if (keys[SDL_SCANCODE_W] == KEY_REPEAT)
	{
		gbullet = { ((ship_rect1.x / 2 + (ship_rect1.w / 2)/2)), ship_rect1.y/2, 20/2, 40/2 };
		
		if (!gshot) {
			Mix_PlayChannel(-1, sbeam, 0);
			gshot = true;
		}
	}
	if (gshot) {

		

		if (gshotAngle >= 360)
		{
			gshotAngle = 0;
		}
		if (gshotAngle < 0)
		{
			gshotAngle = 359;
		}
		if (gshotAngle == 0)
		{
			gbullet.y += bvely1;
		}
		if (gshotAngle > 0 && gshotAngle < 90) {

			bpos.x += bvelx1;
			bpos.y -= bvely1;
			gbullet.x = (int)bpos.x;
			gbullet.y = (int)bpos.y;
		}
		if (gshotAngle == 90)
		{	
			gbullet.x += bvelx1;
		}

		if (gshotAngle > 90 && gshotAngle < 180) {

			
			bpos.x += bvelx1;
			bpos.y += bvely1;
			gbullet.x = (int)bpos.x;
			gbullet.y = (int)bpos.y;


		}
		if (gshotAngle == 180)
		{
			gbullet.y += bvely1;

		}
		if (gshotAngle > 180 && gshotAngle < 270) {

			bpos.x -= bvelx1;
			bpos.y += bvely1;
			gbullet.x = (int)bpos.x;
			gbullet.y = (int)bpos.y;
		}
		if (gshotAngle == 270)
		{
			gbullet.x += bvelx1;
			

		}
		if (gshotAngle > 270 && gshotAngle < 360) {
			bpos.x -= bvelx1;
			bpos.y -= bvely1;
			gbullet.x = (int)bpos.x;
			gbullet.y = (int)bpos.y;
		}
		
	}
	if (keys[SDL_SCANCODE_S] == KEY_REPEAT)
	{
		gshot = false;
	}

	if (keys[SDL_SCANCODE_LEFT] == KEY_REPEAT)
	{
		angle2 -= (speed / 2);
	}
	if (keys[SDL_SCANCODE_RIGHT] == KEY_REPEAT)
	{
		angle2 += (speed / 2);
	}
	if (keys[SDL_SCANCODE_UP] == KEY_REPEAT)
	{
		pbullet = {(ship_rect2.x + (ship_rect2.w / 2)), ship_rect2.y, 20/2, 40/2 };
		
			Mix_PlayChannel(-1, sbeam, 0);
			pshot = true;
		
	}
	if (pshot) {

		if (pshotAngle >= 360)
		{
			pshotAngle = 0;
		}
		if (pshotAngle < 0)
		{
			pshotAngle = 359;
		}
		if (pshotAngle == 0)
		{
			pbullet.y += bvely2;
		}
		if (pshotAngle > 0 && pshotAngle < 90) {

			bpos2.x += bvelx2;
			bpos2.y -= bvely2;
			pbullet.x = (int)bpos2.x;
			pbullet.y = (int)bpos2.y;
		}
		if (pshotAngle == 90)
		{
			pbullet.x += bvelx2;
		}

		if (pshotAngle > 90 && pshotAngle < 180) {

			bpos2.x += bvelx2;
			bpos2.y += bvely2;
			pbullet.x = (int)bpos2.x;
			pbullet.y = (int)bpos2.y;


		}
		if (pshotAngle == 180)
		{
			pbullet.y += bvely2;

		}
		if (pshotAngle > 180 && pshotAngle < 270) {

			bpos2.x -= bvelx2;
			bpos2.y += bvely2;
			pbullet.x = (int)bpos2.x;
			pbullet.y = (int)bpos2.y;
		}
		if (pshotAngle == 270)
		{
			bvelx2 = -speed;
			bvely2 = 0;
			pbullet.x += bvelx2;
			pbullet.y += bvely2;

		}
		if (pshotAngle > 270 && pshotAngle < 360) {

			bpos2.x -= bvelx2;
			bpos2.y -= bvely2;
			pbullet.x = (int)bpos2.x;
			pbullet.y = (int)bpos2.y;
		}
	}

	if (keys[SDL_SCANCODE_DOWN] == KEY_REPEAT)
	{
		pshot = false;
	}
	if (keys[SDL_SCANCODE_SPACE] == KEY_DOWN)
	{
		gamestarted = true;
	}
}

int main(int argc, char* argv[])
{
	srand(time(NULL));

	
	
	

	if (InitSDL())
	{
		InitVariables();
		while (ProcessInput())
		{
			UpdateLogic();

			if (check_collision(ship_rect1, ship_rect2) && !gameEnd) {

				Mix_PlayChannel(-1, explosion, 0);
				SDL_Delay(10);
				render1 = false;
				render2 = false;
				draw = true;
				gameEnd = true;
				
				
			}
			if (check_border(ship_rect1, bg_rect) && !gameEnd) {

				Mix_PlayChannel(-1, explosion, 0);
				SDL_Delay(10);
				render1 = false;
				render2 = false;
				purpleWin = true;
				greenWin = false;
				gameEnd = true;

			}
			if (check_border(ship_rect2, bg_rect) && !gameEnd) {

				Mix_PlayChannel(-1, explosion, 0);
				SDL_Delay(10);
				render1 = false;
				render2 = false;
				purpleWin = false;
				greenWin = true;
				gameEnd = true;

			}
			if (check_collision(gbullet, ship_rect2) && !gameEnd) {

				Mix_PlayChannel(-1, explosion, 0);
				SDL_Delay(10);
				render1 = false;
				render2 = false;
				greenWin = true;
				gameEnd = true;



			}
			if (check_collision(pbullet, ship_rect1) && !gameEnd) {

				Mix_PlayChannel(-1, explosion, 0);
				SDL_Delay(10);
				render1 = false;
				render2 = false;
				purpleWin = true;
				gameEnd = true;

			}

			if (!gshot) {

				gshotAngle = angle1;
				
				bpos.x = (ship_rect1.x + 58) + (sin(angle1*0.01745329251) * 58);
				bpos.y = ((ship_rect1.y + 71) + (cos(angle1*0.01745329251) * 71));
				gbullet.x = bpos.x;
				gbullet.y = bpos.y;

				bvelx1 = velx1 * bspeed;
				bvely1 = vely1 * bspeed;
				
				gshot = false;
			}
			if (!pshot) {

				pshotAngle = angle2;

				bpos2.x = (ship_rect2.x + 58) + (sin(angle2*0.01745329251) * 58);
				bpos2.y = ((ship_rect2.y + 71) + (cos(angle2*0.01745329251) * 71));
				pbullet.x = bpos2.x;
				pbullet.y = bpos2.y;

				bvelx2 = velx2 * bspeed;
				bvely2 = vely2 * bspeed;

				pshot = false;
			}
			Draw();
			
			if (gamestarted) {
				Movement1();
				Movement2();
			}
			
			
			
			if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN) {

				SDL_Quit();
				break;

			}
		}
	}

	CleanupSDL();
	return 0;
}
