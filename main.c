#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "MatrizYRender.h"
#include "posandmov.h"
#include "timeandinput.h"


int ancho = 0, altura = 1, scatter = 1, powerup, angulo = 0, points=0, pointstotal;
int dim = 32;
int const uptps = 8, tmov=100;
int tiempo = 0;

int main(int argc, char* argv[])
{   //Agrega laberintos\\ a la string que se usara como file pointer, para no tener que agregar la carpeta al input
    printf("Ingresar el nombre del archivo .txt a leer (.txt incluido) (Asegurar que se encuentre en la carpeta Laberintos):\n");
    char Arch[96] = "Laberintos\\";
    char test = getchar();
    int ii = 0;
    while (test != '\n') {
        Arch[ii + 11] = test;
        ii++;
        test = getchar();
    }
    int pos[10];
    int** MAPA = crearLaberinto(&ancho, &altura, Arch, pos);
    printf("Comandos:\nW o UP para mover hacia arriba\nD o RIGHT para mover hacia la derecha\nS o DOWN para mover hacia abajo");
    printf("\nA o LEFT para mover hacia la derecha\nH para guardar la partida (la puedes cargar al momento de abrir el archivo)\n Buena suerte!\n");
    ancho--;
    //La matriz de intersecciones se utiliza para facilitar la IA de los fantasmas, solo tienen que actualizar su objetivo cuando estan en una interseccion
    int** Inter = crearIntersecciones(MAPA, altura, ancho);
    int mov[5] = { 0,0,0,0,0 }; //ultimo movimiento de cada objeto
    int tar[10] = { 10,10,10,10,20,20,5,5,15,15 }; //coord. X e Y de la casilla objetivo de cada objeto (pacman no la utiliza, solo estan ahi por conveniencia)
    int dir[5] = { 2,2,2,2,2 }; //direccion de cada objeto
    int ov[5] = { 0,0,0,0,0 }; //si el movimiento de cada objeto genera screenwrap o no
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError()); return 0;
    }
    if (altura >= 30 || ancho >=40) {
        dim = 24; //ajusta el tamaño de la casilla si es demasiado grande
        if (altura >= 50 || ancho >= 60) {
            dim = 16;
        }
    }
    if (TTF_Init() == -1) {
        return 0;
    }

    SDL_Window* window = SDL_CreateWindow("Pac-Man", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ancho * dim, altura * dim*1.1, SDL_WINDOW_SHOWN); //tamaño a la altura para info adicional
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError()); return 0;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Error al crear el renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window); SDL_Quit(); return 0;
    }

    const Uint8* tecla = SDL_GetKeyboardState(NULL);

    //Crea surfaces a partur de las distintas imagenes, y despues los convierte en texture para usar en el resto del programa
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    SDL_Surface* fantasmaR = IMG_Load("sprites\\fantasmaR.png");
    SDL_Surface* fantasmaRo = IMG_Load("sprites\\fantasmaRo.png");
    SDL_Surface* fantasmaN = IMG_Load("sprites\\FantasmaN.png");
    SDL_Surface* fantasmaC = IMG_Load("sprites\\FantasmaC.png");
    SDL_Surface* pacman = IMG_Load("sprites\\pacman.png");
    SDL_Surface* pacman_0 = IMG_Load("sprites\\pacman_0.png");
    SDL_Surface* pacman_2 = IMG_Load("sprites\\pacman_2.png");

    SDL_Texture* fantasmaR_texture = SDL_CreateTextureFromSurface(renderer, fantasmaR);
    SDL_Texture* fantasmaRo_texture = SDL_CreateTextureFromSurface(renderer, fantasmaRo);
    SDL_Texture* fantasmaN_texture = SDL_CreateTextureFromSurface(renderer, fantasmaN);
    SDL_Texture* fantasmaC_texture = SDL_CreateTextureFromSurface(renderer, fantasmaC);
    SDL_Texture* pacman_texture = SDL_CreateTextureFromSurface(renderer, pacman);
    SDL_Texture* pacman_0_texture = SDL_CreateTextureFromSurface(renderer, pacman_0);
    SDL_Texture* pacman_2_texture = SDL_CreateTextureFromSurface(renderer, pacman_2);

    SDL_FreeSurface(fantasmaR);
    SDL_FreeSurface(fantasmaRo);
    SDL_FreeSurface(fantasmaN);
    SDL_FreeSurface(fantasmaC);
    SDL_FreeSurface(pacman);
    SDL_FreeSurface(pacman_0);
    SDL_FreeSurface(pacman_2);

    SDL_Rect fantasmaR_destination;
    SDL_Rect fantasmaRo_destination;
    SDL_Rect fantasmaN_destination;
    SDL_Rect fantasmaC_destination;
    SDL_Rect pacman_destination;


    //todos los sprites tienen tamaño cuadrado de dimensiones dimxdim, excepto los puntos que son 1/4dim de lado
    fantasmaR_destination.w = dim; fantasmaR_destination.h = dim;
    fantasmaRo_destination.w = dim; fantasmaRo_destination.h = dim;
    fantasmaN_destination.w = dim; fantasmaN_destination.h = dim;
    fantasmaC_destination.w = dim; fantasmaC_destination.h = dim;
    pacman_destination.w = dim;  pacman_destination.h = dim;
    SDL_Rect muros;
    muros.w = dim; muros.h = dim;
    SDL_Rect punto;
    punto.w = dim/4; punto.h = dim/4;

    //define la posicion del texto con info adicional, y el rectangulo que hace wipe para actualizar el texto
    SDL_Rect info;
    info.x = 0; info.w = ancho * dim;
    info.y = altura*dim + 1; info.h = altura * dim * 0.1;
    SDL_Rect itime;
    itime.x = ancho / 20 * dim;
    itime.y = altura * 1.01 * dim;
    SDL_Rect ipoints;
    ipoints.x = ancho / 20 * 14 * dim;
    ipoints.y = itime.y;
    SDL_Rect ttime;
    ttime.x = itime.x;
    ttime.y = altura * 1.04 * dim;
    SDL_Rect tpoints;
    tpoints.x = ipoints.x;
    tpoints.y = ttime.y;
    TTF_Font* font = TTF_OpenFont("fuente.ttf", dim);
    SDL_Color color = { 255, 255, 255, 255 };

    SDL_Event event;
    int jugando = 1;

    while (jugando==1)
    {
       //actualiza el renderer uptps veces, dando el delay suficiente como para que se demore tmov milisegundos en avanzar una casilla
        for (int w=0; w<uptps;w++){
            for (int i = 0; i < altura; i++) {
                for (int j = 0; j < ancho; j++) {
                    DibujarMapa(MAPA, i, j, renderer, &muros, &punto, dim);
                }
                switch (tiempo % 4) {
                case 0: //añade la animacion del pacman moviendo la boca
                    renderPacman(&pacman_destination, renderer, pacman_0_texture, dir, pos, mov, dim, 0, w, uptps); break;
                case 1:
                    renderPacman(&pacman_destination, renderer, pacman_texture, dir, pos, mov, dim, 0, w, uptps); break;
                case 2:
                    renderPacman(&pacman_destination, renderer, pacman_2_texture, dir, pos, mov, dim, 0, w, uptps); break;
                case 3:
                    renderPacman(&pacman_destination, renderer, pacman_texture, dir, pos, mov, dim, 0, w, uptps); break;
                }
                renderObject(&fantasmaN_destination, renderer, fantasmaN_texture, dir + 4, pos + 8, mov + 3, dim, 4, w, uptps);
                renderObject(&fantasmaC_destination, renderer, fantasmaC_texture, dir + 3, pos + 6, mov + 4, dim, 3, w, uptps);
                renderObject(&fantasmaRo_destination, renderer, fantasmaRo_texture, dir + 2, pos + 4, mov + 2, dim, 2, w, uptps);
                renderObject(&fantasmaR_destination, renderer, fantasmaR_texture, dir + 1, pos + 2, mov + 1, dim, 1, w, uptps);
                AddInfo(renderer, &info, &tpoints, &ttime, &itime, &ipoints, font, color);

            }
            for (int vz = 1; vz <= 4; vz++) {
                if (losecon(mov, dir, pos, vz, w, dim, uptps) == 1 && powerup==0) jugando = -1; //checkea si la posicion de los fantasmas es < de media casilla, y si no tienes powerup
            }
            if (points == pointstotal) jugando = 2; //checkea si conseguiste todos los puntos del nivel
            SDL_RenderPresent(renderer);
            if (jugando == -1) {
                printf("\n GAME OVER, MUCHAS GRACIAS POR JUGAR!\n");
                SDL_Delay(2000);
                break; }
            if (jugando == 2) {
                printf("\n FELICITACIONES, HAS GANADO!!\n MUCHAS GRACIAS POR JUGAR!\n");
                SDL_Delay(2000);
                break;
            }
            SDL_Delay(tmov/uptps);
        }
        for (int i = 0; i < 5; i++) {
            movact(mov[i], pos + 2 * i, ov[i]); //actualiza el arreglo de posiciones a donde ahora aparecen estar los sprites
        }
        pointcheck(MAPA, *pos, *(pos + 1));
        SDL_RenderPresent(renderer);
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                jugando = 0;
            }
        }
        if (jugando <= 0 || jugando==2) break;
        jugando = inputdec(tecla, MAPA, Inter, pos, tar, mov, dir, ov);
        tiempo++;
        if (powerup == 0) { //actualiza el comportamiento de los fantasmas en un ciclo de 12 segundos de scatter -> 48 segundos de persecusión, a no ser que tenga el powerup
            if (tiempo % 60 > 12) scatter = 0; 
            else scatter = 1; 
        }
        else { 
            powerup--;
            scatter = 1;
        }
    }
    SDL_DestroyTexture(fantasmaR_texture);
    SDL_DestroyTexture(fantasmaRo_texture);
    SDL_DestroyTexture(fantasmaN_texture);
    SDL_DestroyTexture(fantasmaC_texture);
    SDL_DestroyTexture(pacman_texture);
    SDL_DestroyTexture(pacman_0_texture);
    SDL_DestroyTexture(pacman_2_texture);
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}