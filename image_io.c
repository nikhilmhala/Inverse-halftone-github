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
Version:        @(#)image_io.c	1.9	06/21/98

The authors are with the Laboratory for Image and Video Engineering
at The University of Texas at Austin, and can be reached at
{damera,tom,bevans}@vision.ece.utexas.edu.
*/

/* Standard includes */

#include <stdio.h>
#include <stdlib.h>

#include "image_io.h"
#include "readWriteImage.h"
#include "readWritePPM.h"

/* Allocate a raw byte image */
static unsigned char* allocateImage(int numRows, int numColumns, int pixelSize)
{
    return((unsigned char*) calloc(numRows,
                                   numColumns*sizeof(unsigned char)*pixelSize));
}

/* Read 8 bit per pixel raw image */
static void readRawByteImage(char* filename, unsigned char *imgBufferPtr,
                             int *numRowsPtr, int *numColumnsPtr)
{
    FILE *fp = fopen(filename,"r");
    if (fp != NULL) {
        if (fread(imgBufferPtr, *numRowsPtr, *numColumnsPtr, fp) == 0) {
            fprintf(stderr, "Error reading file '%s'.\n", filename);
            exit(1);
        }
    }
    else {
        fprintf(stderr, "Error opening file '%s' for reading.\n", filename);
        exit(1);
    }
    fclose(fp);
}

/* Write 8 bit per pixel raw image data */
static void writeRawByteImage(char* filename, unsigned char *imgBufferPtr,
                              int *numRowsPtr, int *numColumnsPtr)
{
    FILE *fp = fopen(filename, "w");
    if (fp != NULL) {
        if ( fwrite(imgBufferPtr, *numRowsPtr,
                    (*numColumnsPtr) * sizeof(unsigned char), fp) == 0 ) {
            fprintf(stderr, "Error writing file '%s'.\n", filename);
            exit(1);
        }
    }
    else {
        fprintf(stderr, "Error opening file '%s' for writing.\n", filename);
        exit(1);
    }
    fclose(fp);
}


/*
Read a byte image.  Could be in raw or PGM formats.
Return image information cast as a char*.  Return
a null pointer on an error.
*/
int readByteImage(char* filename, unsigned char **imgBufferPtrPtr,
                  int *numRowsPtr, int *numColumnsPtr)
{
    /* Check to see if the filename is a PPM file */
    int ppmType = FileMatchPPM(filename, numColumnsPtr, numRowsPtr);
    switch (ppmType) {
      case RAW:
        *imgBufferPtrPtr = allocateImage(*numRowsPtr, *numColumnsPtr, 1);
        readRawByteImage(filename, *imgBufferPtrPtr, numRowsPtr, numColumnsPtr);
        break;

      case PGM:
        *imgBufferPtrPtr = allocateImage(*numRowsPtr, *numColumnsPtr, 1);
        FileReadPPM(filename, *imgBufferPtrPtr, *numColumnsPtr, *numRowsPtr);
        break;

      case PPM:
        fprintf(stderr,
                "File '%s' is a color image, but color images "
                "are currently not supported.\n",
                filename);
        break;

      default:
        fprintf(stderr, "Unrecognized image type %d.\n", ppmType);
        break;
    }

    return(ppmType);
}

/* Write a byte image.  Could be in raw or PGM formats. */
void writeByteImage(char* filename, unsigned char *imgBufferPtr,
                    int *numRowsPtr, int *numColumnsPtr, int imageType)
{
    Tk_PhotoImageBlock block;

    switch (imageType) {
      case RAW:
        writeRawByteImage(filename, imgBufferPtr, numRowsPtr, numColumnsPtr);
        break;

      case PGM:
        InitImageInfo(&block, imgBufferPtr, imageType,
                      *numColumnsPtr, *numRowsPtr);
        FileWritePPM(filename, &block);
        break;

      case PPM:
        fprintf(stderr,
                "Request to write '%s' as a color image was not fulfilled "
                "because color images are currently not supported.\n",
                filename);
        break;

      default:
        fprintf(stderr, "Unrecognized image type %d.\n", imageType);
        break;
    }
}
