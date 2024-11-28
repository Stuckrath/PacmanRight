#include "posandmov.h"
//En general, para movimientos se usa de 0 a 4, significando cada uno respectivamente nulo-arriba-derecha-abajo-izquierda
//Objeto 0 es pacman, Objeto 1 es Inky (Rojo), 2 es Pinky (Rosa), 3 es Blinky (Cyan), 4 es Clyde (Naranja)
//Verifica si un mov. es posible (no tiene muro), devuelve 1 si se puede, y 2 si es que da vuelta a la pantalla (screenwrap)
int checkmov(int dir, int* P, int** M) {
    int x = *P, y = *(P + 1);
    switch (dir) {  
    case 1: 
        if (y <= 0) {
            return screenwrap(M,x,altura-1);
        }
        if (M[y-1][x]<0) {
            return 0;
        }
        return 1; break;
    case 2: //right
        if (x+1 >= ancho) return screenwrap(M, 0, y);
        if (M[y][x+1] < 0) return 0;
        return 1; break;
    case 3: //down
        if (y + 1 >= altura) return screenwrap(M, x, 0);
        if (M[y+1][x] < 0) return 0;
        return 1; break;
    case 4: //left
        if (x <= 0) return screenwrap(M, ancho-1, y);
        if (M[y][x-1] < 0) return 0;
        return 1;
        break;
    case 0:
        return 1;
    }
    return 0;
}

int screenwrap(int** Lab, int x, int y) {
    if (Lab[y][x] < 0) return 0;
    else return 2;
}

//Actualiza posicion del objeto, teniendo en cuenta screenwrap
void movact(int mov, int* P, int ov) {
    if (mov == 0) return;
    if (ov == 1) {
        switch (mov) {
        case 1: *(P + 1) = altura-1; break;
        case 2: *(P) = 0; break;
        case 3: *(P + 1) = 0; break;
        case 4: *(P) = ancho-1; break;
        }
    }
    else {
        switch (mov) {
        case 1: *(P + 1) -= 1; break;
        case 2: *(P) += 1; break;
        case 3: *(P + 1) += 1; break;
        case 4: *(P) -= 1; break;
        }
    }
    return;
}

//Busca el movimiento mas eficiente para llegar al Target en una interseccion (que no sea volver sobre sus pasos)
int interseccion(int dir, int* P, int** M, int* T) {
    int dist = altura * ancho;
    int ef = 0;
    ef = intercheck(dir, 1, P, M, T, 0, -1, ef, &dist);
    ef = intercheck(dir, 2, P, M, T, 1, 0, ef, &dist);
    ef = intercheck(dir, 3, P, M, T, 0, 1, ef, &dist);
    ef = intercheck(dir, 4, P, M, T, -1, 0, ef, &dist);
    return ef;
}
//Distancia manhattan
int distancia(int* P, int* T, int mx, int my) {
    return fabs((*P + mx) - *T) + fabs((*(P + 1) + my) - *(T + 1));
}
int intercheck(int dirp, int dira, int* P, int** M, int* T, int mx, int my, int ef, int* dist) {
    if (checkmov(dira, P, M) == 1){
        if(dira != noback(dirp)){
            if (distancia(P, T, mx, my) < *dist) {
                *dist = distancia(P, T, mx, my); ef = dira;
            }
        }
    }
    return ef;
}

//P, T, dir deben estar apuntando directamente a la casilla del objeto(fantasma) correspondiente
int movfantasma(int obj, int* dir, int* P, int** M, int* T, int** I, int* movi) {
    int mov = 0, ov = 0, aux = 0;
    int ic = *P, jc = *(P + 1);
    if (I[jc][ic] == 1) { //solo cambia de objetivo si se encuentra en una interseccion
        target(obj, P - 2 * obj, T - 2 * obj, dir-obj);
        mov = interseccion(*dir, P, M, T);
    } else {
        for (int z = 1; z <=4; z++) { //checkea cada movimiento desde arriba en sentido horario hasta encontrar uno valido, y que no devuelva por sus pasos
            aux = checkmov(z, P, M);
            if (aux > 0 && z != noback(*dir)){ 
                mov = z;
                if (aux == 2) ov = 1;
                break;
            }
        }
    }
    if (checkmov(mov, P, M) > 0) {
        *dir = mov;
    }
    else {
        mov = 0;
    }
    *movi = mov;
    return ov;
} 

