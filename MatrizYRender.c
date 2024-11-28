#include "MatrizYRender.h"
int** crearLaberinto(int* ancho, int* altura, char* fp, int* pos) {
	FILE* fp1 = fopen(fp, "r");
    if (fp1 == NULL) {
        printf("No se pudo abrir el archivo correctamente\n");
        exit(0);
    }
    casillas = 1;
    while (!feof(fp1)) { //lee caracteres hasta encontrar un salto de linea, segun cuantos caracteres leyo en la primera linea determina el ancho, y el alto es n° de \n
        c = getc(fp1);
        if (c == '\n') {
            (*altura)++;
            if (*ancho == 0) {
                (*ancho) = casillas;
            }
        }
        else {
            casillas++;
        }
    }
    if (*altura<=6 || *ancho<=6 || casillas<70) { //para evitar problemas con matrices demasiado pequeñas
        printf("La matriz no cumple con las condiciones para que sea un juego justo\n");
        printf("(Archivo debe contener al menos 7 columnas, 7 filas, y 70 casillas");
        exit(0);
    }
    int** LABERINTO = malloc(*altura * sizeof(int*));
    for (int i = 0; i < *altura; i++) { LABERINTO[i] = (int*)malloc(*ancho * sizeof(int)); }
    FILE* Laberinto = fopen(fp, "r");
    i = 0; j = 0;
    while ((aux = fgetc(Laberinto)) != EOF)
    {   //actualiza posiciones de los elementos y el puntajetotal que pacman tiene que conseguir para ganar mientras lee la matriz
        switch (aux) {
        case '#': val = -1; break;
        case '0': val = 0; break;
        case '*': val = 1; pointstotal++; break;
        case 'P': val = 2; *(pos + 0) = j; *(pos + 1) = i; break;
        case 'b': val = 3; *(pos + 2) = j; *(pos + 3) = i; break;
        case 'c': val = 4; *(pos + 4) = j; *(pos + 5) = i; break;
        case 'i': val = 5; *(pos + 6) = j; *(pos + 7) = i; break;
        case 'p': val = 6; *(pos + 8) = j; *(pos + 9) = i; break;
        case '+': val = 7; pointstotal += 5; break;
        }
        LABERINTO[i][j] = val;
        j++;
        if (j == *ancho) { j = 0; i++; }
    }
    fclose(fp1);
    fclose(Laberinto);

    return LABERINTO;
}

//Los fantasmas solo actualizaran su casilla objetivo cuando se encuentren en una casilla que sea de interseccion
int** crearIntersecciones(int** Laberinto,int altura, int ancho){
    int** Inter = malloc(altura * sizeof(int*));
    for (int i = 0; i < altura; i++) { Inter[i] = (int*)malloc(ancho * sizeof(int)); }
    int cont=0;
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < ancho; j++) {
            cont = 0;
            if (i == 0 || i == altura - 1 || j == 0 || j == ancho - 1) {
                Inter[i][j] = 0;
                continue;
            } //una casilla interseccion es aquella que no es muro, y que tiene 3 casillas no muros adyacentes
              //todas las orillas no se toman como intersecciones para evitar problemas con la logica de los fantasmas
            if (Laberinto[i+1][j] > -1)cont++;
            if (Laberinto[i-1][j] > -1)cont++;
            if (Laberinto[i][j+1] > -1)cont++;
            if (Laberinto[i][j-1] > -1)cont++;
            if (cont >= 3 && Laberinto[i][j]>=0) Inter[i][j] = 1; else Inter[i][j] = 0;
        }
    }
    return Inter;
}

//renderiza toda la informacion que va como texto en el bottom de la pantalla
void AddInfo(SDL_Renderer* rend, SDL_Rect* inf, SDL_Rect* poi, SDL_Rect* tim, SDL_Rect* it, SDL_Rect* ip, TTF_Font* font, SDL_Color color) {
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderFillRect(rend, inf);
    char cadena[50] = "Tiempo trans.:";
    DrawText(it, font, color, rend, cadena);
    *cadena = "";
    sprintf_s(cadena, 50, "Puntaje obt.:");
    DrawText(ip, font, color, rend, cadena);
    *cadena = "";
    sprintf_s(cadena, 50, "%d", tiempo); // Convierte el número en cadena
    DrawText(tim, font, color, rend, cadena);
    *cadena = "";
    sprintf_s(cadena, 50, "%d/%d", points*10, pointstotal*10); 
    DrawText(poi, font, color, rend, cadena);

}

