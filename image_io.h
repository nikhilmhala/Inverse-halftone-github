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
Version:        @(#)image_io.h	1.3	06/21/98

The authors are with the Laboratory for Image and Video Engineering
at The University of Texas at Austin, and can be reached at
{damera,tom,bevans}@vision.ece.utexas.edu.
*/

#ifndef _IMAGE_IO_H
#define _IMAGE_IO_H

int readByteImage(char* filename, unsigned char **imgBufferPtrPtr,
                  int *numRowsPtr, int *numColumnsPtr);
void writeByteImage(char* filename, unsigned char *imgBufferPtr,
                    int *numRowsPtr, int *numColumnsPtr, int imageType);

#endif
