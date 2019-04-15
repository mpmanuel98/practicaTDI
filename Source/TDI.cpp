#include <stdio.h>
#include <iostream>
#include <math.h>
#include <C_General.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

#define PI 3.14159265

double max(double a) {
	if (a > 0) return a;
	return 0;
}

double calculoCoef(double k) {
	double res;
	res = (pow(max(k + 2), 3) + (-4 * pow(max(k + 1), 3)) + (6 * pow(max(k), 3)) + (-4 * pow(max(k - 1), 3))) / 6;
	return res;
}

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
	imagenIN.ReadBMP("Cuad.bmp");
	imagenIN.Reindex(5000, 5000);

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

	//Filas y columnas de la matriz original
	long N = imagenIN.RowN();
	long M = imagenIN.ColN();
	
	//Mitad de fila y columna de la matriz original
	double N2 = N / 2;
	double M2 = M / 2;

	//Valores centrales en la matriz original (corrimientos acumulados)
	double sxIN = imagenIN.FirstRow() + N2;
	double syIN = imagenIN.FirstCol() + M2;

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

	//Se crea la nueva imagen (matriz)
	C_Image imagenOUT = C_Image(p1, p2, q1, q2, 127.0);
	imagenOUT.Reindex(5000, 5000);

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
	printf("0 -> Algoritmo de rotacion directa (vecino mas cercano).\n");
	printf("1 -> Algoritmo de rotacion inversa (vecino mas cercano).\n");
	printf("2 -> Algoritmo de rotacion inversa (interpolacion bilineal).\n");
	printf("3 -> Algoritmo de rotacion inversa (interpolacion bicubica).\n");

	int algoritmo = -1;
	do {
		printf("Seleccione el algoritmo que desea aplicar: ");
		scanf_s("%ld", &algoritmo);
	} while (algoritmo < 0 || algoritmo > 3);

	long ip, jp;
	switch (algoritmo) {
		case 0:
			//ALGORITMO DE IMPLEMENTACION DIRECTA DE LA ROTACION (vecino mas cercano)
			for (j = imagenIN.FirstCol(); j <= imagenIN.LastCol(); j++) {
				yp = j - syIN;
				for (i = imagenIN.FirstRow(); i <= imagenIN.LastRow(); i++) {
					xp = i - sxIN;

					xr = xp * ct + yp * st;
					yr = -xp * st + yp * ct;

					//Con lround se redondean las coordenadas a las del pixel mas cercano
					ip = lround(xr + sxOUT);
					jp = lround(yr + syOUT);

					//Se mapearan en la nueva imagen aquellos pixeles que no se salgan de su rango
					if ((ip >= imagenOUT.FirstRow()) && (jp >= imagenOUT.FirstCol()) && (ip <= imagenOUT.LastRow()) && (jp <= imagenOUT.LastCol())) {
						imagenOUT(ip, jp) = imagenIN(i, j);
					}
				}
			}
			break;

		case 1:
			//ALGORITMO DE IMPLEMENTACION INVERSA DE LA ROTACION (vecino mas cercano)
			for (j = imagenOUT.FirstCol(); j <= imagenOUT.LastCol(); j++) {
				yp = j - syOUT;
				for (i = imagenOUT.FirstRow(); i <= imagenOUT.LastRow(); i++) {
					xp = i - sxOUT;

					xr = xp * ct + yp * st;
					yr = -xp * st + yp * ct;

					//Con lround se redondean las coordenadas a las del pixel mas cercano
					ip = lround(xr + sxIN);
					jp = lround(yr + syIN);

					//Se mapearan en la nueva imagen aquellos pixeles que existan en la imagen original
					if ((ip >= imagenIN.FirstRow()) && (jp >= imagenIN.FirstCol()) && (ip <= imagenIN.LastRow()) && (jp <= imagenIN.LastCol())) {
						imagenOUT(i, j) = imagenIN(ip, jp);
					}
				}
			}
			break;

		case 2:
			//ALGORITMO DE ROTACION INVERSA CON INTERPOLACION BILINEAR
			double xx, yy, f1, f2, f3, f4;
			long i1, j1;
			for (j = imagenOUT.FirstCol(); j <= imagenOUT.LastCol(); j++) {
				yp = j - syOUT;
				for (i = imagenOUT.FirstRow(); i <= imagenOUT.LastRow(); i++) {
					xp = i - sxOUT;

					xr = xp * ct + yp * st;
					yr = -xp * st + yp * ct;

					xx = xr + sxIN;
					yy = yr + syIN;

					ip = trunc(xx);			//Parte entera de x
					jp = trunc(yy);			//Parte entera de y

					i1 = ip + 1;			//Fila inferior a ip
					j1 = jp + 1;			//Columna derecha a jp

					if ((ip >= imagenIN.FirstRow()) && (i1 <= imagenIN.LastRow()) && (jp >= imagenIN.FirstCol()) && (j1 <= imagenIN.LastCol())) {
						//printf("xx: %lf, yy: %lf, ip: %ld, jp: %ld, i1: %ld, j1: %ld \n", xx, yy, ip, jp, i1, j1);

						//Se calculan los coeficientes para cada pixel de alrededor del obtenido en funcion de la distancia entre ellos
						f1 = ((i1 - xx)*(j1 - yy)) / ((i1 - ip)*(j1 - jp));

						f2 = ((xx - ip)*(j1 - yy)) / ((i1 - ip)*(j1 - jp));

						f3 = ((i1 - xx)*(yy - jp)) / ((i1 - ip)*(j1 - jp));

						f4 = ((xx - ip)*(yy - jp)) / ((i1 - ip)*(j1 - jp));

						//printf("f1: %lf, f2: %lf, f3: %lf, f4: %lf, suma: %lf \n", f1, f2, f3, f4, (f1 + f2 + f3 + f4));						
						imagenOUT(i, j) = f1 * imagenIN(ip, jp) + f2 * imagenIN(ip, j1) + f3 * imagenIN(i1, jp) + f4 * imagenIN(i1, j1);

						//printf("Valor pixel: %lf \n", imagenOUT(i,j));
					}
				}
			}
			break;

		case 3:
			//ALGORITMO DE ROTACION INVERSA CON INTERPOLACION BICUBICA
			double valorPixel;
			long ip1, ip2, jp1, jp2, beforeip, beforejp;
			for (j = imagenOUT.FirstCol(); j <= imagenOUT.LastCol(); j++) {
				yp = j - syOUT;
				for (i = imagenOUT.FirstRow(); i <= imagenOUT.LastRow(); i++) {
					xp = i - sxOUT;

					xr = xp * ct + yp * st;
					yr = -xp * st + yp * ct;

					xx = xr + sxIN;
					yy = yr + syIN;

					ip = trunc(xx);			//Parte entera de x
					jp = trunc(yy);			//Parte entera de y

					ip1 = ip + 1;			//Fila 1 mas adelante a ip
					ip2 = ip + 2;			//Fila 2 mas adelante a ip
					beforeip = ip - 1;		//Fila anterior a ip

					jp1 = jp + 1;			//Columna 1 mas adelante a jp
					jp2 = jp + 2;			//Columna 2 mas adelante a jp
					beforejp = jp - 1;		//Columna anterior a jp

					valorPixel = 0;
					if ((beforeip >= imagenIN.FirstRow()) && (ip2 <= imagenIN.LastRow()) && (beforejp >= imagenIN.FirstCol()) && (jp2 <= imagenIN.LastCol())) {
						//Pixel (0,0)
						valorPixel += (imagenIN(beforeip, beforejp)	* calculoCoef(-1 - (xx - beforeip))		* calculoCoef((yy - beforejp) - (-1)));
						//Pixel (0,1)
						valorPixel += (imagenIN(ip, beforejp)		* calculoCoef(0 - (xx - ip))			* calculoCoef((yy - beforejp) - (-1)));
						//Pixel (0,2)
						valorPixel += (imagenIN(ip1, beforejp)		* calculoCoef(1 - (ip1 - xx))			* calculoCoef((yy - beforejp) - (-1)));
						//Pixel (0,3)
						valorPixel += (imagenIN(ip2, beforejp)		* calculoCoef(2 - (ip2 - xx))			* calculoCoef((yy - beforejp) - (-1)));

						//Pixel (1,0)
						valorPixel += (imagenIN(beforeip, jp)		* calculoCoef(-1 - (xx - beforeip))		* calculoCoef(yy - jp));
						//Pixel (1,1)
						valorPixel += (imagenIN(ip, jp)				* calculoCoef(0 - (xx - ip))			* calculoCoef(yy - jp));
						//Pixel (1,2)
						valorPixel += (imagenIN(ip1, jp)			* calculoCoef(1 - (ip1 - xx))			* calculoCoef(yy - jp));
						//Pixel (1,3)
						valorPixel += (imagenIN(ip2, jp)			* calculoCoef(2 - (ip2 - xx))			* calculoCoef(yy - jp));

						//Pixel (2,0)
						valorPixel += (imagenIN(beforeip, jp1)		* calculoCoef(-1 - (xx - beforeip))		* calculoCoef((jp1 - yy) - 1));
						//Pixel (2,1)
						valorPixel += (imagenIN(ip, jp1)			* calculoCoef(0 - (xx - ip))			* calculoCoef((jp1 - yy) - 1));
						//Pixel (2,2)
						valorPixel += (imagenIN(ip1, jp1)			* calculoCoef(1 - (ip1 - xx))			* calculoCoef((jp1 - yy) - 1));
						//Pixel (2,3)
						valorPixel += (imagenIN(ip2, jp1)			* calculoCoef(2 - (ip2 - xx))			* calculoCoef((jp1 - yy) - 1));

						//Pixel (3,0)
						valorPixel += (imagenIN(beforeip, jp2)		* calculoCoef(-1 - (xx - beforeip))		* calculoCoef((jp2 - yy) - 2));
						//Pixel (3,1)
						valorPixel += (imagenIN(ip, jp2)			* calculoCoef(0 - (xx - ip))			* calculoCoef((jp2 - yy) - 2));
						//Pixel (3,2)
						valorPixel += (imagenIN(ip1, jp2)			* calculoCoef(1 - (ip1 - xx))			* calculoCoef((jp2 - yy) - 2));
						//Pixel (3,3)
						valorPixel += (imagenIN(ip2, jp2)			* calculoCoef(2 - (ip2 - xx))			* calculoCoef((jp2 - yy) - 2));

						//printf("valorPixel:  %lf \n", valorPixel);

						imagenOUT(i, j) = valorPixel;
					}
				}
			}
			break;

		default:
			printf("No ha seleccionado ningun algoritmo valido");
			break;
	}
	
	printf("MAX: %lf , MIN: %lf \n", imagenOUT.Max(), imagenOUT.Min());

	imagenOUT.palette = imagenIN.palette;
	imagenOUT.WriteBMP("ImagenRotada.bmp");

	return 0;
}