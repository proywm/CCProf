##
##  Libmonitor top-level Makefile.am.
##
##  Copyright (c) 2007-2013, Rice University.
##  All rights reserved.
##
##  Redistribution and use in source and binary forms, with or without
##  modification, are permitted provided that the following conditions are
##  met:
##
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
##
##  * Neither the name of Rice University (RICE) nor the names of its
##    contributors may be used to endorse or promote products derived from
##    this software without specific prior written permission.
##
##  This software is provided by RICE and contributors "as is" and any
##  express or implied warranties, including, but not limited to, the
##  implied warranties of merchantability and fitness for a particular
##  purpose are disclaimed. In no event shall RICE or contributors be
##  liable for any direct, indirect, incidental, special, exemplary, or
##  consequential damages (including, but not limited to, procurement of
##  substitute goods or services; loss of use, data, or profits; or
##  business interruption) however caused and on any theory of liability,
##  whether in contract, strict liability, or tort (including negligence
##  or otherwise) arising in any way out of the use of this software, even
##  if advised of the possibility of such damage.
##
##  $Id: Makefile.am 145 2013-01-02 20:34:01Z krentel $
##

SUBDIRS = src

SVN_REVISION = @svn_revision@

#
# Our custom dist target adds the svn rev number to the tar file and
# its directory.  None of this would be necessary if Automake just let
# us compute a value for PACKAGE_VERSION instead of requiring a fixed
# literal.
#
MY_DIST_FILES = LICENSE Makefile.am Makefile.in README  \
	aclocal.m4 config.h.in configure configure.ac .svn-revision  \
	src/Makefile.am src/*.in src/*.h src/*.c  \
	config tests/Makefile tests/*.c

MY_DIST_DIR = $(PACKAGE_NAME)-$(PACKAGE_VERSION)-r$(SVN_REVISION)
MY_TAR_FILE = $(MY_DIST_DIR).tar

dist:
	rm -f "$(MY_DIST_DIR)" "$(MY_TAR_FILE)" "$(MY_TAR_FILE).gz"
	ln -s "$(srcdir)" "$(MY_DIST_DIR)"
	tar cf "$(MY_TAR_FILE)"  \
		--exclude .svn  \
		$(addprefix $(MY_DIST_DIR)/,$(MY_DIST_FILES))
	rm -f "$(MY_DIST_DIR)"
	gzip "$(MY_TAR_FILE)"
	@ls -l "$(MY_TAR_FILE).gz"

.PHONY: dist