int noback(int dir) { //cumple la misma funcion que ((dir+1)%4), pero devuelve 0 si dir es 0
    switch (dir) {
        case 1: return 3; break;
        case 2: return 4; break;
        case 3: return 1; break;
        case 4: return 2; break;
    }
    return 0;
}

void tscatter(int obj, int* T) { //Cuando estan en modo scatter, cada fantasma se va a una esquina
    //como no pueden devolverse por sus pasos, van a dar vueltas un poco mas largas mientras esperan que sea hora de ir por pacman
    int auxx = 0; int auxy = 0;
    switch (obj) {
    case 1: auxx = ancho - 1; auxy = 0; break;
    case 2: auxx = 0; auxy = 0; break;
    case 3: auxx = ancho - 1; auxy = altura - 1; break;
    case 4: auxx = 0; auxy = altura - 1; break;
    }
    *(T + 2 * obj) = auxx; *(T + 2 * obj + 1) = auxy;
    return;
}
//llama a la funcion correspondiente segun si esta en modo scatter o no
void target(int obj, int* P, int* T, int* dir) {
    if (scatter == 1) {
        tscatter(obj, T);
    }
    else {
        switch (obj) {
        case 1: tblinky(P,T); break;
        case 2: tpinky(P, T, dir); break;
        case 3: tinky(P, T, dir); break;
        case 4: tclyde(P, T); break;
        }
    }
    return;
}
//Blinky: Va directamente a la pos. de Pacman
void tblinky(int* P, int* T) {
    *(T + 2) = *P;
    *(T + 3) = *(P + 1);
    return;
}
//Pinky: Va a la casilla 4 casillas en frente de Pacman
void tpinky(int* P, int* T, int* dir) {
    int mx = 0, my = 0, auxy, auxx;
    switch (*dir) {
    case 1: my = -4; break;
    case 2: mx = 4; break;
    case 3: my = 4; break;
    case 4: mx = -4; break;
    }
    auxx = max(0, *(P)+mx); auxx = min(ancho - 1, auxx);
    auxy = max(0, *(P + 1) + my); auxy = min(altura - 1, auxy);
    *(T + 4) = auxx;
    *(T + 5) = auxy;
    return;
}
//Inky: Va a la casilla apuntada por el vector que va desde Blinky a la casilla 2 casillas frente a Pacman; multiplicado por 2 (medio complejo)
void tinky(int* P, int* T, int* dir) {
    int mx = 0, my =0, xf = 0, yf = 0, xs = 0, ys = 0, vx = 0, vy = 0, auxx, auxy;
    switch (*dir) {
    case 1: my = -2; break;
    case 2: mx = 2; break;
    case 3: my = 2; break;
    case 4: mx = -2; break;
    }
    xs = *(P + 2); ys = *(P + 3);
    xf = *(P)+mx; yf = *(P + 1) + my;
    vx = xf - xs; vy = yf - ys;
    vx = xs + 2 * vx; vy = ys + 2 * vy;
    auxx = max(0, vx); auxx = min(ancho - 1, auxx);
    auxy = max(0, vy); auxy = min(altura - 1, auxy);
    *(T + 6) = auxx;
    *(T + 7) = auxy;
}
//Clyde: Va hacia Pacman directamente, a no ser que se encuentre a 8 casillas Manhattan de Pacman, en donde vuelve a "esparcirse"
void tclyde(int* P, int* T) {
    int d = distancia(P + 8, P, 0, 0);
    if (d > 8) {
        *(T + 8) = *P;
        *(T + 9) = *(P + 1);
    }
    else {
        tscatter(4, T);
    }
    return;
}
