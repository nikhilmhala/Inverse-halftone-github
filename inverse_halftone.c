/*
Copyright (c) 1998 The University of Texas
All Rights Reserved.
 
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
The GNU Public License is available in the file LICENSE, or you
can write to the Free Software Foundation, Inc., 59 Temple Place -
Suite 330, Boston, MA 02111-1307, USA, or you can find it on the
World Wide Web at http://www.fsf.org.
 
Programmers:	Niranjan Damera-Venkata, Thomas D. Kite, Brian L. Evans
Version:        @(#)inverse_halftone.c	1.17	06/18/98

The authors are with the Laboratory for Image and Video Engineering
at The University of Texas at Austin, and can be reached at
{damera,tom,bevans}@vision.ece.utexas.edu.
*/

/*
The inverse halftoning algorithm implemented by this program is
explained in the following paper:

  N. Damera-Venkata, T. D. Kite, M. Venkataraman, and B. L. Evans,
    ``Fast Blind Inverse Halftoning,'' Proc. IEEE Int. Conf. on
    Image Processing, Oct. 4-7, 1998.
    http://www.ece.utexas.edu/~bevans/papers/1998/inverse_halftoning/
*/

/* Standard includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "matrix_utils.h"
#include "inverse_halftone.h"

/* Constants */

/* Normalization constant for linear filters */
#define NORMALIZATION_CONSTANT 100000000

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Macros */
 
#define SWAP(a,b) { temp = (a); (a) = (b); (b) = temp; }

/*
Filter the entire image with the separable filter given by
the coefficients in filter.  The filter is assumed to have
the same response in each dimension.  We mirror the pixel
values at the boundaries of the image.  The image is
assumed to be binary, so we implement the filtering using
only additions.  The results of the filtering are scaled 
by norm.
*/ 
static float** separable9x9FIRBinaryImage(int m, int n, float **source,
                                          float **dest, int *filter,
                                          float norm)
{
    int i, j;

    /* Temporary array to hold the result of processing the rows */
    float **ws = allocateFloatMatrix(m,n);

    /* Row convolutions */
    for (i = 0; i < m; i++) {
        float *sourceRow = source[i];
        for (j = 0; j < n; j++) {
            float sum = 0.0;

            int p1 = j-4;
            int p2 = j-3;
            int p3 = j-2;
            int p4 = j-1;
            int p6 = j+1;
            int p7 = j+2;
            int p8 = j+3;
            int p9 = j+4;

            /* reflect negative indices to non-negative values using abs */
            if (p1 < 0) p1 = -p1;
            if (p2 < 0) p2 = -p2;
            if (p3 < 0) p3 = -p3;
            if (p4 < 0) p4 = -p4;

            /* reflect indices that are too large back into proper range */
            if (p6 >= n) p6 = 2*n - p6 - 2;
            if (p7 >= n) p7 = 2*n - p7 - 2; 
            if (p8 >= n) p8 = 2*n - p8 - 2;
            if (p9 >= n) p9 = 2*n - p9 - 2;

            if (sourceRow[p1]) sum += filter[0];
            if (sourceRow[p2]) sum += filter[1];
            if (sourceRow[p3]) sum += filter[2];
            if (sourceRow[p4]) sum += filter[3];
            if (sourceRow[j])  sum += filter[4];
            if (sourceRow[p6]) sum += filter[5];
            if (sourceRow[p7]) sum += filter[6];
            if (sourceRow[p8]) sum += filter[7];
            if (sourceRow[p9]) sum += filter[8];

            ws[i][j] = 255 * sum; 
        }
    }

    /* Column convolutions */
    for(j = 0; j < n; j++) {
        for(i = 0; i < m; i++) {
            float sum = 0.0;

            int p1 = i-4;
            int p2 = i-3;
            int p3 = i-2;
            int p4 = i-1;
            int p6 = i+1;
            int p7 = i+2;
            int p8 = i+3;
            int p9 = i+4;

            /* reflect negative indices to non-negative values using abs */
            if (p1 < 0) p1 = -p1;
            if (p2 < 0) p2 = -p2;
            if (p3 < 0) p3 = -p3;
            if (p4 < 0) p4 = -p4;

            /* reflect indices that are too large back into proper range */
            if (p6 >= m) p6 = 2*m - p6 - 2; 
            if (p7 >= m) p7 = 2*m - p7 - 2;
            if (p8 >= m) p8 = 2*m - p8 - 2;
            if (p9 >= m) p9 = 2*m - p9 - 2;

            sum = (filter[4]*ws[i][j]) +
                  (filter[0]*(ws[p1][j]+ws[p9][j])) +
                  (filter[1]*(ws[p2][j]+ws[p8][j])) +
                  (filter[2]*(ws[p3][j]+ws[p7][j])) +
                  (filter[3]*(ws[p4][j]+ws[p6][j]));

            dest[i][j] = (float)((int) (sum/norm + FLOAT_TO_INT_OFFSET)); 
        }
    }

    freeFloatMatrix(ws);
    return(dest);
}

