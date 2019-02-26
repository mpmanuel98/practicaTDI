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
	angulo = 0;
	coseno = cos(angulo);
	seno = sin(angulo);

	long filaCentro = imagenIN.RowN() / 2;
	long columnaCentro = imagenIN.ColN() / 2;
	long x[] = { 0, imagenIN.LastRow(), 0, imagenIN.LastRow() };
	long y[] = { 0, 0, imagenIN.LastCol(), imagenIN.LastCol() };
	long p [4];
	long q [4];

	long xp, xr, yp, yr;

	for (i = 0; i < 4; i++) {
		xp = x[i] - filaCentro;
		yp = y[i] - columnaCentro;
		xr = xp * coseno + yp * seno;
		yr = -xp * seno + yp * coseno;

		p[i] = xr + filaCentro;
		q[i] = yr + columnaCentro;

		printf("%ld - %ld \n", p[i], q[i]);
	}

	long p1 = 9999999999, p2 = -9999999999, q1 = 9999999999, q2 = -9999999999;

	for (i = 0; i < 4; i++) {
		if (p[i] < p1) p1 = p[i];
		if (p[i] > p2) p2 = p[i];
		if (q[i] < q1) q1 = q[i];
		if (q[i] > q2) q2 = q[i];
	}
	printf("%ld - %ld - %ld - %ld\n", p1, p2, q1, q2);

	int filasNewImag, columnasNewImag;
	filasNewImag = p2 - p1 + 1;
	columnasNewImag = q2 - q1 + 1;
	printf("%ld - %ld \n", filasNewImag, columnasNewImag);

	C_Image imagenOUT = C_Image(0, filasNewImag, 0, columnasNewImag, 0.0);

	long sx, sy;
	sx = p1 + filaCentro;
	sy = q1 + columnaCentro;

	long newX, newY;

	for (j = 0; j < columnasNewImag; j++) {
		yp = j - columnaCentro;
		for (i = 0; i < filasNewImag; i++) {
			xp = i - filaCentro;
			xr = xp * coseno + yp * seno;
			yr = -xp * seno + yp * coseno;

			newX = xr + sx;
			newY = yr + sy;

			imagenOUT(newX, newY) = imagenIN(i, j);
		}
	}


	imagenOUT.palette = imagenIN.palette;
	imagenOUT.WriteBMP("Hercules_GrisMOD.bmp");
	return 0;
}