/*
Copyright (c) 1997-1998 The University of Texas
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

Programmer:	Thomas D. Kite
Version:        @(#)fastiht2.c	1.6	06/21/98

The author is with the Laboratory for Image and Video Engineering
at The University of Texas at Austin, and can be reached at
tom@vision.ece.utexas.edu.
*/

/*
fastiht2.c  Inverse halftoning algorithm

Compile with

    gcc  -O2 -o fastiht2 fastiht2.c

or equivalent C compiler.  Run the program using

     ./fastiht2 lena_halftone_512x512 test2 512 512
      
The arguments are
     
     halftoneFile input output imageWidth imageHeight

where the input file and output file are raw 8-bit grayscale images
of size xsize by ysize.  The input file is of course binary, and must
consist of 0 for black and any non-zero integer for white.  With the
distribution, we have provided a file 'lena_halftone_512x512' which
is the error diffused halftone of the file 'lena_512x512'.
*/

/*
The inverse halftoning algorithm implemented by this program is
explained in the following paper:

    T. D. Kite, N. Damera-Venkata, B. L. Evans, and A. C. Bovik,
    ``A High Quality, Fast Inverse Halftoning Algorithm for Error
    Diffused Halftones,'' Proc. IEEE Int. Conf. on Image
    Processing, Oct. 4-7, 1998, to appear.
    http://www.ece.utexas.edu/~bevans/papers/1998/error_diffusion/
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readWriteImage.h"
#include "readWritePPM.h"

#define DEFAULT_IMAGE_DIMENSION 512

#define USAGE_STRING \
  "Usage: %s infile outfile [xsize] [ysize]\n" \
  "This is a fast inverse halftoning algorithm for error diffused\n" \
  "halftones. The infile can be either a raw image or a portable\n" \
  "graymap (PGM) file. For raw images, xsize and ysize default to %d.\n" \
  "See http://www.ece.utexas.edu/~bevans/papers/1998/error_diffusion/\n" \
  "for an explanation of the algorithm.\n"

struct filedata {                               /* command line arguments */
  FILE *ifp, *ofp;
  short xsize, ysize;
};

typedef struct filedata filedata;
typedef unsigned char pixel;                    /* 8-bit pixels */
typedef signed char filt;                       /* 8-bit filter coefficients */
typedef signed short fout;                      /* filter outputs */

/* Allocate and clear memory, and bomb if not available */
static void* my_alloc(int size)
{
  static int count=0;
  void *newptr = (void*) malloc(size);
  if (!newptr) {
    fprintf(stderr, "Failed to allocate array %d.  Exiting.\n", count);
    exit(-1);
  }
  count++;
  return newptr;
}

/* Process command line arguments */
static filedata* process_args(int argc, char *argv[])
{
  filedata* out = (filedata*) my_alloc(sizeof(filedata));

  if ((argc < 3) || (argc > 5)) {
    fprintf(stderr, USAGE_STRING, argv[0], DEFAULT_IMAGE_DIMENSION);
    fprintf(stderr,
            "You passed %d arguments and 2-4 arguments are required.\n",
            argc - 1);
    exit(-1);
  }
  if ((out->ifp = fopen(argv[1],"r")) == NULL) {
    fprintf(stderr, "Can't open file %s\n for reading", argv[1]);
    exit(-1);
  }
  if ((out->ofp = fopen(argv[2],"w")) == NULL) {
    fprintf(stderr, "Can't open file %s\n for writing", argv[2]);
    exit(-1);
  }
  out->xsize = DEFAULT_IMAGE_DIMENSION;
  if ((argc > 3) && (out->xsize=atoi(argv[3])) == 0) {
    fprintf(stderr, "Invalid xsize\n");
    exit(-1);
  }
  out->ysize = DEFAULT_IMAGE_DIMENSION;
  if ((argc > 4) && (out->ysize=atoi(argv[4])) == 0) {
    fprintf(stderr, "Invalid ysize\n");
    exit(-1);
  }

  return out;
}


