/*
 * @(#)readWritePPM.c	1.6	06/21/98
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

#include "readWritePPM.h"

/*
 * The maximum amount of memory to allocate for data read from the
 * file.  If we need more than this, we do it in pieces.
 */

#define MAX_MEMORY	10000		/* don't allocate > 10KB */


/*
Initializes an instance of a Tk_PhotoImageBlock.
*/
int
InitImageInfo(Tk_PhotoImageBlock *blockPtr, unsigned char* imgBufferPtr,
              int type, int width, int height)
{
    if (blockPtr == NULL) return TCL_ERROR;

    blockPtr->pixelPtr = imgBufferPtr;

    if (type == PPM) {
        blockPtr->pixelSize = 3;
        blockPtr->offset[0] = 0;
	blockPtr->offset[1] = 1;
	blockPtr->offset[2] = 2;
    }
    else {
        blockPtr->pixelSize = 1;
        blockPtr->offset[0] = 0;
	blockPtr->offset[1] = 0;
	blockPtr->offset[2] = 0;
    }
    blockPtr->width = width;
    blockPtr->height = height;
    blockPtr->pitch = blockPtr->pixelSize * width;

    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * FileMatchPPM --
 *
 *	This procedure is invoked by the photo image type to see if
 *	a file contains image data in PPM format.
 *
 * Results:
 *	The return value is >0 if the first characters in file "f" look
 *	like PPM data, and 0 otherwise.
 *
 * Side effects:
 *	The access position in f may change.
 *
 *----------------------------------------------------------------------
 */

int
FileMatchPPM(fileName, widthPtr, heightPtr)
    char *fileName;		/* The name of the image file. */
    int *widthPtr, *heightPtr;	/* The dimensions of the image are
				 * returned here if the file is a valid
				 * raw PPM file. */
{
    int dummy, retval;

    FILE* chan = fopen(fileName, "r");
    if (chan == NULL) {
	return 0;
    }
    retval = ReadPPMFileHeader(chan, widthPtr, heightPtr, &dummy);
    fclose(chan);

    return(retval);
}

/*
 *----------------------------------------------------------------------
 *
 * FileReadPPM --
 *
 *	This procedure is called by the photo image type to read
 *	PPM format data from a file and write it into a given
 *	photo image.
 *
 * Results:
 *	A standard TCL completion code.  If TCL_ERROR is returned
 *	then an error message is printed to stderr.
 *
 * Side effects:
 *	The access position in file f is changed, and new data is
 *	added to the image given by imageHandle.
 *
 *----------------------------------------------------------------------
 */
int
FileReadPPM(fileName, imgBufferPtr, width, height)
    char *fileName;		/* The name of the image file. */
    byte *imgBufferPtr;		/* The photo image to write into. */
    int width, height;          /* Dimensions of block of photo image to
                                 * be written to. */
{
    int fileWidth, fileHeight, maxIntensity;
    int type;
    FILE* chan = 0;
    Tk_PhotoImageBlock block;

    /* Open the portable anymap (PPM) image file */
    chan = fopen(fileName, "r");
    if (chan == NULL) {
        fprintf(stderr, "File '%s' could not be opened for reading.\n",
		fileName);
        return TCL_ERROR;
    }

    /* Read the header of the (PPM) image file and report errors if any */
    type = ReadPPMFileHeader(chan, &fileWidth, &fileHeight, &maxIntensity);
    if (type == 0) {
        fprintf(stderr, "Couldn't read raw PPM header from file '%s'.\n",
		fileName);
	return TCL_ERROR;
    }
    if ((fileWidth <= 0) || (fileHeight <= 0)) {
        fprintf(stderr, "PPM image file '%s' has dimension(s) <= 0.\n",
	        fileName);
	return TCL_ERROR;
    }
    if ((maxIntensity <= 0) || (maxIntensity >= 256)) {
	fprintf(stderr,
	        "PPM image file '%s' has bad maximum intensity value %d.\n",
	        fileName,
		maxIntensity);
	return TCL_ERROR;
    }
    if ((width != fileWidth) || (height != fileHeight)) {
	fprintf(stderr,
	        "The width and height requested do not match "
		"the image read from '%s'.\n",
	        fileName);
	return TCL_ERROR;
    }

    /* Read the contents of the (PPM) image file */
    InitImageInfo(&block, imgBufferPtr, type, width, height);
    if (fread(imgBufferPtr, height, width * block.pixelSize, chan) == 0) {
        fprintf(stderr, "Error reading file '%s'.\n", fileName);
        fclose(chan);
	return TCL_ERROR;
    }

    fclose(chan);
    return TCL_OK;
}

int FileWritePPMHeader(FILE *chan, Tk_PhotoImageBlock *blockPtr)
{
    char header[30];
    char* id = (blockPtr->pixelSize == 3) ? "P6" : "P5";
    sprintf(header, "%s\n%d %d\n255\n", id, blockPtr->width, blockPtr->height);
    fprintf(chan, header);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * FileWritePPM --
 *
 *	This procedure is invoked to write image data to a file in PPM
 *	format.
 *
 * Results:
 *	A standard TCL completion code.  If TCL_ERROR is returned
 *	then an error message is written to stderr.
 *
 * Side effects:
 *	Data is written to the file given by "fileName".
 *
 *----------------------------------------------------------------------
 */

int
FileWritePPM(fileName, blockPtr)
    char *fileName;
    Tk_PhotoImageBlock *blockPtr;
{
    FILE* chan = 0;
    int w, h;
    int greenOffset, blueOffset, nBytes;
    unsigned char *pixelPtr, *pixLinePtr;

    chan = fopen(fileName, "w");
    if (chan == NULL) {
	return TCL_ERROR;
    }

    FileWritePPMHeader(chan, blockPtr);

    pixLinePtr = blockPtr->pixelPtr + blockPtr->offset[0];
    greenOffset = blockPtr->offset[1] - blockPtr->offset[0];
    blueOffset = blockPtr->offset[2] - blockPtr->offset[0];

    if ((greenOffset == 1) && (blueOffset == 2) && (blockPtr->pixelSize == 3)
	    && (blockPtr->pitch == (blockPtr->width * 3))) {
	nBytes = blockPtr->height * blockPtr->pitch;
	if (fwrite((char *) pixLinePtr, nBytes, 1, chan) == 0) {
	    goto writeerror;
	}
    } else if (blockPtr->pixelSize == 1) {
	nBytes = blockPtr->height * blockPtr->width;
	if (fwrite((char *) pixLinePtr, nBytes, 1, chan) == 0) {
	    goto writeerror;
	}
    } else {
	for (h = blockPtr->height; h > 0; h--) {
	    pixelPtr = pixLinePtr;
	    for (w = blockPtr->width; w > 0; w--) {
		if ((fwrite((char *) &pixelPtr[0], 1, 1, chan) == 0)
			|| (fwrite((char *) &pixelPtr[greenOffset], 1, 1, chan) == 0)
			|| (fwrite((char *) &pixelPtr[blueOffset], 1, 1, chan) == 0)) {
		    goto writeerror;
		}
		pixelPtr += blockPtr->pixelSize;
	    }
	    pixLinePtr += blockPtr->pitch;
	}
    }

    if (fclose(chan) == 0) {
        return TCL_OK;
    }
    chan = NULL;

 writeerror:
    fprintf(stderr, "Error writing '%s'.\n", fileName);
    if (chan != NULL) {
	fclose(chan);
    }
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * ReadPPMFileHeader --
 *
 *	This procedure reads the PPM header from the beginning of a
 *	PPM file and returns information from the header.
 *
 * Results:
 *	The return value is PGM if file "f" appears to start with
 *	a valid PGM header, PPM if "f" appears to start with a valid
 *      PPM header, and 0 otherwise.  If the header is valid,
 *	then *widthPtr and *heightPtr are modified to hold the
 *	dimensions of the image and *maxIntensityPtr is modified to
 *	hold the value of a "fully on" intensity value.
 *
 * Side effects:
 *	The access position in f advances.
 *
 *----------------------------------------------------------------------
 */

int
ReadPPMFileHeader(chan, widthPtr, heightPtr, maxIntensityPtr)
    FILE* chan;		/* Image file to read the header from */
    int *widthPtr, *heightPtr;	/* The dimensions of the image are
				 * returned here. */
    int *maxIntensityPtr;	/* The maximum intensity value for
				 * the image is stored here. */
{
#define BUFFER_SIZE 1000
    char buffer[BUFFER_SIZE];
    int i, numFields, firstInLine;
    int type = 0;
    char c;

    /*
     * Read 4 space-separated fields from the file, ignoring
     * comments (any line that starts with "#").
     */

    if (fread(&c, 1, 1, chan) != 1) {
	return 0;
    }
    firstInLine = 1;
    i = 0;
    for (numFields = 0; numFields < 4; numFields++) {
	/*
	 * Skip comments and white space.
	 */

	while (1) {
	    while (isspace(UCHAR(c))) {
		firstInLine = (c == '\n');
		if (fread(&c, 1, 1, chan) != 1) {
		    return 0;
		}
	    }
	    if (c != '#') {
		break;
	    }
	    do {
		if (fread(&c, 1, 1, chan) != 1) {
		    return 0;
		}
	    } while (c != '\n');
	    firstInLine = 1;
	}

	/*
	 * Read a field (everything up to the next white space).
	 */

	while (!isspace(UCHAR(c))) {
	    if (i < (BUFFER_SIZE-2)) {
		buffer[i] = c;
		i++;
	    }
	    if (fread(&c, 1, 1, chan) != 1) {
		goto done;
	    }
	}
	if (i < (BUFFER_SIZE-1)) {
	    buffer[i] = ' ';
	    i++;
	}
	firstInLine = 0;
    }
    done:
    buffer[i] = 0;

    /*
     * Parse the fields, which are: id, width, height, maxIntensity.
     */

    if (strncmp(buffer, "P6 ", 3) == 0) {
	type = PPM;
    } else if (strncmp(buffer, "P5 ", 3) == 0) {
	type = PGM;
    } else {
	return 0;
    }
    if (sscanf(buffer+3, "%d %d %d", widthPtr, heightPtr, maxIntensityPtr)
        != 3) {
	return 0;
    }
    return type;
}