/*
Filter the entire image with the separable filter given by
the coefficients in filter.  The filter is assumed to have
the same response in each dimension.  We mirror the pixel
values at the boundaries of the image.  The image is
assumed to be grey.  The results of the filtering are scaled 
by norm.
*/ 
static float** separable7x7FIRGreyImage(int m, int n, float **source,
                                  float **dest, int *filter, float norm)
{
    int i, j;

    /* Temporary array to hold the result of processing the rows */
    float **ws = allocateFloatMatrix(m,n);

    /* Row convolutions */
    for(i = 0; i < m; i++) {
        float *sourceRow = source[i];
        for(j = 0; j < n; j++) {
            int p2 = j-3;
            int p3 = j-2;
            int p4 = j-1;
            int p6 = j+1;
            int p7 = j+2;
            int p8 = j+3;

            /* reflect negative indices to non-negative values using abs */
            if (p2 < 0) p2 = -p2;
            if (p3 < 0) p3 = -p3;
            if (p4 < 0) p4 = -p4;

            /* reflect indices that are too large back into proper range */
            if (p6 >= n) p6 = 2*n - p6 - 2;
            if (p7 >= n) p7 = 2*n - p7 - 2; 
            if (p8 >= n) p8 = 2*n - p8 - 2; 

            ws[i][j] = (filter[3]*sourceRow[j]) +
                       (filter[0]*(sourceRow[p2]+sourceRow[p8])) +
                       (filter[1]*(sourceRow[p3]+sourceRow[p7])) +
                       (filter[2]*(sourceRow[p4]+sourceRow[p6]));
        }
    }

    /* Column convolutions */
    for (j=0; j<n; j++) {
        for (i=0; i<m; i++) {
            float sum;

            int p2 = i-3;
            int p3 = i-2;
            int p4 = i-1;
            int p6 = i+1;
            int p7 = i+2;
            int p8 = i+3;

            /* reflect negative indices to non-negative values using abs */
            if (p2 < 0) p2 = -p2;
            if (p3 < 0) p3 = -p3;
            if (p4 < 0) p4 = -p4;

            /* reflect indices that are too large back into proper range */
            if (p6 >= m) p6 = 2*m - p6 - 2;
            if (p7 >= m) p7 = 2*m - p7 - 2;
            if (p8 >= m) p8 = 2*m - p8 - 2;

            sum = (filter[3]*ws[i][j]) +
                  (filter[0]*(ws[p2][j]+ws[p8][j])) +
                  (filter[1]*(ws[p3][j]+ws[p7][j])) +
                  (filter[2]*(ws[p4][j]+ws[p6][j]));

            dest[i][j] = (float)((int) (sum/norm + FLOAT_TO_INT_OFFSET)); 
        }
    }
    freeFloatMatrix(ws);
    return(dest);
}

/* First Gaussian filter: 9 x 9  */
static float** GaussianFilter1(int m, int n, float **s, float **dest)
{
  static int h1[9] =
    {11,135,808,2359,3372,2359,808,135,11};
  separable9x9FIRBinaryImage(m, n, s, dest, h1, NORMALIZATION_CONSTANT);
  return(dest);
}

