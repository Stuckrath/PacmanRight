#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>
int casillas;
char c, aux;
int i, j, val,pointstotal, points, tiempo, powerup;
int** crearLaberinto(int* ancho, int* altura, char* fp, int* pos);
int** crearIntersecciones(int** Laberinto, int altura, int ancho);
void DibujarMapa(int** M, int i, int j, SDL_Renderer* renderer, SDL_Rect* mur, SDL_Rect* punt, int dim);
void renderObject(SDL_Rect* objdest, SDL_Renderer* renderer, SDL_Texture* textur, int* dir, int* pos, int* mov, int dim, int obj, int t, int upt);
void renderPacman(SDL_Rect* objdest, SDL_Renderer* renderer, SDL_Texture* textur, int* dir, int* pos, int* mov, int dim, int obj, int t, int upt);
void pointcheck(int** M, int x, int y);
void DrawText(SDL_Rect* obj, TTF_Font* font, SDL_Color color, SDL_Renderer* rend, char* texto);
void AddInfo(SDL_Renderer* rend, SDL_Rect* inf, SDL_Rect* poi, SDL_Rect* tim, SDL_Rect* it, SDL_Rect* ip, TTF_Font* font, SDL_Color color);