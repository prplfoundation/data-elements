AM_MAKEFLAGS = COPYFILE_DISABLE='$(COPYFILE_DISABLE)'

dist-hook: remove-mac-resource-forks tag-distribution-date
remove-mac-resource-forks: 
	rm -f $(distdir)/._*

tag-distribution-date:
	echo "Distribution date: `date`" >> README

SUBDIRS = sensors utils processors transmitters