/* Second Gaussian filter: 7 x 7 */
static float** GaussianFilter2(int m, int n, float **s, float **dest)
{
  static int h2[7] = {44,540,2420,3991,2420,540,44};
  separable7x7FIRGreyImage(m, n, s, dest, h2, NORMALIZATION_CONSTANT);
  return(dest);
}

/* Third Gaussian filter: 7 x 7 */
static float** GaussianFilter3(int m, int n, float **s, float **dest)
{
  static int h3[7]= {1,103,2075,5641,2075,103,1};
  separable7x7FIRGreyImage(m, n, s, dest, h3, NORMALIZATION_CONSTANT);
  return(dest);
}

/* Select the kth element from the buffer data of length arrayLen elements */
/* FIXME: Replace this routine with one that we can release under GNU terms. */
static float selectElement(unsigned int k, unsigned int arrayLen, float *data)
{
    unsigned int ir = arrayLen, l = 1;
    float temp = 0.0;

    for(;;) {
      if ( ir <= l+1) {
        if ((ir == l+1) && (data[ir] < data[l])) SWAP(data[l],data[ir]);
        break;
      }
      else {
        float a;
        unsigned int i = l + 1, j = ir;
        unsigned int mid = (l+ir) >> 1;
        SWAP(data[mid],data[l+1]);
        if (data[l] > data[ir]) SWAP(data[l],data[ir]);
        if (data[l+1] > data[ir]) SWAP(data[l+1],data[ir]);
        if (data[l] > data[l+1]) SWAP(data[l],data[l+1]);
        a = data[l+1];
        for(;;) {
           do i++; while(data[i]<a);
           do j--; while(data[j]>a);
           if (j<i) break;
           SWAP(data[i],data[j]);
        }
        data[l+1]=data[j];
        data[j]=a;
        if (j>=k) ir=j-1;
        if (j<=k) l=i;
      }
    }

    return data[k];
}

/*
Compute a 3x3 median filter on a grey image.  This operation
amounts to sorting the 9 values in the 3x3 window and picking
the middle sorted value.
*/
static float** median3x3GreyImage(int m, int n, float **x, float **x1)
{
    int i;

    for(i = 0; i < m; i++) {
        int j;
        for(j = 0; j < n; j++) { 
            float data[10];         /* selectElement begins indexing at 1 */

            int i1 = i - 1;
            int j1 = j - 1;
            if (i1 < 0) i1 = -i1;
            if (j1 < 0) j1 = -j1;

            data[1] = x[i1][j1];
            data[2] = x[i1][j];

            if (j+1>=n)
                data[3] = x[i1][2*n-(j+1)-2];
            else
                data[3] = x[i1][j+1];

            data[4] = x[i][j1];
            data[5] = x[i][j];

            if ((j+1) >= n)
                data[6] = x[i][2*n-(j+1)-2];
            else
                data[6] = x[i][j+1];

            if ((i+1) >= m)
                data[7] = x[2*m-(i+1)-2][j1];
            else
                data[7] = x[i+1][j1];

            if ((i+1) >= m)
                data[8] = x[2*m-(i+1)-2][j];
            else
                data[8] = x[i+1][j];

            if ((i+1) >= m && (j+1) >= n)
                data[9] = x[2*m-(i+1)-2][2*n-(j+1)-2];
            if ((i+1) >= m && (j+1) < n) 
                data[9] = x[2*m-(i+1)-2][j+1];
            if ((i+1) < m && (j+1) >= n)
                data[9] = x[i+1][2*n-(j+1)-2];
            if ((i+1) < m && (j+1) < n)
                data[9] = x[i+1][j+1];

            x1[i][j] = selectElement(5, 9, data);
        }    
    }

    return(x1);
}

