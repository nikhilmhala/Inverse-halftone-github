/*
 * @(#)readWritePPM.h	1.6	06/21/98
 *
 *      The source code is a version of tkImgPPM.c from the
 * Tcl/Tk 8.0 distribution modifed by Brian L. Evans of The
 * University of Texas at Austin.
 *
 *      The tkImgPPM.c module is a photo image file handler for
 * PPM (Portable PixMap) files.
 *
 * Copyright (c) 1994 The Australian National University.
 * Copyright (c) 1994-1997 Sun Microsystems, Inc.
 *
 * Author: Paul Mackerras (paulus@cs.anu.edu.au),
 *	   Department of Computer Science,
 *	   Australian National University.
 *
 * SCCS: @(#) tkImgPPM.c 1.16 97/10/28 14:51:46
 */

/*
This software is copyrighted by the Regents of the University of
California, Sun Microsystems, Inc., and other parties.  The following
terms apply to all files associated with the software unless explicitly
disclaimed in individual files.

The authors hereby grant permission to use, copy, modify, distribute,
and license this software and its documentation for any purpose, provided
that existing copyright notices are retained in all copies and that this
notice is included verbatim in any distributions. No written agreement,
license, or royalty fee is required for any of the authorized uses.
Modifications to this software may be copyrighted by their authors
and need not follow the licensing terms described here, provided that
the new terms are clearly indicated on the first page of each file where
they apply.
 
IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

GOVERNMENT USE: If you are acquiring this software on behalf of the
U.S. government, the Government shall have only "Restricted Rights"
in the software and related documentation as defined in the Federal 
Acquisition Regulations (FARs) in Clause 52.227.19 (c) (2).  If you
are acquiring the software on behalf of the Department of Defense, the
software shall be classified as "Commercial Computer Software" and the
Government shall have only "Restricted Rights" as defined in Clause
252.227-7013 (c) (1) of DFARs.  Notwithstanding the foregoing, the
authors grant the U.S. Government and others acting in its behalf
permission to use and distribute the software in accordance with the
terms specified in this license.
*/

#ifndef _READWRITEPPM_H
#define _READWRITEPPM_H

#include "readWriteImage.h"

/*
 * Utility function
 */

int            InitImageInfo _ANSI_ARGS_((
                            Tk_PhotoImageBlock* blockPtr,
                            unsigned char* imgBufferPtr,
                            int type, int width, int height));

/*
 * The format record for the PPM file format:
 */

int            FileMatchPPM _ANSI_ARGS_((
                            char *fileName, int *widthPtr, int *heightPtr));
int            FileReadPPM  _ANSI_ARGS_((
                            char *filename, byte *imgBufferPtr,
                            int width, int height));
int            FileWritePPMHeader _ANSI_ARGS_((
                            FILE *chan, Tk_PhotoImageBlock *blockPtr));
int            FileWritePPM _ANSI_ARGS_((
                            char *filename, Tk_PhotoImageBlock *blockPtr));

/*
 * Prototypes for local procedures defined in this file:
 */

int            ReadPPMFileHeader _ANSI_ARGS_((
                            FILE* chan,
                            int *widthPtr, int *heightPtr,
                            int *maxIntensityPtr));

#endif