void DrawText(SDL_Rect* obj, TTF_Font* font, SDL_Color color, SDL_Renderer* rend, char* texto) {
    SDL_Surface* font_s = TTF_RenderText_Solid(font, texto, color);
    SDL_Texture* font_t = SDL_CreateTextureFromSurface(rend, font_s);
    (*obj).w = font_s->w;
    (*obj).h = font_s->h;
    SDL_RenderCopy(rend, font_t, NULL, obj);
    SDL_FreeSurface(font_s);
    SDL_DestroyTexture(font_t); //IMPORTANTE: Si no se definen y se liberan las surface/texture, hay memory leak, causando altos drops de frames
    //Esto tampoco es perfecto, pues ralentiza ligeramente la velocidad real del juego, pero aumentando la velocidad artificialmente causa que no aparente problemas

}

//Asigna un color de fondo segun si es muro u otro objeto y dibuja el rectangulo. Si es una casilla con punto, dibuja el punto tambien con el color correspondiente
void DibujarMapa(int** M, int i, int j, SDL_Renderer* renderer, SDL_Rect* mur, SDL_Rect* punt, int dim) {
    (*mur).x = j * dim; (*mur).y = i * dim;
    switch (M[i][j]) {
    case -1:SDL_SetRenderDrawColor(renderer, 0, 0, 80, 255);
        if (powerup > 6) SDL_SetRenderDrawColor(renderer, 0, 100, 20, 255); //cambia el color de los muros para dejar claro cuando está el powerup activo
        else if (powerup > 0) SDL_SetRenderDrawColor(renderer, 0, 50, 50, 255); //y cuando esta por acabarse
        SDL_RenderFillRect(renderer, mur); break;
    case 0: SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, mur); break;
    case 1:
        (*punt).x = j * dim + (dim - (*punt).w) / 2; (*punt).y = i * dim + (dim - (*punt).h) / 2;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, mur);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, punt); break;
    case 7:
        (*punt).x = j * dim + (dim - (*punt).w) / 2; (*punt).y = i * dim + (dim - (*punt).h) / 2;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, mur);
        SDL_SetRenderDrawColor(renderer, 200, 150, 0, 255);
        SDL_RenderFillRect(renderer, punt); break;
    default:
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, mur); break;
    }
}

//actualiza la matriz cuando se obtiene un punto/powerup, actualiza la cantidad de puntos recogidos, y otorga el powerup si se tomo
void pointcheck(int** M, int x, int y) { 
    if (M[y][x] == 1) {
        points++;
        M[y][x] = 0;
    }
    else if (M[y][x] == 7) {
        points += 5;
        if (powerup > 0) powerup += 10;
        else powerup = 30;
        M[y][x] = 0;
    }
}


//renderiza el objeto en la posicion que se encuentra, mas un modificado a cada eje segun en que "fraccion" de la casilla se deberia encontrar
void renderObject(SDL_Rect* objdest, SDL_Renderer* renderer, SDL_Texture* textur, int* dir, int* pos, int* mov, int dim, int obj, int t, int upt) {
    float mx = 0, my = 0;
    calculo(&mx, &my, *mov, *dir, dim, upt, t);
    (*objdest).x = *(pos)*dim + mx;
    (*objdest).y = *(pos + 1) * dim + my;
    SDL_RenderCopy(renderer, textur, NULL, objdest);
    return;
}
//lo mismo que render object, pero con angulos incluidos
void renderPacman(SDL_Rect* objdest, SDL_Renderer* renderer, SDL_Texture* textur, int* dir, int* pos, int* mov, int dim, int obj, int t, int upt) {
    float mx = 0, my = 0;
    int angulo = 0;
    calculo(&mx, &my, *mov, *dir, dim, upt, t);
    switch (*dir) {
    case 1: angulo = 270; break;
    case 2: angulo = 0; break;
    case 3: angulo = 90; break;
    case 4: angulo = 180;
    }
    (*objdest).x = *(pos)*dim + mx;
    (*objdest).y = *(pos + 1) * dim + my;
    SDL_RenderCopyEx(renderer, textur, NULL, objdest, angulo, NULL, SDL_FLIP_NONE);
    return;
}