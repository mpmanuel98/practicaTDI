#include <stdio.h>
#include <math.h>
#include <C_General.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

#define PI 3.14159265

int main(int argc, char **argv)
{
	long i, j, angulo;
	C_Image imagenIN;
	imagenIN.ReadBMP("Hercules_Gris.bmp");

	double st, ct;
	angulo = 0;

	//Calculamos el seno y coseno del angulo (en radianes)
	ct = cos((angulo * 2 * PI) / 360);
	st = sin((angulo * 2 * PI) / 360);

	long N = imagenIN.RowN();
	long M = imagenIN.ColN();

	double N2 = N / 2;
	double M2 = M / 2;
	//printf("n2 = %ld \t m2 = %ld \n", N2, M2);

	//long x[] = { 0, imagenIN.LastRow() - 1, 0, imagenIN.LastRow() - 1 };
	//long y[] = { 0, 0, imagenIN.LastCol() - 1, imagenIN.LastCol() - 1 };
	long x[] = { imagenIN.FirstRow(), imagenIN.LastRow(), imagenIN.FirstRow(), imagenIN.LastRow() };
	long y[] = { imagenIN.FirstCol(), imagenIN.FirstCol(), imagenIN.LastCol(), imagenIN.LastCol() };

	long p [4];
	long q [4];

	double xp, xr, yp, yr;

	for (i = 0; i < 4; i++) {
		//printf("[_________________________________] x[%d] = %ld \t y[%d] = %ld \n", i, x[i], i, y[i]);
		xp = x[i] - N2;
		yp = y[i] - M2;
		xr = xp * ct + yp * st;
		yr = -xp * st + yp * ct;

		p[i] = xr + N2;
		q[i] = yr + M2;

		//printf("p[%d] = %ld \t q[%d] = %ld \n", i, p[i], i, q[i]);
	}

	long p1 = LONG_MAX;
	long q1 = LONG_MAX;
	long p2 = LONG_MIN;
	long q2 = LONG_MIN;

	for (i = 0; i < 4; i++) {
		if (p[i] < p1) p1 = p[i];
		if (p[i] > p2) p2 = p[i];
		if (q[i] < q1) q1 = q[i];
		if (q[i] > q2) q2 = q[i];
	}
	//printf("p1: %ld p2: %ld q1: %ld q2: %ld\n", p1, p2, q1, q2);

	//Filas y columnas de la nueva imagen
	long Np, Mp;
	Np = p2 - p1 + 1;
	Mp = q2 - q1 + 1;
	//printf("Filas: %ld | Columnas: %ld \n", Np, Mp);

	C_Image imagenOUT = C_Image(p1, p2, q1, q2, 127.0);

	//La mitad de la filas y columnas de la nueva imagen
	double Np2 = imagenOUT.RowN() / 2;
	double Mp2 = imagenOUT.ColN() / 2;

	double sx, sy;
	sx = p1 + Np2;
	sy = q1 + Mp2;
	
	double ys, yc, xx, yy, c1, d1, a1, b1, a2, b2, f1, f2, f3, f4;
	long ip, jp, i1, j1;

	for (j = imagenIN.FirstCol(); j <= imagenIN.LastCol(); j++) {
		yp = j - Mp2;
		ys = -st * yp;
		yc = ct * yp;
		for (i = imagenIN.FirstRow(); i <= imagenIN.LastRow(); i++) {
			xp = i - Np2;
			xr = xp * ct + ys;
			yr = xp * st + yc;

			xx = xr + Np2;
			yy = yr + Mp2;

			ip = (long)x;
			jp = (long)y;

			i1 = ip + 1;
			j1 = jp + 1;
			
			c1 = xx - ip;
			d1 = yy - jp;

			a1 = 1 - c1;
			b1 = c1;

			a2 = 1 - d1;
			b2 = d1;

			f1 = a1 * a2;
			f2 = b1 * a2;
			f3 = a1 * b2;
			f4 = a2 * b2;




			//printf("newX: %ld | newY: %ld \n", newX, newY);

			imagenOUT(i, j) = f1 * imagenIN(ip, jp) + f2 * imagenIN(i1, jp) + f3 * imagenIN(ip, j1) + f4 * imagenIN(i1, j1);
		}
	}

	/*
	// IMPLEMENTACION DIRECTA
		for (j = imagenIN.FirstCol(); j <= imagenIN.LastCol(); j++) {
		yp = j - M2;
		for (i = imagenIN.FirstRow(); i <= imagenIN.LastRow(); i++) {
			xp = i - N2;
			xr = xp * ct + yp * st;
			yr = -xp * st + yp * ct;

			newX = xr + sx;
			newY = yr + sy;
			//printf("newX: %ld | newY: %ld \n", newX, newY);

			imagenOUT(newX, newY) = imagenIN(i, j);
		}
	}
	*/
	   
	imagenOUT.palette = imagenIN.palette;
	imagenOUT.WriteBMP("Hercules_GrisMOD1.bmp");

	/*	Hay que hacerlo tb con mas algoritmos como el de los k vecinos mas cercanos y el de busqueda binaria / cuadratica	*/	

	return 0;
}