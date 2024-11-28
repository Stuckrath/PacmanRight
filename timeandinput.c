#include "timeandinput.h"
#include "posandmov.h"

//Funcion principal para detectar inputs en el juego y 
int inputdec(Uint8* tec, int** L, int** I, int* p,int* t, int* m, int* d, int*o) {
	int maux = 0;
	while (1) {
		if (tec[SDL_SCANCODE_UP] || tec[SDL_SCANCODE_W]) { maux = 1; break; }
		if (tec[SDL_SCANCODE_RIGHT] || tec[SDL_SCANCODE_D]) { maux = 2; break; }
		if (tec[SDL_SCANCODE_DOWN] || tec[SDL_SCANCODE_S]) { maux = 3; break; }
		if (tec[SDL_SCANCODE_LEFT] || tec[SDL_SCANCODE_A]) { maux = 4; break; }
		if (tec[SDL_SCANCODE_ESCAPE]) { printf("Saliendo del juego");  return 0; }
		maux = *m;
		break;
	}
	switch (checkmov(maux, p, L)) { //en caso de que el movimiento ingresado no sea valido, revisa si el ultimo movimiento valido es tambien valido ahora (util para inputs "fluidos)
	case 1: 
		if (maux!=0) d[0] = maux; 
		*o = 0; break;
	case 2:
		if (maux != 0) d[0] = maux;
		*o = 1; break;
	case 0: 
		switch (checkmov(*m, p, L)) {
		case 1:
			maux=*m, d[0] = maux;
			*o = 0; break;
		case 2:
			maux = *m; d[0] = maux;
			*o = 1; break;
		case 0: maux = 0; *o = 0; break;
		}
	}
	m[0] = maux;
	for (int i = 1; i <= 4; i++) {;
		*(o+i) = movfantasma(i, d + i, p + 2 * i, L, t + 2 * i, I, m + i); //actualiza las matrices de movimiento, direccion y overwrap de cada fantasma. Despues en main se actualiza posicion
	}
	return 1;

}

//verifica si es que el fantasma se encuentra a media casilla o menos de distancia de pacman
int losecon(int* mov, int* dir, int* pos, int obj, int t, int dim, int upt) {
	float pxpac, pypac, pxobj, pyobj;
	float mxc = 0, myc = 0;
	calculo(&mxc, &myc, *mov, *dir, dim, upt, t); 
	pxpac = *(pos) * dim + mxc;
	pypac = *(pos+1) * dim + myc;
	calculo(&mxc, &myc, *(mov+obj), *(dir+obj), dim, upt, t);
	pxobj = *(pos + 2*obj) * dim + mxc;
	pyobj = *(pos + 2 * obj + 1) * dim + myc;
	float distdim = 0;
	distdim += fabs(pxobj - pxpac);
	distdim += fabs(pyobj - pypac);
	if (distdim <= dim / 2) return 1;
	return 0;
}
//Calcula cuanta posicion adicional agregarle al objeto, para que el movimiento sea fluido y no se "teletransporte" de una casilla a otra
void calculo(float* mxc, float* myc, int mov, int dir, int dim, int upt, int t) {
	float auxxc = 0, auxyc=0, auxt = t;
	if (mov != 0 && t % upt > 0) {
		switch (dir) {
		case 1: auxyc = -1; break;
		case 2: auxxc = 1; break;
		case 3: auxyc = 1; break;
		case 4: auxxc = -1; break;
		}
	}
	*mxc = auxxc * (dim / upt) * auxt;
	*myc = auxyc * (dim / upt) * auxt;
	return;

}