void main(int argc, char* argv[])
{
  Tk_PhotoImageBlock block;
  filedata* fdata;
  FILE *ifp, *ofp;
  short xsize, ysize, xpadsize, rowadd, row, col;
  int xsizeInt, ysizeInt;
  pixel *imtl, *imbr, *imptr2;
  register pixel *imptr, *imcmp;
  filt f2x[25]={19,32,0,-32,-19,55,92,0,-92,-55, \
                72,120,0,-120,-72,55,92,0,-92,-55, \
                19,32,0,-32,-19};
  filt f2y[25]={19,55,72,55,19,32,92,120,92,32, \
                0,0,0,0,0,-32,-92,-120,-92,-32, \
                -19,-55,-71,-55,-19};
  filt f3x[49]={12,27,25,0,-25,-27,-12,30,68,64,0,-64,-68,-30, \
                45,103,96,0,-96,-103,-45,54,124,114,0,-114,-124,-54, \
                45,103,96,0,-96,-103,-45,30,68,64,0,-64,-68,-30, \
                12,27,25,0,-25,-27,-12};
  filt f3y[49]={12,30,45,54,45,30,12,27,68,103,124,103,68,27, \
                25,64,96,114,96,64,25,0,0,0,0,0,0,0, \
                -25,-64,-96,-114,-96,-64,-25,-27,-68,-103,-124,-103,-68,-27, \
                -12,-30,-45,-54,-45,-30,-12};
  filt *f2xp, *f2yp, *f3xp, *f3yp;
  register fout t2x, t2y, t3x, t3y;
  int fx[7]={0,0,0,4*1024,0,0,0}, *fxptr;
  int fy[7]={0,0,0,4*1024,0,0,0}, *fyptr;
  int out, outrow;
  int imageType, dummy;
  float cx1, cy1, cx2, cy2, xcomp, ycomp, out2;

  /*
    filtscale scales the output of the product of the edge detectors.
    It is computed as 1/(1024*2048*2048), since that is how much the
    coefficients of the edge detectors are scaled to get them to fit
    into one byte each.
    filtscale2 scales the output of the smoothing filter.  It is 
    computed as 255/(16*1024*1024).  The 255/16 comes from the need
    to scale to the full 0-255 range, and the fact that the scaling
    factor for the separable filter has a 4 in the denominator (see
    report).  The two factors of 1024 compensate for the multiplication
    applied to the floating-point coefficients before they are converted
    to integers for fast implementation.
  */

  float filtscale = 2.328306436538696e-10;          /* scale edge detector */
  float filtscale2 = 1.519918441772461e-5;          /* scale smooth filter */

  fdata = process_args(argc, argv);                 /* parse command line */
  xsize = fdata->xsize;
  ysize = fdata->ysize;
  ifp = fdata->ifp;
  ofp = fdata->ofp;

  /* Read and write image headers */
  imageType = ReadPPMFileHeader(ifp, &ysizeInt, &xsizeInt, &dummy);
  switch(imageType) {
    case RAW:  
      /* no header to read */
      rewind(ifp);
      break;
    case PGM:
      xsize = xsizeInt;
      ysize = ysizeInt;
      InitImageInfo(&block, 0, PGM, ysizeInt, xsizeInt);
      FileWritePPMHeader(ofp, &block);
      break;
    case PPM:
      fprintf(stderr,
              "File '%s' is a color image, but color images "
              "are currently not supported.\n",
              argv[1]);
      break;
    default:
      fprintf(stderr, "Unrecognized image type %d.\n", imageType);
      break;
  }

  xpadsize = xsize+6;                               /* extended image size */
  rowadd = xsize-1;                                 /* to get to next row */
  imtl = (pixel*) my_alloc(xpadsize*7);             /* 7 row store */
  imbr = imtl+xpadsize*7;                           /* bottom right + 1 */

  /* Load in the first four image rows.  Mirror at sides and above */

  for (imptr=imtl+xpadsize*3+3, imptr2 = imptr; imptr<imbr; imptr+=8) {
    fread(imptr, 1, xsize, ifp);
    *--imptr2=*++imptr; *--imptr2=*++imptr; *--imptr2=*++imptr;
    imptr+=xsize-7; imptr2+=xpadsize;
    *--imptr2=*imptr++; *--imptr2=*imptr++; *--imptr2=*imptr; imptr2+=6;
  }
  imptr2=imtl+(xpadsize<<2);
  for (imptr = imtl+(xpadsize<<1); imptr>=imtl; imptr-=xpadsize) {
    memcpy(imptr, imptr2, xpadsize);
    imptr2+=xpadsize;
  }

  /* Loop over image */

  for (row=0; row<ysize; row++) {                       /* all image rows */
    for (col=0; col<xsize; col++) {                     /* all image cols */

      /* Compute the four gradient estimator outputs t{2,3}{x,y} */

      t2x=0; t2y=0; f2xp=f2x; f2yp=f2y;                 /* zero totals */
      t3x=0; t3y=0; f3xp=f3x; f3yp=f3y;                 /* and reset filters */
      for (imptr=imtl+col, imcmp=imptr+7; imptr<imcmp;) /* row 1 (3) */
        if (*imptr++) {t3x+=*f3xp++; t3y+=*f3yp++;}
          else {f3xp++; f3yp++;}
      for (imptr+=rowadd; imptr<imtl+xpadsize*6; imptr+=rowadd){ /* rows 2-6 */
        if (*imptr++) {t3x+=*f3xp++; t3y+=*f3yp++;}     /* col 1 (3) */
          else {f3xp++; f3yp++;}
        for (imcmp=imptr+5; imptr<imcmp;)               /* cols 2-6 (2,3) */
          if (*imptr++) {t2x+=*f2xp++; t2y+=*f2yp++;
                         t3x+=*f3xp++; t3y+=*f3yp++;}
            else {f2xp++; f2yp++; f3xp++; f3yp++;}
        if (*imptr++) {t3x+=*f3xp++; t3y+=*f3yp++;}     /* col 7 (3) */
          else {f3xp++; f3yp++;}
      }
      for (imcmp=imptr+7; imptr<imcmp;)                 /* row 7 (3) */
        if (*imptr++) {t3x+=*f3xp++; t3y+=*f3yp++;}
          else {f3xp++; f3yp++;}

      /*
         Compute composite gradients.  First, find the absolute value of 
         the product of the gradients at two scales.  Then find the cube
         root of this product.  First, check for the inputs that lead to
         the limits of 3.33 and 1.95 and exclude them.  For valid inputs,
         compute a first approximation using a two-line fit, then perform
         two Newton-Raphson iterations.  Maximum error is 2e-4.
      */

      xcomp = (float) t2x*t3x*t3x * filtscale;          /* comp gradient */
      if (xcomp) {                                      /* < max. smoothing? */
        if (xcomp<0) xcomp=-xcomp;                      /* abs */
        if (xcomp>0.0141909899) cx1=1.95;               /* min smoothing */
        else {
          if (xcomp>0.001) cx1=9.6742*xcomp+0.116165;   /* first linear fit */
          else cx1=98.87505*xcomp+0.027;                /* second linear fit */
          xcomp=xcomp*0.3333333333;
          cx1=0.66666666*cx1+xcomp/(cx1*cx1);           /* Newton-Raphson */
          cx1=0.66666666*cx1+xcomp/(cx1*cx1);
          cx1=3.33-5.7*cx1;                             /* filter coeff. */
        }
      }
      else cx1=3.33;                                    /* max smoothing */

      ycomp = (float) t2y*t3y*t3y * filtscale;          /* comp gradient */
      if (ycomp) {                                      /* < max. smoothing? */
        if (ycomp<0) ycomp=-ycomp;                      /* abs */
        if (ycomp>0.0141909899) cy1=1.95;               /* min smoothing */
        else {
          if (ycomp>0.001) cy1=9.6742*ycomp+0.116165;   /* first linear fit */
          else cy1=98.87505*ycomp+0.027;                /* second linear fit */
          ycomp=ycomp*0.3333333333;
          cy1=0.66666666*cy1+ycomp/(cy1*cy1);           /* Newton-Raphson */
          cy1=0.66666666*cy1+ycomp/(cy1*cy1);
          cy1=3.33-5.7*cy1;                             /* filter coeff. */
        }
      }
      else cy1=3.33;                                    /* max smoothing */

      /* Compute second parameter of filters and construct integer filters */
      /* with max. 13 bit coefficients */

      cx2=-3.611679+cx1*(4.659894+cx1*(-2.426115+cx1*0.4630577));
      fx[1]=1024*cx2; cx2+=2; fx[0]=1024*(cx2-cx1); fx[2]=1024*cx1;
      fx[4]=fx[2]; fx[5]=fx[1]; fx[6]=fx[0];

      cy2=-3.611679+cy1*(4.659894+cy1*(-2.426115+cy1*0.4630577));
      fy[1]=1024*cy2; cy2+=2; fy[0]=1024*(cy2-cy1); fy[2]=1024*cy1;
      fy[4]=fy[2]; fy[5]=fy[1]; fy[6]=fy[0];

      /*
         Compute output by separable filtering.  We treat the first pixel
         of each row, and the first row sum, uniquely, to avoid an addition.
         This saves 7 extra additions per pixel.
       */
      fyptr=fy; imptr=imtl+col;

      fxptr=fx;
      if (*imptr++) outrow=*fxptr++;                    /* row 1, col 1 */
        else {outrow=0; fxptr++;}
      for (; fxptr<fx+7;)                               /* row 1, cols 2-7 */
        if (*imptr++) outrow+=*fxptr++;
          else fxptr++;
      out = outrow * *fyptr++;                          /* y filter */

      for (imptr+=rowadd; imptr<imbr; imptr+=rowadd) {  /* rows 2-7 */
        fxptr=fx;
        if (*imptr++) outrow=*fxptr++;                  /* col 1 */
          else {outrow=0; fxptr++;}
        for (; fxptr<fx+7;)                             /* cols 2-7 */
          if (*imptr++) outrow+=*fxptr++;
            else fxptr++;
        out += outrow * *fyptr++;                       /* y filter */
      }

      /* Scale output and write to file */

      out2=filtscale2 * out / (cx2*cy2) + 0.5;          /* scale and round */
      out2=(out2<0) ? 0 : (out2>255) ? 255 : out2;      /* clip */
      putc((unsigned char) out2, ofp);                  /* write to file */
    }

    /* Shift input and either read in row and mirror sides or mirror old row */

    memcpy(imtl, imtl+xpadsize, xpadsize*6);            /* shift up one row */
    if (row<ysize-4) {                                  /* get new row */
      imptr=imtl+xpadsize*6+3;                          /* and mirror */
      imptr2=imptr;
      fread(imptr, 1, xsize, ifp);
      *--imptr2=*++imptr; *--imptr2=*++imptr; *--imptr2=*++imptr;
      imptr+=xsize-7; imptr2+=xpadsize;
      *--imptr2=*imptr++; *--imptr2=*imptr++; *--imptr2=*imptr;
    }
    else if (row!=ysize-1) {                            /* mirror old row */
      imptr=imtl+xpadsize*6;
      imptr2=imtl+xpadsize*((ysize-row-2)<<1);
      memcpy(imptr,imptr2,xpadsize);
    }
  }

  /* All done */

  fclose(ifp);
  fclose(ofp);
  free(fdata);
  free(imtl);
}
