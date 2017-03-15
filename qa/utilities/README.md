#QA Utilities README#

##Build Script Utilities##

These scripts - 

    build_ap_image.sh
    build_openwrt.sh
    build_wccf.sh
    full_build.sh
    README.md

These helper files -

    NETGEAR.config
    TP-LINK.config


offer a convenient way to build the WCCF package and incorporate
it swiftly into an OpenWrt image build.

The scripts require two env vars to be set before execution that
each point to one of the Git local repositories: (a) the WCCF local
repository, and (b) the OpenWrt local repository.  

These respositories must be previously prepared in order for these
scripts to function properly.  Details on preparing the local
repositories appear in the [wccf/doc/HOWTO_*](../../doc) documents.

A test is made in each script to see that the env vars are set,
and an error is produced if a problem is found. See sample export
commands below.

UPDATE: 

Script full_build.sh is the newest and simplest way to build.  It
utilizes the helper files (above) to avoid the OpenWrt image build
options menus completely.  Full build takes a cmd line arg to set
the desired AP type (NETGEAR or TP-LINK).  See internal comments.

Script build_image.sh is used to call, in order, each of the other
two scripts to fully automate the rebuild step.

Note that build_openwrt.sh always executes step 'make menuconfig'
so some keyboard input is required to complete and/or verify the
build settings.  However, once done, that likely will not change
across rebuilds (for a given AP device). Alternatively, that
build step (make menuconfig) could be made optional or removed.

Briefly review comments in the headers, and throughout, each script.

From build_image.sh:

    # Required env vars (sample settings):
    #
    #    export WCCF_BUILD_ROOT=$HOME/wccf_git_clone/wccf
    #    export OPENWRT_BUILD_ROOT=$HOME/openwrt_git_clone/openwrt
