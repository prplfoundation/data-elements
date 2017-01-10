dist-hook: remove-mac-resource-forks include-files
remove-mac-resource-forks:
	rm -f $(distdir)/._*

include-files: remove-mac-resource-forks
	cp *.h $(distdir)

AM_CPPFLAGS =  
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = wccf_sensor_station wccf_sensor_scan wccf_sensor_survey
wccf_sensor_station_SOURCES = wccf_sensor_station.c
wccf_sensor_station_CFLAGS = -I ./
wccf_sensor_station_CPPFLAGS = -I ./
wccf_sensor_station_LDFLAGS = -liw -lnl-tiny -ljson-c -lc
wccf_sensor_scan_SOURCES = wccf_sensor_scan.c
wccf_sensor_scan_CFLAGS = -I ./
wccf_sensor_scan_CPPFLAGS = -I ./
wccf_sensor_scan_LDFLAGS = -liw -lnl-tiny -ljson-c -liwinfo -lc
wccf_sensor_survey_SOURCES = wccf_sensor_survey.c
wccf_sensor_survey_CFLAGS = -I ./
wccf_sensor_survey_CPPFLAGS = -I ./
wccf_sensor_survey_LDFLAGS = -liw -lnl-tiny -ljson-c -lc