/*
Compute the 5x5 median on a binary image.  We count the
number of '1' pixels in the 25 possible pixels in a window.
If there are 13 or more '1' pixels, then the output is 1.
*/
static float** median5x5BinaryImage(int m, int n, float **x)
{
    int i;
    float **t = allocateFloatMatrix(m,n);

    for(i = 0;i < m; i++) {
        int j;
        for(j = 0; j < n; j++) {
            int d = 0;
            int k;
            for(k = 0; k < 5; k++) {
                int l;
                for(l = 0; l < 5; l++) {
                    if ((i-k)>=0 && (j-l)>=0 && (i-k)<m && (j-l)<n) {
                        if (x[i-k][j-l]) d++;
                        if (d >= 13) break;
                    }
                }
                if (d >= 13) break;
            }
            if (d >= 13)
                t[i][j]=1;
            else
                t[i][j]=0;
        }
    }
    return t;
}

static float** thresholdDiffImage(int m, int n, float **x,
                                  float **z, int threshold, float** hie)
{
    int i;
    float **edgeMap;

    for(i = 0; i < m; i++) {
        int j;
        float *hieRow = hie[i];
        float *xRow = x[i];
        float *zRow = z[i];
        for(j = 0; j < n; j++) {
            /* Compute hie[i][j] = x[i][j] - z[i][j]; */
            float pixel = (*xRow) - (*zRow);
            if ((pixel <= threshold) && (pixel >= -threshold)) {
              *zRow++ = 0.0;
            }
            else {
              *zRow++ = 1.0;
            }
            *hieRow++ = pixel;
            xRow++;
        }      
    }

    edgeMap = median5x5BinaryImage(m, n, z);

    for(i = 0; i < m; i++) {
        int j;
        float *hieRow = hie[i];
        float *edgeMapRow = edgeMap[i];
        float *zRow = z[i];
        for(j = 0; j < n; j++) {
            /* Compute z[i][j] *= edgeMap[i][j]; hie[i][j] *= z[i][j]; */
            *hieRow++ *= (*zRow++) * (*edgeMapRow++);
        }
    }

    freeFloatMatrix(edgeMap);

    return(hie);
}

/*
Compute the last stage of the inverse halftoning algorithm.
The last stage consists of only pointwise operations.
The float and byte matrices are of dimension nrow x ncol.
*/
static unsigned char* lastStage(int nrow, int ncol, int gain,
                                float **hie, float **y1,
                                unsigned char* outputByteImage)
{
    int i;
    unsigned char* outputPtr = outputByteImage;
    float gainAsFloat = (float) gain;
    for (i = 0; i < nrow; i++) {
        int j = 0;
        float* hieRowPtr = hie[i];
        float* y1RowPtr = y1[i];
        for (j = 0; j < ncol; j++) {
           unsigned char pixelValue;
           float outputValue = FLOAT_TO_INT_OFFSET;
           outputValue += gainAsFloat * (*hieRowPtr++) + (*y1RowPtr++);
           if (outputValue < 0.0) {
               pixelValue = 0;
           }
           else if (outputValue > 255.0) {
               pixelValue = 255;
           }
           else {
               pixelValue = (unsigned char) outputValue;
           }
           *outputPtr++ = pixelValue;
        }
    }
    return(outputByteImage);
}

