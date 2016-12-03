
      Two Fast, High-Quality, Algorithms for Inverse Halftoning

    Niranjan Damera-Venkata, Thomas D. Kite, and Brian L. Evans

             Laboratory for Image and Video Engineering
         Department of Electrical and Computer Engineering
      The University of Texas at Austin, Austin, TX 78712-1084

                   http://anchovy.ece.utexas.edu

            {damera,tom,bevans}@vision.ece.utexas.edu

                              06/21/98


1.0 Halftoning and Inverse Halftoning

Since the advent of the printing press, it has been desirable to
reproduce grayscale (multi-bit) imagery on inherently binary
(one-bit) media.  Simple truncation of the grayscale image gives
visually unacceptable results; instead, a specialized binarization
procedure must be used that attempts to preserve image features and
gray levels.  This process is known in the printing industry as
halftoning.  By judiciously applying dots to the paper in patterns of
varying density, it is possible to achieve the illusion of grayscale.
These varying densities are seen by the eye as different shades of gray.

Inverse halftoning algorithms recover grayscale images from halftones.
This is useful when a halftone is the only available version of an image,
and enhancement, compression, or some other manipulation of the image
is required.  Apart from very simple operations, such as cropping and
rotation through multiples of 90 degrees, halftones cannot be manipulated
without causing severe image degradation.  They are also difficult to
compress, either losslessly or lossily; grayscale images, on the other
hand, can be compressed efficiently.  The ability to generate an inverse
halftone allows a wide range of operations to be performed.


2.0 Fast Inverse Halftoning Algorithm I

The first fast inverse halftoning algorithm applies to a wide variety
of halftones.  The algorithm is explained in the following paper:
 
     N. Damera-Venkata, T. D. Kite, M. Venkataraman, and B. L. Evans,
       ``Fast Blind Inverse Halftoning,'' Proc. IEEE Int. Conf. on
       Image Processing, Oct. 4-7, 1998.
       http://www.ece.utexas.edu/~bevans/papers/1998/inverse_halftoning/

We have implemented the algorithm in the program fastiht1.c.  The
algorithm supports error diffused and ordered dithered halftones.

You can compile the fastiht1 program on Unix by

     make fastiht1

The fastiht1.c program depends on the functions in the files 
image_io.c, inverse_halftone.c, and matrix_utils.c.  Pre-built
binary versions of fastiht1 exist for Windows '95/NT under the
bin.nt4 directory and the Solaris 2.5 operating system for a
Sun Workstation in the bin.sol2.5 directory.  You can build the
code using the GNU C compiler by executing the following commands:

     gcc -O3 -c fastiht1.c
     gcc -O3 -c image_io.c
     gcc -O3 -c inverse_halftone.c
     gcc -O3 -c matrix_utils.c
     gcc -O3 -c readWritePPM.c
     gcc -o fastiht1 fastiht1.o image_io.o inverse_halftone.o \
	    matrix_utils.o readWritePPM.o

Instead of 'gcc', you can use 'cc' to invoke the native C compiler
on your Unix platform.

Once you have compiled the 'fastiht1' program, you can run it by
executing

     ./fastiht1 lena_halftone_512x512 test1 0 4 1 
 
or

     ./fastiht1 lena_halftone.pgm test1.pgm 0 4 1 

The usage information follows:

     halfFile inverseFile threshold gain halfType [rows] [columns]

Note that halfType is 1 for error diffusion, 2 for dispersed dither,
and 3 for clustered dither.  For raw images, the number of rows and
number of columns default to 512.  The threshold and gain parameters
are integers: the threshold is typically 0, 1, 2, 3, or 4, and the
gain is 4, 5, 6, 7, or 8.  Both the 'lena_halftone_512x512' and
'lena_halftone.pgm' files contain the error diffused halftone version
of the 512 x 512 Lena image in the 'lena_512x512' file.  The fastiht1
algorithm by default prints the execution time of the algorithm not
counting the time to read and write the images from disk.  (You can
change the behavior by changing the value of TIME_EXECUTION_FLAG in
the fastiht1.c file to 0 and then rebuild the fastiht1 program.)  The
timing may only be accurate to one second.  We ran the algorithm for
100 iterations on a 167 MHz Ultra-2 workstation and found that the
algorithm took 2.4 seconds on average to process the 512 x 512 halftone
'lena_halftone_512x512'.  The result of running this algorithm on the
'lena_halftone_512x512' halftone is stored in 'lena_1_invhalf_512x512'.


3.0 Fast Inverse Halftoning Algorithm II

The second fast inverse halftoning algorithm applies only to error
diffused halftones.  The second algorithm is explained in the
following paper:
 
    T. D. Kite, N. Damera-Venkata, B. L. Evans, and A. C. Bovik,
      ``A High Quality, Fast Inverse Halftoning Algorithm for Error
      Diffused Halftones,'' Proc. IEEE Int. Conf. on Image
      Processing, Oct. 4-7, 1998, to appear.
      http://www.ece.utexas.edu/~bevans/papers/1998/error_diffusion/

We have implemented the algorithm in the program fastiht2.c.  You
can compile this program on Unix by

     make fastiht2

The fastiht2.c program depends on functions in only one file in this
directory: readWritePPM.c.  As an alternative to running make, you
can compile the fastiht2 program using

     gcc -O3 -c readWritePPM.c
     gcc -O3 -c fastiht2.c
     gcc -o fastiht2 fastiht2.o readWritePPM.o
 
or an equivalent C compiler such as 'cc'.  Pre-built binary versions
of fastiht2 exist for Windows '95/NT machines under the bin.nt4
directory and for Solaris 2.5 operating system for a Sun Workstation
in the bin.sol2.5 directory.

You can run the fastiht2 program using
 
     ./fastiht2 lena_halftone_512x512 test2 512 512
 
The arguments are

     halftoneFile greyImageFile [xsize] [ysize]

where the input file and output file are either raw 8-bit grayscale
images of size xsize by ysize (default size is 512 x 512) or they
are portable graymap (PGM) files.  The input file is of course binary,
and consists of 0 for black and any non-zero integer for white.  With
the distribution, we have provided the files 'lena_halftone_512x512'
and 'lena_halfone.pgm' which are both versions of the error diffused
halftone of the file 'lena_512x512'.  The result of running this
algorithm on the 'lena_halftone_512x512' halftone is stored in
'lena_2_invhalf_512x512'.


4.0  Future Releases

We plan future releases.  Right now, fast algorithm I is implemented
using floating-point operations when in fact all of the operations
can be implemented using integer arithmetic.  By converting all
arithmetic operations to integer, we expect a 3:1 speedup.  Tom Kite
has spent significant time hand optimizing the C code for fast
algorithm II.  At this time, we do not expect any substantial changes
to the implementation of fast algorithm II in future releases.
