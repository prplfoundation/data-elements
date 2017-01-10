dist-hook: remove-mac-resource-forks include-files
remove-mac-resource-forks:
	rm -f $(distdir)/._*

include-files: remove-mac-resource-forks
	cp *.h $(distdir)

AM_CPPFLAGS =  
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = wccf_tx_rest
wccf_tx_rest_SOURCES = wccf_tx_rest.c
wccf_tx_rest_CFLAGS = -I ./
wccf_tx_rest_CPPFLAGS = -I ./
