dist-hook: remove-mac-resource-forks include-files
remove-mac-resource-forks:
	rm -f $(distdir)/._*

include-files: remove-mac-resource-forks
	cp *.h $(distdir)

AM_CPPFLAGS =  
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = wccf_sensor_station wccf_sensor_scan wccf_sensor_channel wccf_sensor_interface wccf_sensor_phycapa wccf_sensor_event
wccf_sensor_station_SOURCES = wccf_sensor_station.c
wccf_sensor_station_CFLAGS = -I ./
wccf_sensor_station_CPPFLAGS = -I ./
wccf_sensor_station_LDFLAGS = -liw -lnl-tiny -ljson-c -lc
wccf_sensor_scan_SOURCES = wccf_sensor_scan.c
wccf_sensor_scan_CFLAGS = -I ./
wccf_sensor_scan_CPPFLAGS = -I ./
wccf_sensor_scan_LDFLAGS = -liw -lnl-tiny -ljson-c -liwinfo -lc
wccf_sensor_channel_SOURCES = wccf_sensor_channel.c
wccf_sensor_channel_CFLAGS = -I ./
wccf_sensor_channel_CPPFLAGS = -I ./
wccf_sensor_channel_LDFLAGS = -liw -lnl-tiny -ljson-c -lc
wccf_sensor_interface_SOURCES = wccf_sensor_interface.c
wccf_sensor_interface_CFLAGS = -I ./
wccf_sensor_interface_CPPFLAGS = -I ./
wccf_sensor_interface_LDFLAGS = -liw -lnl-tiny -ljson-c -lc
wccf_sensor_phycapa_SOURCES = wccf_sensor_phycapa.c
wccf_sensor_phycapa_CFLAGS = -I ./
wccf_sensor_phycapa_CPPFLAGS = -I ./
wccf_sensor_phycapa_LDFLAGS = -liw -lnl-tiny -ljson-c -lc
wccf_sensor_event_SOURCES = wccf_sensor_event.c
wccf_sensor_event_CFLAGS = -I ./
wccf_sensor_event_CPPFLAGS = -I ./
wccf_sensor_event_LDFLAGS = -liw -lnl-tiny -ljson-c -lc
