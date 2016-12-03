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
Version:        @(#)fastiht1.c	1.24	06/21/98

The authors are with the Laboratory for Image and Video Engineering
at The University of Texas at Austin, and can be reached at
{damera,tom,bevans}@vision.ece.utexas.edu.
*/

/*
An example of running this program is:

./fastiht1 lena_halftone_512x512 test 0 4 512 512

The 'lena_halftone_512x512' file contains the error diffused halftone
version of the 512 x 512 Lena image in the 'lena_512x512' file.  This
fastiht1 algorithm by default prints the execution time of the
algorithm not counting the time to read and write the images from disk.
(You can change the behavior by changing the value of the constant
TIME_EXECUTION_FLAG in this file to 0 and then rebuild the program.)
The timing may only be accurate to one second.  We ran the algorithm
for 100 iterations on a 167 MHz Ultra-2 workstation and found that the
algorithm took 2.4 seconds on average to process the 512 x 512 halftone
in 'lena_halftone_512x512'.
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

#include "image_io.h"
#include "matrix_utils.h"
#include "inverse_halftone.h"

/* Constants */

#define DEFAULT_IMAGE_DIMENSION 512

#define TIME_EXECUTION_FLAG 1

#define USAGE_STRING \
  "Usage: %s halfFile inverseFile threshold gain halfType [rows] [columns]\n" \
  "This is a fast inverse halftoning algorithm, where halfType is 1 for\n" \
  "error diffusion, 2 for dispersed dither, and 3 for clustered dither. The\n" \
  "infile can be either a raw image or a portable graymap (PGM) file. For\n" \
  "raw images, the number of rows and number of columns default to %d.\n" \
  "See http://www.ece.utexas.edu/~bevans/papers/1998/inverse_halftoning/\n" \
  "for an explanation of the algorithm.\n"

/* Read an integer from the string numericStr; and exit program on failure. */
static int readIntArg(char *descStr, char *numericStr, int minValue) {
    int tempInt = 0;
    int validIntFlag = 0;
    int readValue = 0;
    validIntFlag = (sscanf(numericStr, "%d", &tempInt) == 1);
    if (validIntFlag && (tempInt >= minValue)) {
        readValue = tempInt;
    }
    else {
        fprintf(stderr,
                "%s, %s, is not an integer greater than or equal to %d.\n",
                descStr, numericStr, minValue);
        exit(1);
    }
    return(readValue);
}

/* Main routine */
int main(int argc, char *argv[])
{
    unsigned char *inputByteImage = 0, *outputByteImage = 0;
    int gain = 0, threshold = 0;
    int numRows = DEFAULT_IMAGE_DIMENSION,
        numColumns = DEFAULT_IMAGE_DIMENSION;
    int exitStatus = 0;
    int halftoningType = 0, imageType = 0;
    double execTime = 0.0;

    /* Check for the right number of arguments */
    if ((argc < 6) || (argc > 8)) {
        fprintf(stderr, USAGE_STRING, argv[0], DEFAULT_IMAGE_DIMENSION);
        fprintf(stderr,
                "You passed %d arguments and 5-7 arguments are required.\n",
                argc - 1);
        exit(1);
    }

    /* Get parameters */
    threshold = readIntArg("Threshold", argv[3], 0);
    gain = readIntArg("Gain", argv[4], 0);
    halftoningType = readIntArg("Type of halftoning", argv[5],
                                 HALFTONING_BY_ERROR_DIFFUSION);
    if (argc >= 7) {
        numRows = readIntArg("Number of rows", argv[6], 1);
    }
    if (argc == 8) {
        numColumns = readIntArg("Number of columns", argv[7], 1);
    }
    else {
        numColumns = numRows;
    }

    /* Read the halftoned image: the filename is given by argv[1] */
    imageType = readByteImage(argv[1], &inputByteImage, &numRows, &numColumns);

    /* Allocate the output byte image */
    outputByteImage = (unsigned char *) malloc(numRows*numColumns*sizeof(char));
    if (outputByteImage == 0) {
        fprintf(stderr, "Could not allocate enough memory for images.\n");
        exit(1);
    }

    /* Process image in variable inputImage and report the time */
    execTime = inverseHalftone(inputByteImage, outputByteImage,
                               numRows, numColumns, gain, threshold,
                               TIME_EXECUTION_FLAG, halftoningType);

    exitStatus = (execTime < 0.0);
    if (exitStatus) {
      if (execTime == INVERSE_HALFTONING_NO_MEMORY) {
        fprintf(stderr,
          "Could not allocate enough memory in the inverseHalftone routine.\n");
      }
      else if (execTime == INVERSE_HALFTONING_BAD_METHOD) {
        fprintf(stderr,
                "Invalid halftoning method %d specified.\n",
                halftoningType);
      }
      else {
        fprintf(stderr,
          "Error encountered in the inverseHalftone routine.\n");
      }
    }
    else {
        /* Report computation time and save the result */
        printf("%f sec\n", execTime);
        writeByteImage(argv[2], outputByteImage,
                       &numRows, &numColumns, imageType);
    }

    return(exitStatus);
}