float** separable9x9FIRGreyImage(int m, int n, float** s, float** st,
                                 int* h, float norm)
{
    int i, j;
    float **ws = allocateFloatMatrix(m,n);

    /* Row convolutions */
    for (i = 0; i < m; i++) {
        float *wsRow = ws[i];
        float *sourceRow = s[i];
        for (j = 0; j < n; j++) {
            int p1 = j-4;
            int p2 = j-3;
            int p3 = j-2;
            int p4 = j-1;
            int p6 = j+1;
            int p7 = j+2;
            int p8 = j+3;
            int p9 = j+4;

            /* reflect negative indices to non-negative values using abs */
            if (p1 < 0) p1 = -p1;
            if (p2 < 0) p2 = -p2;
            if (p3 < 0) p3 = -p3;
            if (p4 < 0) p4 = -p4;

            /* reflect indices that are too large back into proper range */
            if (p6>=n) p6 = 2*n - p6 -2; 
            if (p7>=n) p7 = 2*n - p7 - 2;
            if (p8>=n) p8 = 2*n - p8 - 2;
            if (p9>=n) p9 = 2*n - p9 - 2;

            wsRow[j] = (h[4]*sourceRow[j]) +
                       (h[0]*(sourceRow[p1]+sourceRow[p9])) +
                       (h[1]*(sourceRow[p2]+sourceRow[p8])) +
                       (h[2]*(sourceRow[p3]+sourceRow[p7])) +
                       (h[3]*(sourceRow[p4]+sourceRow[p6]));
        }
    }

    /* Column convolutions */
    for (j = 0; j < n; j++) {
        for (i = 0; i < m; i++) {
            float sum = 0.0;

            int p1 = i-4;
            int p2 = i-3;
            int p3 = i-2;
            int p4 = i-1;
            int p6 = i+1;
            int p7 = i+2;
            int p8 = i+3;
            int p9 = i+4;

            /* reflect negative indices to non-negative values using abs */
            if (p1 < 0) p1 = -p1;
            if (p2 < 0) p2 = -p2;
            if (p3 < 0) p3 = -p3;
            if (p4 < 0) p4 = -p4;

            /* reflect indices that are too large back into proper range */
            if (p6>=m) p6 = 2*m - p6 - 2; 
            if (p7>=m) p7 = 2*m - p7 - 2;
            if (p8>=m) p8 = 2*m - p8 - 2;
            if (p9>=m) p9 = 2*m - p9 - 2;

            sum = (h[4]*ws[i][j]) +
                  (h[0]*(ws[p1][j]+ws[p9][j])) +
                  (h[1]*(ws[p2][j]+ws[p8][j])) +
                  (h[2]*(ws[p3][j]+ws[p7][j])) +
                  (h[3]*(ws[p4][j]+ws[p6][j]));
 
            st[i][j]= (float)((int) (sum/norm + FLOAT_TO_INT_OFFSET)); 
        }
    }
    freeFloatMatrix(ws);
    return(st);
}

