
# Configuration for cc.
#
# @(#)config-cc.mk	1.1	06/13/98

LOCALCCFLAGS =	-g
LOCALCFLAGS = 	$(LOCALCCFLAGS)
GPPFLAGS =	$(OPTIMIZER) $(LOCALCCFLAGS) $(USERFLAGS)
CFLAGS =	$(OPTIMIZER) $(LOCALCFLAGS) $(USERFLAGS)
