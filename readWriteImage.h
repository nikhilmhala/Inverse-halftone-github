/*
 * @(#)readWriteImage.h	1.2	06/21/98
 *
 *      This include file includes definitions from tcl.h and
 * tk.h from the Tcl/Tk 8.0 distribution, arranged by Brian L.
 * Evans of The University of Texas at Austin.
 *
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

#ifndef _READWRITEIMAGE_H
#define _READWRITEIMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define UCHAR(c) ((unsigned char) (c))

#define TCL_OK          0
#define TCL_ERROR       1
#define TCL_RETURN      2
#define TCL_BREAK       3
#define TCL_CONTINUE    4

/*
 * Define types of images.
 * RAW: raw grayscale image
 * PGM: gray image
 * PPM: color image
 */
 
#define RAW 0
#define PGM 1
#define PPM 2

#ifndef _ANSI_ARGS_
#define _ANSI_ARGS_(x) x
#endif

typedef unsigned char byte;

/*
 * The following structure describes a block of pixels in memory:
 */
 
typedef struct Tk_PhotoImageBlock {
    unsigned char *pixelPtr;    /* Pointer to the first pixel. */
    int         width;          /* Width of block, in pixels. */
    int         height;         /* Height of block, in pixels. */
    int         pitch;          /* Address difference between corresponding
                                 * pixels in successive lines. */
    int         pixelSize;      /* Address difference between successive
                                 * pixels in the same line. */
    int         offset[3];      /* Address differences between the red, green
                                 * and blue components of the pixel and the
                                 * pixel as a whole. */
} Tk_PhotoImageBlock;

#endif
