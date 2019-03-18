#include <stdio.h>
#include <math.h>
#include <C_General.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

#define PI 3.14159265

int main(int argc, char **argv)
{
	long i, j;

	//strcat(destino, origen) // Agrega el contenido de origen al final de destino

	//Pedimos al usuario que seleccione la imagen a rotar
	C_Image imagenIN;
	/*
	char name[100];
	printf("Introduzca el nombre de la imagen que desea rotar: ");
	fgets(name, 100, stdin);

	if (C_FileExists(name)) {
		printf("\nExisto!!! \n");
	}

	printf("%s\n", name);
	*/
	imagenIN.ReadBMP("Hercules_Gris.bmp");

	//Pedimos al usuario que introduzca el angulo a rotar
	long angulo = -1;
	do {
		printf("Introduzca los grados (entre 0 y 360) que desea rotar la imagen: ");
		scanf("%ld", &angulo);
	} while (angulo < 0 || angulo > 360);

	//Calculamos el seno y coseno del angulo a rotar (en radianes)
	double st, ct;
	ct = cos((angulo * 2 * PI) / 360);
	st = sin((angulo * 2 * PI) / 360);
	
	/*
	//Calculamos el seno y coseno del angulo a rotar (en grados)
	double st, ct;
	ct = cos((angulo * 2 * PI) / 360);
	st = sin((angulo * 2 * PI) / 360);
	*/

	//Filas y columnas de la matriz original
	long N = imagenIN.RowN();
	long M = imagenIN.ColN();

	printf("Filas: %ld, Columnas: %ld\n", N, M);

	//Mitad de fila y columna de la matriz original
	double N2 = N / 2;
	double M2 = M / 2;

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
		Calculamos los indices de fila y columna para la nueva matriz:
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

	//Creamos la nueva imagen (matriz)
	C_Image imagenOUT = C_Image(p1, p2, q1, q2, 127.0);

	//Filas y columnas de la nueva matriz
	long Np = imagenOUT.RowN();
	long Mp = imagenOUT.ColN();

	printf("FilasRot: %ld, ColumnasRot: %ld\n", Np, Mp);

	//Mitad de fila y columna de la nueva matriz
	double Np2 = Np / 2;
	double Mp2 = Mp / 2;

	//Valores centrales en la matriz original (corrimientos acumulados)
	double sxIN = imagenIN.FirstRow() + N2;
	double syIN = imagenIN.FirstCol() + M2;

	//Valores centrales en la nueva matriz (corrimientos acumulados)
	double sxOUT = imagenOUT.FirstRow() + Np2;
	double syOUT = imagenOUT.FirstCol() + Mp2;
	
	
	//Pedimos al usuario que seleccione el algoritmo a aplicar
	printf("____________________ ALGORITMOS ____________________\n");
	printf("0 -> Algoritmo de rotacion directa.\n");
	printf("1 -> Algoritmo de rotacion inversa.\n");
	printf("2 -> Algoritmo de rotacion inversa bis.\n");

	int algoritmo = -1;
	do {
		printf("Seleccione el algoritmo que desea aplicar: ");
		scanf("%ld", &algoritmo);
	} while (algoritmo < 0 || algoritmo > 3);

	long ip, jp;
	switch (algoritmo) {
		case 0:
			//ALGORITMO DE IMPLEMENTACION DIRECTA DE LA ROTACION
			for (j = imagenIN.FirstCol(); j <= imagenIN.LastCol(); j++) {
				yp = j - syIN;
				for (i = imagenIN.FirstRow(); i <= imagenIN.LastRow(); i++) {
					xp = i - sxIN;

					xr = xp * ct + yp * st;
					yr = -xp * st + yp * ct;

					//Con lround redondeamos a la parte entera
					ip = lround(xr + sxOUT);
					jp = lround(yr + syOUT);

					imagenOUT(ip, jp) = imagenIN(i, j);
				}
			}
			break;

		case 1:
			//ALGORITMO DE IMPLEMENTACION INVERSA DE LA ROTACION
			for (j = imagenOUT.FirstCol(); j <= imagenOUT.LastCol(); j++) {
				yp = j - syOUT;
				for (i = imagenOUT.FirstRow(); i <= imagenOUT.LastRow(); i++) {
					xp = i - sxOUT;

					xr = ct * xp + st * yp;
					yr = -st * xp + ct * yp;

					//Con lround redondeamos a la parte entera
					ip = lround(xr + sxIN);
					jp = lround(yr + syIN);

					if ((ip >= imagenIN.FirstRow()) && (jp >= imagenIN.FirstCol()) && (ip <= imagenIN.LastRow()) && (jp <= imagenIN.LastCol())) {
						imagenOUT(i, j) = imagenIN(ip, jp);
					}
					else {
						imagenOUT(i, j) = 127.0;
					}

					//imagenOUT(i, j) = imagenIN(ip, jp);
				}
			}
			break;

		case 2:
			//ALGORITMO DE ROTACION INVERSA BIS (FUENTE BIBLIOGRAFICA)
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

					//Con lround redondeamos a la parte entera
					ip = lround(x2);		//Parte entera de x
					jp = lround(y2);		//Parte entera de y

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
	imagenOUT.WriteBMP("Hercules_GrisRot.bmp");

	/*	Hay que implementarlo con mas algoritmos como el de los k vecinos mas cercanos y el de busqueda binaria/cuadratica	*/	

	return 0;
}