#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
int ancho, altura;
int scatter;

int movfantasma(int, int*, int*, int**, int*, int**,int*);
int checkmov(int, int*, int**);
int screenwrap(int**, int, int);
void movact(int, int*, int); 
void tscatter(int, int*);
void target(int, int*, int*, int*);
int interseccion(int, int*, int**, int*);
int distancia(int*, int*, int, int);
int intercheck(int, int, int*, int**, int*, int, int, int, int*);
void tblinky(int* P, int* T);
void tpinky(int* P, int* T, int* dir);
void tinky(int* P, int* T, int* dir);
void tclyde(int* P, int* T);