dist-hook: remove-mac-resource-forks
remove-mac-resource-forks:
	rm -f $(distdir)/._*

AM_CPPFLAGS =
ACLOCAL_AMFLAGS = -I m4

bin_SCRIPTS=wccf_tx_rest wccf_proc_null wccf_tx_rest.cfg wccf_proc_null.cfg wccf_util_purge.sh wccf_sensors.cron root cron.update
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST=wccf_tx_rest.in wccf_proc_null.in wccf_tx_rest.cfg.in wccf_proc_null.cfg.in wccf_util_purge.sh.in wccf_sensors.cron.in cron.update.in

wccf_util_purge.sh: wccf_util_purge.sh.in Makefile
	cp $(srcdir)/wccf_util_purge.sh.in wccf_util_purge.sh
	chmod +x wccf_util_purge.sh

wccf_tx_rest: wccf_tx_rest.in Makefile
	cp $(srcdir)/wccf_tx_rest.in wccf_tx_rest
	chmod +x wccf_tx_rest

wccf_proc_null: wccf_proc_null.in Makefile
	cp $(srcdir)/wccf_proc_null.in wccf_proc_null
	chmod +x wccf_proc_null

wccf_tx_rest.cfg: wccf_tx_rest.cfg.in Makefile
	cp $(srcdir)/wccf_tx_rest.cfg.in wccf_tx_rest.cfg
	chmod 644 wccf_tx_rest.cfg

wccf_proc_null.cfg: wccf_proc_null.cfg.in Makefile
	cp $(srcdir)/wccf_proc_null.cfg.in wccf_proc_null.cfg
	chmod 644 wccf_proc_null.cfg

wccf_sensors.cron: wccf_sensors.cron.in Makefile
	cp $(srcdir)/wccf_sensors.cron.in wccf_sensors.cron
	chmod 644 wccf_sensors.cron

root: wccf_sensors.cron.in Makefile
	cp $(srcdir)/wccf_sensors.cron.in root
	chmod 600 root

cron.update: cron.update.in Makefile
	cp $(srcdir)/cron.update.in cron.update
	chmod 600 cron.update

