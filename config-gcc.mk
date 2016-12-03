
# Config to use for gcc.
# This is not a complete config; it only overrides those options
# specific to using g++
#
# @(#)config-gcc.mk	1.1	06/13/98

# -Wsynth is new in g++-2.6.x
# Under gcc-2.7.0, you will need to add -fno-for-scope to LOCALCCFLAGS
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
GCC_270_OPTIONS = -fno-for-scope
WARNINGS =	-Wall -Wsynth 
LOCALCCFLAGS =	-g $(GCC_270_OPTIONS)
LOCALCFLAGS = 	$(LOCALCCFLAGS)
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)
LINKFLAGS =	-static

