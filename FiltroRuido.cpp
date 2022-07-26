#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
    Mat imagen;
    imagen = imread("lena.jpg", 1);

    uchar Azul;
    uchar Verde;
    uchar Rojo;
    uchar Orden;

    RNG numero;
    float aleatorio;

    cv::Vec3b pixel;

    int filas = imagen.rows;
    int columnas = imagen.cols;
    int sumaTotal = 0;
    int sumaTotalUniforme = 0;

    Mat gris(filas, columnas, CV_8UC1);

    Mat grisRuidoGaussiano(filas, columnas, CV_8UC1);
    Mat imagenBordesGaussiano(filas + 2, columnas + 2, CV_8UC1);
    Mat imagenSinRuidoGaussiano(filas, columnas, CV_8UC1);

    Mat grisRuidoSalPimienta(filas, columnas, CV_8UC1);
    Mat imagenBordesSalPimienta(filas + 2, columnas + 2, CV_8UC1);
    Mat imagenSinRuidoSalPimienta(filas, columnas, CV_8UC1);

    Mat grisRuidoUniforme(filas, columnas, CV_8UC1);
    Mat imagenBordesUniforme(filas + 2, columnas + 2, CV_8UC1);
    Mat imagenSinRuidoUniforme(filas, columnas, CV_8UC1);

    Mat matrizLineal(1, 9, CV_8UC1);

    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            pixel = imagen.at<Vec3b>(i, j);

            Azul = pixel[0];
            Verde = pixel[1];
            Rojo = pixel[2];

            gris.at<uchar>(i, j) = (Azul + Verde + Rojo) / 3;
            grisRuidoGaussiano.at<uchar>(i, j) = gris.at<uchar>(i, j)*(numero.uniform(0.f, 1.f));

            grisRuidoSalPimienta.at<uchar>(i, j) = gris.at<uchar>(i, j);
            if ((1000 * (numero.uniform(0.f, 1.f))) > 950)
            {
                grisRuidoSalPimienta.at<uchar>(i, j) = 255;
            }
            else if ((1000 * (numero.uniform(0.f, 1.f))) < 50)
            {
                grisRuidoSalPimienta.at<uchar>(i, j) = 0;
            }

            grisRuidoUniforme.at<uchar>(i, j) = gris.at<uchar>(i, j);
            if ( ( j >= (1*(columnas/4)) && j <= (3*(columnas/4)) ) )
            {
                grisRuidoUniforme.at<uchar>(i, j) = gris.at<uchar>(i, j)* (0.4);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

             /////////////////////////////////   FILTRO DE MEDIA     //////////////////////////////////

    for (int k = 0; k < filas + 2; k++)
    {
        for (int l = 0; l < columnas + 2; l++)
        {
            if ((k == 0) || (l == 0))
            {
                imagenBordesGaussiano.at<uchar>(k, l) = 0;
                imagenBordesUniforme.at<uchar>(k, l) = 0;
            }
            else if ((k == filas + 1) || (l == columnas + 1))
            {
                imagenBordesGaussiano.at<uchar>(k, l) = 0;
                imagenBordesUniforme.at<uchar>(k, l) = 0;
            }
            else
            {
                imagenBordesGaussiano.at<uchar>(k, l) = grisRuidoGaussiano.at<uchar>(k - 1, l - 1);
                imagenBordesUniforme.at<uchar>(k, l) = grisRuidoUniforme.at<uchar>(k - 1, l - 1);
            }
        }
    }

    for (int m = 0; m < filas; m++)
    {
        for (int n = 0; n < columnas; n++)
        {
            for (int p = 0; p < 3; p++)
            {
                for (int q = 0; q < 3; q++)
                {
                    sumaTotal = sumaTotal + imagenBordesGaussiano.at<uchar>(p + m, q + n);
                    sumaTotalUniforme = sumaTotalUniforme + imagenBordesUniforme.at<uchar>(p + m, q + n);
                }
            }

            imagenSinRuidoGaussiano.at<uchar>(m, n) = sumaTotal / 9;
            imagenSinRuidoUniforme.at<uchar>(m, n) = sumaTotalUniforme / 9;

            sumaTotal = 0;
            sumaTotalUniforme = 0;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////   FILTRO DE MEDIANA     //////////////////////////////////

    for (int r = 0; r < filas + 2; r++)
    {
        for (int s = 0; s < columnas + 2; s++)
        {
            if ((r == 0) || (s == 0))
            {
                imagenBordesSalPimienta.at<uchar>(r, s) = 0;
            }
            else if ((r == filas + 1) || (s == columnas + 1))
            {
                imagenBordesSalPimienta.at<uchar>(r, s) = 0;
            }
            else
            {
                imagenBordesSalPimienta.at<uchar>(r, s) = grisRuidoSalPimienta.at<uchar>(r - 1, s - 1);
            }
        }
    }

    for (int t = 0; t < filas; t++)
    {
        for (int u = 0; u < columnas; u++)
        {

            for (int v = 0; v < 3; v++)
            {
                for (int w = 0; w < 3; w++)
                {
                    matrizLineal.at<uchar>(0, 3*v + w) = imagenBordesSalPimienta.at<uchar>(t + v, u + w);
                }
            }

            for (int x = 0; x < 9; x++)
            {
                for (int y = 0; y < 8 - x; y++)
                {
                    if (matrizLineal.at<uchar>(0, y) > matrizLineal.at<uchar>(0, y + 1))
                    {
                        Orden = matrizLineal.at<uchar>(0, y);
                        matrizLineal.at<uchar>(0, y) = matrizLineal.at<uchar>(0, y + 1);
                        matrizLineal.at<uchar>(0, y + 1) = Orden;
                    }

                }
            }

            imagenSinRuidoSalPimienta.at<uchar>(t, u) = matrizLineal.at<uchar>(0, (9 + 1) / 2);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////



    //imshow("Imagen a color", imagen);
    //imshow("Imagen gris", gris);
    //imshow("Imagen gris con ruido gaussiano", grisRuidoGaussiano);
    //imshow("Imagen gris con ruido gaussiano con bordes", imagenBordesGaussiano);
    //imshow("Imagen gris sin ruido gaussiano", imagenSinRuidoGaussiano);
    //imshow("Imagen gris con ruido sal y pimienta", grisRuidoSalPimienta);
    //imshow("Imagen gris con ruido sal y pimienta con bordes", imagenBordesSalPimienta);
    //imshow("Imagen gris sin ruido sal y pimienta", imagenSinRuidoSalPimienta);
    imshow("Imagen gris con ruido uniforme", grisRuidoUniforme);
    //imshow("Imagen gris con ruido sal y pimienta con bordes", imagenBordesUniforme);
    //imshow("Imagen gris sin ruido sal y pimienta", imagenSinRuidoUniforme);

    waitKey();
    return 0;
}