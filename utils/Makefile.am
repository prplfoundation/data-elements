dist-hook: remove-mac-resource-forks
remove-mac-resource-forks:
	rm -f $(distdir)/._*

AM_CPPFLAGS =
ACLOCAL_AMFLAGS = -I m4

bin_SCRIPTS=wccf_sensor_event wccf_sensor_event.cfg wccf_tx_rest wccf_proc_multiap wccf_tx_rest.cfg wccf_proc_multiap.cfg wccf_util_purge.sh wccf_sensors.cron root wccf_openwrt_pubkey wccf_openwrt_privkey wccf_config_sync.sh wccf_config_sync.cfg wccf_util_update.sh wccf_util_update_worker.sh
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST=wccf_sensor_event.in wccf_sensor_event.cfg.in wccf_tx_rest.in wccf_proc_multiap.in wccf_tx_rest.cfg.in wccf_proc_multiap.cfg.in wccf_util_purge.sh.in wccf_sensors.cron.in wccf_openwrt_pubkey.in wccf_openwrt_privkey.in wccf_config_sync.sh.in wccf_config_sync.cfg.in wccf_util_update.sh.in wccf_util_update_worker.sh.in

wccf_util_purge.sh: wccf_util_purge.sh.in Makefile
	cp $(srcdir)/wccf_util_purge.sh.in wccf_util_purge.sh
	chmod +x wccf_util_purge.sh

wccf_util_update.sh: wccf_util_update.sh.in Makefile
	cp $(srcdir)/wccf_util_update.sh.in wccf_util_update.sh
	chmod +x wccf_util_update.sh

wccf_util_update_worker.sh: wccf_util_update_worker.sh.in Makefile
	cp $(srcdir)/wccf_util_update_worker.sh.in wccf_util_update_worker.sh
	chmod +x wccf_util_update_worker.sh

wccf_sensor_event: wccf_sensor_event.in Makefile
	cp $(srcdir)/wccf_sensor_event.in wccf_sensor_event
	chmod +x wccf_sensor_event

wccf_tx_rest: wccf_tx_rest.in Makefile
	cp $(srcdir)/wccf_tx_rest.in wccf_tx_rest
	chmod +x wccf_tx_rest

#wccf_proc_null: wccf_proc_null.in Makefile
#	cp $(srcdir)/wccf_proc_null.in wccf_proc_null
#	chmod +x wccf_proc_null
#
wccf_proc_multiap: wccf_proc_multiap.in Makefile
	cp $(srcdir)/wccf_proc_multiap.in wccf_proc_multiap
	chmod +x wccf_proc_multiap

wccf_tx_rest.cfg: wccf_tx_rest.cfg.in Makefile
	cp $(srcdir)/wccf_tx_rest.cfg.in wccf_tx_rest.cfg
	chmod 644 wccf_tx_rest.cfg

#wccf_proc_null.cfg: wccf_proc_null.cfg.in Makefile
#	cp $(srcdir)/wccf_proc_null.cfg.in wccf_proc_null.cfg
#	chmod 644 wccf_proc_null.cfg
#
wccf_proc_multiap.cfg: wccf_proc_multiap.cfg.in Makefile
	cp $(srcdir)/wccf_proc_multiap.cfg.in wccf_proc_multiap.cfg
	chmod 644 wccf_proc_multiap.cfg

wccf_sensors.cron: wccf_sensors.cron.in Makefile
	cp $(srcdir)/wccf_sensors.cron.in wccf_sensors.cron
	chmod 644 wccf_sensors.cron

root: wccf_sensors.cron.in Makefile
	cp $(srcdir)/wccf_sensors.cron.in root
	chmod 600 root

wccf_openwrt_pubkey: wccf_openwrt_pubkey.in Makefile
	cp $(srcdir)/wccf_openwrt_pubkey.in wccf_openwrt_pubkey
	chmod 644 wccf_openwrt_pubkey

wccf_openwrt_privkey: wccf_openwrt_privkey.in Makefile
	cp $(srcdir)/wccf_openwrt_privkey.in wccf_openwrt_privkey
	chmod 644 wccf_openwrt_privkey

wccf_config_sync.sh: wccf_config_sync.sh.in Makefile
	cp $(srcdir)/wccf_config_sync.sh.in wccf_config_sync.sh
	chmod 644 wccf_config_sync.sh

wccf_config_sync.cfg: wccf_config_sync.cfg.in Makefile
	cp $(srcdir)/wccf_config_sync.cfg.in wccf_config_sync.cfg
	chmod 644 wccf_config_sync.cfg

wccf_sensor_event.cfg: wccf_sensor_event.cfg.in Makefile
	cp $(srcdir)/wccf_sensor_event.cfg.in wccf_sensor_event.cfg
	chmod 644 wccf_sensor_event.cfg