float** median5x5GreyImage(int m, int n, float** x, float** x1)
{
int i;
 
for(i=0;i<m;i++){
   int j;
   for(j=0;j<n;j++){ 
       float data[26];         /* selectElement begins indexing at 1 */
       data[1]=x[abs(i-2)][abs(j-2)];
       data[2]=x[abs(i-2)][abs(j-1)];
       data[3]=x[abs(i-2)][j];
       if(j+1>=n)
           data[4]=x[abs(i-2)][2*n-(j+1)-2];
       else
           data[4]=x[abs(i-2)][j+1];

       if(j+2>=n)
           data[5]=x[abs(i-2)][2*n-(j+2)-2];
       else
           data[5]=x[abs(i-2)][j+2];
       
       data[6]=x[abs(i-1)][abs(j-2)];
       data[7]=x[abs(i-1)][abs(j-1)];
       data[8]=x[abs(i-1)][j];
    
       if((j+1)>=n)
           data[9]=x[abs(i-1)][2*n-(j+1)-2];
       else
           data[9]=x[abs(i-1)][j+1];

       if((j+2)>=n)
           data[10]=x[abs(i-1)][2*n-(j+2)-2];
       else
           data[10]=x[abs(i-1)][j+2];   

       data[11]=x[i][abs(j-2)];        
       data[12]=x[i][abs(j-1)];
       data[13]=x[i][j];
   
       if((j+1)>=n)
           data[14]=x[i][2*n-(j+1)-2];
       else
           data[14]=x[i][j+1];

       if((j+2)>=n)
           data[15]=x[i][2*n-(j+2)-2];
       else
           data[15]=x[i][j+2];

       if((i+1)>=m){
           data[16]=x[2*m-(i+1)-2][abs(j-2)];
           data[17]=x[2*m-(i+1)-2][abs(j-1)];
           data[18]=x[2*m-(i+1)-2][j];
       }
       else {
           data[16]=x[i+1][abs(j-2)];
           data[17]=x[i+1][abs(j-1)];
           data[18]=x[i+1][abs(j)];
       }

       if((i+1)>=m && (j+1)>=n)data[19]=x[2*m-(i+1)-2][2*n-(j+1)-2];
       if((i+1)>=m && (j+1)<n)data[19]=x[2*m-(i+1)-2][j+1];
       if((i+1)<m && (j+1)>=n)data[19]=x[i+1][2*n-(j+1)-2];
       if((i+1)<m && (j+1)<n)data[19]=x[i+1][j+1];      

       if((i+1)>=m && (j+2)>=n)data[20]=x[2*m-(i+1)-2][2*n-(j+2)-2];
       if((i+1)>=m && (j+2)<n)data[20]=x[2*m-(i+1)-2][j+2];
       if((i+1)<m && (j+2)>=n)data[20]=x[i+1][2*n-(j+2)-2];
       if((i+1)<m && (j+2)<n)data[20]=x[i+1][j+2];

       if((i+2)>=m && (j+1)>=n)data[24]=x[2*m-(i+2)-2][2*n-(j+1)-2];
       if((i+2)>=m && (j+1)<n)data[24]=x[2*m-(i+2)-2][j+1];
       if((i+2)<m && (j+1)>=n)data[24]=x[i+2][2*n-(j+1)-2];
       if((i+2)<m && (j+1)<n)data[24]=x[i+2][j+1];

       if((i+2)>=m && (j+2)>=n)data[25]=x[2*m-(i+2)-2][2*n-(j+2)-2];
       if((i+2)>=m && (j+2)<n)data[25]=x[2*m-(i+2)-2][j+2];
       if((i+2)<m && (j+2)>=n)data[25]=x[i+2][2*n-(j+2)-2];
       if((i+2)<m && (j+2)<n)data[25]=x[i+2][j+2];

       if((i+2)>=m) {
           data[21]=x[2*m-(i+2)-2][abs(j-2)];
           data[22]=x[2*m-(i+2)-2][abs(j-1)];
           data[23]=x[2*m-(i+2)-2][j];
       }
       else {
           data[21]=x[i+2][abs(j-2)];
           data[22]=x[i+2][abs(j-1)];
           data[23]=x[i+2][j];
       }

       x1[i][j] = selectElement(13, 25, data);
    }    
}
return x1;
}

/* First Gaussian filter for Dispersed dot dither: 9 x 9  */
static float** GaussianFilterDispDith1(int m, int n, float **s, float **dest)
{
  static int hd[9] =
    {103,419,1138,2074,2533,2074,1138,419,103};
  separable9x9FIRBinaryImage(m, n, s, dest, hd, NORMALIZATION_CONSTANT);
  return(dest);
}

/* First Gaussian filter for Clustered dot dither: 9 x 9  */
static float** GaussianFilterClustDith1(int m, int n, float **s, float **dest)
{
  static int hc[9] =
    {583, 903, 1234, 1488, 1584, 1488, 1234, 903, 583};
  separable9x9FIRBinaryImage(m, n, s, dest, hc, NORMALIZATION_CONSTANT);
  return(dest);
}

/* Second Gaussian filter: 9 x 9 */
static float** GaussianFilterDith2(int m, int n, float **s, float **dest)
{
  static int hD2[9] = 
     {1,44,540,2420,3989,2420,540,44,1};
  separable9x9FIRGreyImage(m, n, s, dest, hD2, NORMALIZATION_CONSTANT);
  return(dest);
}

/* Third Gaussian filter: 9 x 9 */
static float** GaussianFilterDith3(int m, int n, float **s, float **dest)
{
  static int hD3[9]= 
     {0,1,103,2075,5641,2075,103,1,0};
  separable9x9FIRGreyImage(m, n, s, dest, hD3, NORMALIZATION_CONSTANT);
  return(dest);
}


