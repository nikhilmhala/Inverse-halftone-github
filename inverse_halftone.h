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
Version:        @(#)inverse_halftone.h	1.5	06/18/98

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

#ifndef _INVERSE_HALFTONE_H
#define _INVERSE_HALFTONE_H

#define HALFTONING_BY_ERROR_DIFFUSION 1
#define HALFTONING_BY_DISPERED_DITHER 2
#define HALFTONING_BY_CLUSTERED_DITHER 3

#define INVERSE_HALFTONING_NO_MEMORY  -1.0
#define INVERSE_HALFTONING_BAD_METHOD -2.0

double inverseHalftone(unsigned char* inputImage, unsigned char* outputImage,
                       int numRows, int numColumns,
                       int gain, int threshold, int timingFlag,
		       int halftoningType);

#endif

