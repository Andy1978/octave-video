## Copyright 2015-2019 Andreas Weber
## Copyright 2015-2016 Mike Miller
## Copyright 2015-2016 Carnë Draug
## Copyright 2015-2016 Oliver Heimlich
##
## Copying and distribution of this file, with or without modification,
## are permitted in any medium without royalty provided the copyright
## notice and this notice are preserved.  This file is offered as-is,
## without any warranty.

## Makefile to simplify Octave Forge package maintenance tasks

PACKAGE = $(shell $(SED) -n -e 's/^Name: *\(\w\+\)/\1/p' DESCRIPTION | $(TOLOWER))
VERSION = $(shell $(SED) -n -e 's/^Version: *\(\w\+\)/\1/p' DESCRIPTION | $(TOLOWER))

RELEASE_DIR     = $(PACKAGE)-$(VERSION)
RELEASE_TARBALL = $(PACKAGE)-$(VERSION).tar.gz
HTML_DIR        = $(PACKAGE)-html
HTML_TARBALL    = $(PACKAGE)-html.tar.gz

M_SOURCES   = $(wildcard inst/*.m)
CC_SOURCES  = src/cap_ffmpeg_wrapper.cc
#OCT_FILES   = $(patsubst %.cc,%.oct,$(CC_SOURCES))
PKG_ADD     = $(shell grep -Pho '(?<=// PKG_ADD: ).*' $(CC_SOURCES) $(M_SOURCES))

MD5SUM    ?= md5sum
MKOCTFILE ?= mkoctfile
OCTAVE    ?= octave
SED       ?= sed
TAR       ?= tar

TOLOWER = $(SED) -e 'y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/'

.PHONY: help dist html release install all check run doc debug clean maintainer-clean

help:
	@echo "Targets:"
	@echo "   dist             - Create $(RELEASE_TARBALL) for release"
	@echo "   html             - Create $(HTML_TARBALL) for release"
	@echo "   release          - Create both of the above and show md5sums"
	@echo
	@echo "   install          - Install the package in GNU Octave"
	@echo "   all              - Build all oct files"
	@echo "   check            - Execute package tests (w/o install)"
	@echo "   run              - Run Octave with development in PATH (no install)"
	@echo "   debug            - Build with debug flags, run and attach gdb (w/o install)"
	@echo "   doc              - Build Texinfo package manual"
	@echo
	@echo "   clean            - Remove releases, html documentation, and oct files"
	@echo "   realclean        - Additionally remove all generated files"

$(RELEASE_DIR): all
	@echo "Creating package version $(VERSION) release ..."
	-rm -rf $@
	mkdir -p $@/src
	cp COPYING DESCRIPTION NEWS $@
	cp -r ./inst $@
	cp ./src/configure ./src/configure.ac ./src/bootstrap ./src/Makefile.in ./src/cap_ffmpeg_impl_ov.hpp ./src/cap_ffmpeg_wrapper.cc ./src/ffmpeg_codecs.hpp $@/src
	sed -i '/###/q' $@/src/Makefile.in
	chmod -R a+rX,u+w,go-w $@

$(RELEASE_TARBALL): $(RELEASE_DIR)
	$(TAR) cf - --posix $< | gzip -9n > $@
	-rm -rf $<

$(HTML_DIR): install
	@echo "Generating HTML documentation. This may take a while ..."
	-rm -rf $@
	$(OCTAVE) --silent \
	  --eval 'graphics_toolkit ("fltk");' \
	  --eval 'pkg load generate_html $(PACKAGE);' \
	  --eval 'generate_package_html ("$(PACKAGE)", "$@", "octave-forge");'
	chmod -R a+rX,u+w,go-w $@

$(HTML_TARBALL): $(HTML_DIR)
	$(TAR) cf - --posix $< | gzip -9n > $@
	-rm -rf $<

dist: $(RELEASE_TARBALL)

html: $(HTML_TARBALL)

release: dist html
	@$(MD5SUM) $(RELEASE_TARBALL) $(HTML_TARBALL)
	@echo "Upload @ https://sourceforge.net/p/octave/package-releases/new/"
	@echo "Execute: hg tag \"$(VERSION)\""

install: $(RELEASE_TARBALL)
	@echo "Installing package locally ..."
	$(OCTAVE) --silent --eval 'pkg install $(RELEASE_TARBALL);'

all:
	cd src && ./bootstrap && $(MAKE) $@

check: all
	$(OCTAVE) --silent \
	  --eval 'addpath (fullfile (pwd, "src"));' \
	  --eval 'addpath (fullfile (pwd, "inst"));' \
	  --eval '${PKG_ADD}' \
	  --eval 'runtests ("inst");'

run: all
	$(OCTAVE) --silent --persist \
	  --eval 'addpath (fullfile (pwd, "src"));' \
	  --eval 'addpath (fullfile (pwd, "inst"));' \
	  --eval '${PKG_ADD}'

debug: clean
	cd src/ && ./bootstrap && ./configure
	$(MAKE) -C src/ debug
	$(OCTAVE) --no-gui --silent --persist \
	  --eval 'addpath (fullfile ([pwd filesep "src"]));' \
	  --eval 'system (sprintf ("xfce4-terminal --command \"gdb -p %d\"", getpid ()), "async");'

style:
	find ./src \( -name "*.cc" -or -name "*.h" -or -name "*.m" \) -exec sed -i 's/[[:space:]]*$$//' {} \;
	find ./src \( -name "*.cc" -or -name "*.h" \) -exec astyle --style=gnu -s2 -n {} \;

doc:

clean:
	-rm -rf $(RELEASE_DIR) $(RELEASE_TARBALL) $(HTML_TARBALL) $(HTML_DIR)
	cd src && $(MAKE) $@

realclean: clean
	cd src && $(MAKE) $@

