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
Version:        @(#)matrix_utils.c	1.5	06/14/98

The authors are with the Laboratory for Image and Video Engineering
at The University of Texas at Austin, and can be reached at
{damera,tom,bevans}@vision.ece.utexas.edu.
*/

/* Standard includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix_utils.h"

#ifndef TRUE
#define TRUE 1
#endif
 
#ifndef FALSE
#define FALSE 0
#endif

/*  Allocate a float matrix */
/*  FIXME: Reference for the source of this code and check copyright. */
float **allocateFloatMatrix(int height, int width)
{
    int i;
    float **floatMatrix = (float **) malloc(height*sizeof(float *));
  
    if (!floatMatrix) {
        fprintf(stderr, "Cannot allocate memory in allocateFloatMatrix.\n");
        exit(1);
    }
    floatMatrix[0] = (float *) malloc(width*height*sizeof(float));
    if (!floatMatrix[0]) {
        fprintf(stderr, "Cannot allocate matrix in allocateFloatMatrix.\n");
        exit(2);
    }
    for (i = 1; i < height; i++) floatMatrix[i] = floatMatrix[i-1] + width;
    return(floatMatrix);
}

/*  Deallocate a float matrix */
int freeFloatMatrix(float **floatMatrix)
{
    if (floatMatrix == 0) {
        return FALSE;
    }
    if (floatMatrix[0]) {
        free(floatMatrix[0]);
        floatMatrix[0] = 0;
    }
    free(floatMatrix);
    return TRUE;
}
