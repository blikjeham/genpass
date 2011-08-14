# Makefile.am
#
# For genpass manpages.
#
# $Id: Makefile.am,v 1.4 2010/11/19 20:54:43 jabik Exp $

dist_man_MANS = genpass.1 genpass.conf.5

all-local:
	-sed -i -e 's|CONFDIR|$(sysconfdir)|g' genpass.*

clean-local:
	-sed -i -e 's|$(sysconfdir)|CONFDIR|g' genpass.*
