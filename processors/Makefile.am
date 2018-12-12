dist-hook: remove-mac-resource-forks include-files
remove-mac-resource-forks:
	rm -f $(distdir)/._*

include-files: remove-mac-resource-forks
	cp *.h $(distdir)
	cp -R ./rapidjson $(distdir)

AM_CPPFLAGS =  
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = wccf_proc_null wccf_proc_multiap 
wccf_proc_null_SOURCES = wccf_proc_null.c
wccf_proc_null_CFLAGS = -I ./
wccf_proc_null_CPPFLAGS = -I ./
wccf_proc_null_LDFLAGS = -liw -lnl-tiny -ljson-c -lc
wccf_proc_multiap_SOURCES = wccf_proc_multiap.cpp WCCFProcMultiAP.cpp
wccf_proc_multiap_CFLAGS = -I ./
wccf_proc_multiap_CPPFLAGS = -I ./
wccf_proc_multiap_LDFLAGS = -liw -lnl-tiny -lc

