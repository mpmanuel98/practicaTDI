#include <stdio.h>
#include <math.h>
#include <C_General.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

#define PI 3.14159265

int main(int argc, char **argv)
{
	long i, j;
	C_Image imagenIN;
	imagenIN.ReadBMP("Hercules_Gris.bmp");

	long angulo = 90;

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
	long N2 = N / 2;
	long M2 = M / 2;

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
	long xp, xr, yp, yr;
	for (i = 0; i < 4; i++) {
		xp = x[i] - N2;
		yp = y[i] - M2;
		xr = xp * ct + yp * st;
		yr = -xp * st + yp * ct;

		p[i] = xr + N2;
		q[i] = yr + M2;

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

	printf("Filas: %ld, Columnas: %ld\n", Np, Mp);

	//Mitad de fila y columna de la nueva matriz
	double Np2 = Np / 2;
	double Mp2 = Mp / 2;

	/*
	//ALGORITMO DE INTERPOLACION LINEAL INVERSA PARA LA ROTACION
	long c1, d1, a1, b1, a2, b2, ip, jp, i1, j1;
	double x2, y2, ys, yc, f1, f2, f3, f4;

	for (j = imagenOUT.FirstCol(); j <= imagenOUT.LastCol(); j++) {
		yp = j - Mp2;
		ys = -st * yp;
		yc = ct * yp;
		for (i = imagenOUT.FirstRow(); i <= imagenOUT.LastRow(); i++) {
			xp = i - Np2;
			xr = xp * ct + ys;
			yr = xp * st + yc;

			x2 = xr + Np2;
			y2 = yr + Mp2;

			ip = x2;		//Parte entera de x
			jp = y2;		//Parte entera de y

			i1 = ip + 1;	//Indice derecho a i
			j1 = jp + 1;	//Indice inferior a j

			c1 = x2 - ip;	//Error entre entero y decimal
			d1 = y2 - jp;	//Error entre entero y decimal

			a1 = 1 - c1;	//a1 = 1 - c1 = c2
			b1 = c1;		//b1 = 1 - c2 = c1

			a2 = 1 - d1;	//a2 = 1 - d1 = d2
			b2 = d1;		//b2 = 1 - d2 = d1

			f1 = a1 * a2;		f2 = b1 * a2;		f3 = a1 * b2;		f4 = a2 * b2;

			imagenOUT(i, j) = f1 * imagenIN(ip, jp) + f2 * imagenIN(i1, jp) + f3 * imagenIN(ip, j1) + f4 * imagenIN(i1, j1);
			//imagenOUT(i, j) = imagenIN(ip, jp);
		}
	}
	*/
	
	/*
	//ALGORITMO DE IMPLEMENTACION INVERSA DE LA ROTACION
	//Valores centrales en la matriz original (corrimientos acumulados)
	long sxIN = imagenIN.FirstRow() + N2;
	long syIN = imagenIN.FirstCol() + M2;
	//Valores centrales en la nueva matriz (corrimientos acumulados)
	long sxOUT = imagenOUT.FirstRow() + Np2;
	long syOUT = imagenOUT.FirstCol() + Mp2;

	long ip, jp;
	for (j = imagenOUT.FirstCol(); j <= imagenOUT.LastCol(); j++) {
		printf("j: %ld, yp: %ld\n", j, yp);
		yp = j - syOUT;
		for (i = imagenOUT.FirstRow(); i <= imagenOUT.LastRow(); i++) {
			xp = i - sxOUT;
			
			xr = ct * xp + st * yp;
			yr = -st * xp + ct * yp;

			ip = xr + sxIN;
			jp = yr + syIN;
			
			if ((ip >= imagenIN.FirstRow()) && (jp >= imagenIN.FirstCol()) && (ip <= imagenIN.LastRow()) && (jp <= imagenIN.LastCol())) {
				imagenOUT(i, j) = imagenIN(ip, jp);
			}
			else {
				imagenOUT(i, j) = 127.0;
			}
			
			//imagenOUT(i, j) = imagenIN(ip, jp);
		}
	}
	*/
	
	//ALGORITMO DE IMPLEMENTACION DIRECTA DE LA ROTACION
	//Valores centrales en la matriz original (corrimientos acumulados)
	long sxIN = imagenIN.FirstRow() + N2;
	long syIN = imagenIN.FirstCol() + M2;
	//Valores centrales en la nueva matriz (corrimientos acumulados)
	long sxOUT = imagenOUT.FirstRow() + Np2;
	long syOUT = imagenOUT.FirstCol() + Mp2;

	long ip, jp;
	for (j = imagenIN.FirstCol(); j <= imagenIN.LastCol(); j++) {
		yp = j - syIN;
		for (i = imagenIN.FirstRow(); i <= imagenIN.LastRow(); i++) {
			xp = i - sxIN;

			xr = xp * ct + yp * st;
			yr = -xp * st + yp * ct;

			ip = xr + sxOUT;
			jp = yr + syOUT;

			imagenOUT(ip, jp) = imagenIN(i, j);
		}
	}
	
	 
	imagenOUT.palette = imagenIN.palette;
	imagenOUT.WriteBMP("Hercules_GrisMOD1.bmp");

	/*	Hay que implementarlo con mas algoritmos como el de los k vecinos mas cercanos y el de busqueda binaria/cuadratica	*/	

	return 0;
}