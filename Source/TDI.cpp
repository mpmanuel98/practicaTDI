#include <stdio.h>
#include <math.h>
#include <C_General.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

#define PI 3.14159265

int main(int argc, char **argv)
{
	int i, j, angulo;
	C_Image imagenIN;
	imagenIN.ReadBMP("Hercules_Gris.bmp");

	double seno, coseno;
	angulo = 25;

	coseno = cos((angulo * 2 * PI) / 360);
	seno = sin((angulo * 2 * PI) / 360);

	long n2 = imagenIN.RowN() / 2;
	long m2 = imagenIN.ColN() / 2;
	printf("n2 = %ld \t m2 = %ld \n", n2, m2);

	//long x[] = { 0, imagenIN.LastRow() - 1, 0, imagenIN.LastRow() - 1 };
	//long y[] = { 0, 0, imagenIN.LastCol() - 1, imagenIN.LastCol() - 1 };
	long x[] = { imagenIN.FirstRow(), imagenIN.LastRow(), imagenIN.FirstRow(), imagenIN.LastRow() };
	long y[] = { imagenIN.FirstCol(), imagenIN.FirstCol(), imagenIN.LastCol(), imagenIN.LastCol() };

	long p [4];
	long q [4];

	long xp, xr, yp, yr;

	for (i = 0; i < 4; i++) {
		printf("[_________________________________] x[%d] = %ld \t y[%d] = %ld \n", i, x[i], i, y[i]);
		xp = x[i] - n2;
		yp = y[i] - m2;
		xr = xp * coseno + yp * seno;
		yr = -xp * seno + yp * coseno;

		p[i] = xr + n2;
		q[i] = yr + m2;

		printf("p[%d] = %ld \t q[%d] = %ld \n", i, p[i], i, q[i]);
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
	printf("p1: %ld p2: %ld q1: %ld q2: %ld\n", p1, p2, q1, q2);

	int filasNewImag, columnasNewImag;
	filasNewImag = p2 - p1 + 1;
	columnasNewImag = q2 - q1 + 1;
	printf("Filas: %ld | Columnas: %ld \n", filasNewImag, columnasNewImag);

	C_Image imagenOUT = C_Image(p1, p2, q1, q2, 127.0);

	long newn2 = imagenOUT.RowN() / 2;
	long newm2 = imagenOUT.ColN() / 2;

	long sx, sy;
	sx = p1 + newn2;
	sy = q1 + newm2;

	long newX, newY;

	for (j = imagenIN.FirstCol(); j <= imagenIN.LastCol(); j++) {
		yp = j - m2;
		for (i = imagenIN.FirstRow(); i <= imagenIN.LastRow(); i++) {
			xp = i - n2;
			xr = xp * coseno + yp * seno;
			yr = -xp * seno + yp * coseno;

			newX = xr + sx;
			newY = yr + sy;
			printf("newX: %ld | newY: %ld \n", newX, newY);
			imagenOUT(newX, newY) = imagenIN(i, j);
		}
	}

	/*
	for (j = imagenIN.FirstCol(); j < imagenIN.LastCol(); j++) {
		yp = j - m2;
		for (i = imagenIN.FirstRow(); i < imagenIN.LastRow(); i++) {
			xp = i - n2;
			xr = xp * coseno + yp * seno;
			yr = -xp * seno + yp * coseno;

			newX = xr + sx;
			newY = yr + sy;

			imagenOUT(newX, newY) = imagenIN(i, j);
		}
	}
	*/

	   
	imagenOUT.palette = imagenIN.palette;
	imagenOUT.WriteBMP("Hercules_GrisMOD1.bmp");
	return 0;
}