#pragma once
#include <time.h>
#include <stdio.h>
#include <SDL.h>
#include <conio.h>
int inputdec(Uint8* tec, int** L, int** I, int* p, int* t, int* m, int* d, int*o);
void renderObject(SDL_Rect* objdest, SDL_Renderer* renderer, SDL_Texture* textur, int* dir, int* pos, int* mov, int dim, int obj, int t, int upt);
int losecon(int* mov, int* dir, int* pos, int obj, int t, int dim, int upt);
void calculo(float* mx, float* my, int mov, int dir, int dim, int upt, int t);
int points;
int powerup;