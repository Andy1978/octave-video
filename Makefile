## Copyright 2015-2016 Andreas Weber
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
	@echo "   maintainer-clean - Additionally remove all generated files"

$(RELEASE_DIR): .hg/dirstate
	@echo "Creating package version $(VERSION) release ..."
	-rm -rf $@
	hg archive --exclude ".hg*" --exclude "devel" --exclude Makefile --type files $@
	chmod -R a+rX,u+w,go-w $@

$(RELEASE_TARBALL): $(RELEASE_DIR)
	$(TAR) cf - --posix $< | gzip -9n > $@
	-rm -rf $<

$(HTML_DIR): install
	@echo "Generating HTML documentation. This may take a while ..."
	-rm -rf $@
	$(OCTAVE) --silent \
	  --eval 'graphics_toolkit ("gnuplot");' \
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
	cd src && $(MAKE) $@

check: all
	$(OCTAVE) --no-gui --silent \
	  --eval 'addpath (fullfile ([pwd filesep "src"]));' \
	  --eval 'runtests ("src");'

run: all
	$(OCTAVE) --silent --persist \
	  --eval 'addpath (fullfile ([pwd filesep "src"]));'

debug: clean
	cd src/ && ./configure
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

maintainer-clean: clean
	cd src && $(MAKE) $@