/*
Compute the inverse halftone of inputImage and store the result in
outputImage.  The inputImage and outputImage is of size numRows by
numColumns.  The return value is the amount of computation time
if timingFlag is TRUE, and 0.0 otherwise.  A negative return value 
indicates an error.
*/
double inverseHalftone(unsigned char* inputByteImage,
                       unsigned char* outputByteImage,
                       int numRows, int numColumns,
                       int gain, int threshold,
                       int timingFlag, int halftoningType)
{
    int maxRows = numRows;
    int maxColumns = numColumns;
    int i;
    int errorFlag = FALSE;
    double computationTime = 0.0;
    unsigned char* tempBytePtr = 0;
    time_t startTime, finishTime;

    /* Allocation intermediate images */
    float** inputImage = allocateFloatMatrix(maxRows, maxColumns);
    float** z = allocateFloatMatrix(maxRows, maxColumns);
    float** y0 = allocateFloatMatrix(maxRows, maxColumns);
    float** y1 = allocateFloatMatrix(maxRows, maxColumns);
    float** y2 = allocateFloatMatrix(maxRows, maxColumns);
    float** hie = allocateFloatMatrix(maxRows, maxColumns);

    /* Check memory allocation, and return an error upon failure */
    if ((inputImage == 0) || (z == 0) ||
        (y0 == 0) || (y1 == 0) || (y2 == 0) || (hie == 0)) {
        computationTime = -1.0;        /* Fixes a Visual C++ warning */
        return(computationTime);
    }

    /* Convert the byte input image to a float-point image */
    tempBytePtr = inputByteImage;
    for (i = 0; i < numRows; i++) {
        int j = 0;
        float *floatRowImagePtr = inputImage[i];
        for (j = 0; j < numColumns; j++) {
            *floatRowImagePtr++ = (float) *tempBytePtr++;
        }
    }

    /* Perform inverse halftoning on inputImage and report the time */
    /* The last two steps are the same for all three algorithms. */
    if (timingFlag) time(&startTime);

    switch(halftoningType) {
      case HALFTONING_BY_ERROR_DIFFUSION:
        GaussianFilter1(numRows, numColumns, inputImage, y0);      /* set y0 */
        median3x3GreyImage(numRows, numColumns, y0, y1);           /* set y1 */
        GaussianFilter2(numRows, numColumns, y1, y2);              /* set y2 */
        GaussianFilter3(numRows, numColumns, y2, z);               /* set z  */
        thresholdDiffImage(numRows, numColumns, y2, z, threshold, hie); 
        lastStage(numRows, numColumns, gain, hie, y1, outputByteImage);
        break;

      case HALFTONING_BY_DISPERED_DITHER:
        GaussianFilterDispDith1(numRows, numColumns, inputImage, y0);
        median5x5GreyImage(numRows, numColumns, y0, y1);           /* set y1 */
        GaussianFilterDith2(numRows, numColumns, y1, y2);          /* set y2 */
        GaussianFilterDith3(numRows, numColumns, y2, z);           /* set z  */
        thresholdDiffImage(numRows, numColumns, y2, z, threshold, hie); 
        lastStage(numRows, numColumns, gain, hie, y1, outputByteImage);
        break;

      case HALFTONING_BY_CLUSTERED_DITHER:
        GaussianFilterClustDith1(numRows, numColumns, inputImage, y0);
        median5x5GreyImage(numRows, numColumns, y0, y1);           /* set y1 */
        GaussianFilterDith2(numRows, numColumns, y1, y2);          /* set y2 */
        GaussianFilterDith3(numRows, numColumns, y2, z);           /* set z  */
        thresholdDiffImage(numRows, numColumns, y2, z, threshold, hie); 
        lastStage(numRows, numColumns, gain, hie, y1, outputByteImage);
        break;

      default:
        errorFlag = TRUE;
    }

    if (errorFlag) {
        computationTime = -2.0;
    }
    else if (timingFlag) {
        time(&finishTime);
        computationTime = difftime(finishTime, startTime);
    }

    /* Deallocate intermediate images */ 
    freeFloatMatrix(inputImage);
    freeFloatMatrix(z);
    freeFloatMatrix(y0);
    freeFloatMatrix(y1);
    freeFloatMatrix(y2);
    freeFloatMatrix(hie);

    return(computationTime);
}
