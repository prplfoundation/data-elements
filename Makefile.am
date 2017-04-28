AM_MAKEFLAGS = COPYFILE_DISABLE='$(COPYFILE_DISABLE)'

dist-hook: remove-mac-resource-forks tag-distribution-date
remove-mac-resource-forks: 
	rm -f $(distdir)/._*

tag-distribution-date:
	echo "Distribution date: `date`" >> README

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = sensors utils processors transmitters

bin_SCRIPTS=Makefile
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST=targets/openwrt/Makefile.template targets/openwrt/patches/*

Makefile: targets/openwrt/Makefile.template Makefile
	cp $(srcdir)/targets/openwrt/Makefile.template Makefile
	cp -r $(srcdir)/targets/openwrt/patches patches
	chmod 644 Makefile
	chmod -R 644 patches

