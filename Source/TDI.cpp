#include <stdio.h>
#include <iostream>
#include <math.h>
#include <C_General.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

#define PI 3.14159265

int main(int argc, char **argv)
{
	//Indices de recorrido de bucles
	long i, j;

	/*
	//Se pide al usuario que introduzca el nombre de la imagen a rotar
	string name1;
	char name[50];
	do {
		printf("Introduzca el nombre de la imagen BMP que desea rotar (sin la extension): ");
		getline(cin, name1);
		strcpy(name, name1.c_str());
		strcat(name, ".bmp");
	} while (!C_FileExists(name));

	*/

	//Creamos la imagen original
	C_Image imagenIN;
	imagenIN.ReadBMP("Sierra_Nevada.bmp");
	imagenIN.Reindex(0, 0);

	//Se pide al usuario que introduzca el angulo a rotar
	long angulo = -1;
	do {
		printf("Introduzca los grados (entre 0 y 360) que desea rotar la imagen: ");
		scanf_s("%ld", &angulo);
	} while (angulo < 0 || angulo > 360);

	//Se calcula el seno y coseno del angulo a rotar (en radianes)
	double st, ct;
	ct = cos((angulo * PI) / 180);
	st = sin((angulo * PI) / 180);
	
	/*
	//Se calcula el seno y coseno del angulo a rotar (en grados)
	double st, ct;
	ct = cos(angulo);
	st = sin(angulo);
	*/

	//Filas y columnas de la matriz original
	long N = imagenIN.RowN();
	long M = imagenIN.ColN();
	
	//Mitad de fila y columna de la matriz original
	double N2 = N / 2;
	double M2 = M / 2;

	//Valores centrales en la matriz original (corrimientos acumulados)
	double sxIN = imagenIN.FirstRow() + N2;
	double syIN = imagenIN.FirstCol() + M2;

	printf("N2: %lf, M2: %lf\n", N2, M2);

	//Esquinas de la matriz original
	long x[] = { imagenIN.FirstRow(), imagenIN.LastRow(), imagenIN.FirstRow(), imagenIN.LastRow() };
	long y[] = { imagenIN.FirstCol(), imagenIN.FirstCol(), imagenIN.LastCol(), imagenIN.LastCol() };

	//Esquinas de la matriz transformada
	long p [4];
	long q [4];

	long p1 = LONG_MAX;
	long q1 = LONG_MAX;
	long p2 = LONG_MIN;
	long q2 = LONG_MIN;

	/*
		Se calculan los indices de fila y columna para la nueva matriz:
		p1 -> primera fila
		p2 -> ultima fila
		q1 -> primera columna
		q2 -> ultima columna
	*/
	double xp, xr, yp, yr;
	for (i = 0; i < 4; i++) {
		xp = x[i] - N2;
		yp = y[i] - M2;

		xr = xp * ct + yp * st;
		yr = -xp * st + yp * ct;

		p[i] = lround(xr + N2);
		q[i] = lround(yr + M2);

		if (p[i] < p1) p1 = p[i];
		if (p[i] > p2) p2 = p[i];
		if (q[i] < q1) q1 = q[i];
		if (q[i] > q2) q2 = q[i];
	}

	printf("p1: %ld, p2: %ld, q1: %ld, q2: %ld\n", p1, p2, q1, q2);
	//Se crea la nueva imagen (matriz)
	C_Image imagenOUT = C_Image(p1, p2, q1, q2, 127.0);
	imagenOUT.Reindex(0, 0);

	printf("p1: %ld, p2: %ld, q1: %ld, q2: %ld\n", imagenOUT.FirstRow(), imagenOUT.LastRow(), imagenOUT.FirstCol(), imagenOUT.LastCol());

	//Filas y columnas de la nueva matriz
	long Np = imagenOUT.RowN();
	long Mp = imagenOUT.ColN();

	//Mitad de fila y columna de la nueva matriz
	double Np2 = Np / 2;
	double Mp2 = Mp / 2;

	//Valores centrales en la nueva matriz (corrimientos acumulados)
	double sxOUT = imagenOUT.FirstRow() + Np2;
	double syOUT = imagenOUT.FirstCol() + Mp2;
	
	//Se pide al usuario que seleccione el algoritmo a aplicar
	printf("____________________ ALGORITMOS ____________________\n");
	printf("0 -> Algoritmo de rotacion directa (k vecinos mas cercanos).\n");
	printf("1 -> Algoritmo de rotacion inversa (k vecinos mas cercanos).\n");
	printf("2 -> Algoritmo de rotacion inversa (interpolacion lineal).\n");

	int algoritmo = -1;
	do {
		printf("Seleccione el algoritmo que desea aplicar: ");
		scanf_s("%ld", &algoritmo);
	} while (algoritmo < 0 || algoritmo > 3);

	long ip, jp;
	switch (algoritmo) {
		case 0:
			//ALGORITMO DE IMPLEMENTACION DIRECTA DE LA ROTACION (k vecinos mas cercanos)
			for (j = imagenIN.FirstCol(); j <= imagenIN.LastCol(); j++) {
				yp = j - syIN;
				for (i = imagenIN.FirstRow(); i <= imagenIN.LastRow(); i++) {
					xp = i - sxIN;

					xr = xp * ct - yp * st;
					yr = xp * st + yp * ct;

					//Con lround redondeamos a la parte entera
					ip = lround(xr + sxOUT);
					jp = lround(yr + syOUT);

					//printf("ip: %ld, jp: %ld\n", ip, jp);
					imagenOUT(ip, jp) = imagenIN(i, j);
				}
			}
			break;

		case 1:
			//ALGORITMO DE IMPLEMENTACION INVERSA DE LA ROTACION (k vecinos mas cercanos)
			for (j = imagenOUT.FirstCol(); j <= imagenOUT.LastCol(); j++) {
				yp = j - syOUT;
				for (i = imagenOUT.FirstRow(); i <= imagenOUT.LastRow(); i++) {
					xp = i - sxOUT;

					xr = xp * ct + yp * st;
					yr = -xp * st + yp * ct;

					//Con lround se redondea a la parte entera
					ip = lround(xr + sxIN);
					jp = lround(yr + syIN);

					//Se mapearan en la nueva imagen aquellos pixeles que existan en la imagen original
					if ((ip >= imagenIN.FirstRow()) && (jp >= imagenIN.FirstCol()) && (ip <= imagenIN.LastRow()) && (jp <= imagenIN.LastCol())) {
						imagenOUT(i, j) = imagenIN(ip, jp);
					}
					else {
						imagenOUT(i, j) = 127.0;
					}
				}
			}
			break;

		case 2:
			//ALGORITMO DE ROTACION INVERSA CON INTERPOLACION (FUENTE BIBLIOGRAFICA)
			double ys, yc, x2, y2, c1, d1, a1, b1, a2, b2, f1, f2, f3, f4;
			long i1, j1;
			for (j = imagenOUT.FirstCol(); j <= imagenOUT.LastCol(); j++) {
				yp = j - syOUT;
				ys = -st * yp;
				yc = ct * yp;
				for (i = imagenOUT.FirstRow(); i <= imagenOUT.LastRow(); i++) {
					xp = i - sxOUT;
					xr = xp * ct + ys;
					yr = xp * st + yc;

					x2 = xr + sxIN;
					y2 = yr + syIN;

					//En este caso interesa la parte entera unicamente
					ip = trunc(x2);			//Parte entera de x
					jp = trunc(y2);			//Parte entera de y

					i1 = ip + 1;			//Indice derecho a i
					j1 = jp + 1;			//Indice inferior a j

					c1 = x2 - ip;			//Error entre entero y decimal
					d1 = y2 - jp;			//Error entre entero y decimal

					a1 = 1 - c1;			//a1 = 1 - c1 = c2
					b1 = c1;				//b1 = 1 - c2 = c1

					a2 = 1 - d1;			//a2 = 1 - d1 = d2
					b2 = d1;				//b2 = 1 - d2 = d1

					f1 = a1 * a2;		f2 = b1 * a2;		f3 = a1 * b2;		f4 = a2 * b2;

					imagenOUT(i, j) = f1 * imagenIN(ip, jp) + f2 * imagenIN(i1, jp) + f3 * imagenIN(ip, j1) + f4 * imagenIN(i1, j1);
				}
			}
			break;

		default: 
			printf("No ha seleccionado ningun algoritmo valido");
			break;
	}
	
	imagenOUT.palette = imagenIN.palette;
	imagenOUT.WriteBMP("ImagenRotada.bmp");

	/*	Hay que implementarlo con mas algoritmos como el bicubico/bilineal ademas de aplicar la interpolacion	*/	

	return 0;
}