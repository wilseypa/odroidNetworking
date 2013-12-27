#!/bin/bash
# Usage: ofed_checkout.sh [branch]
#        must be launched from top of git repo

# get ref to "real" stdout
exec 3>&1

# Execute command w/ echo and exit if it fails
ex()
{
        echo "$@" >&3
        eval "$@"
        if [ $? -ne 0 ]; then
                printf "\nFailed executing $@\n" >&3
                exit 1
        fi
}

# Like ex above, but command is self echoing on stderr
xex()
{
        eval "$@" 2>&3
        if [ $? -ne 0 ]; then
                printf "\nFailed executing $@\n" >&3
                exit 1
        fi
}

# branch defaults to ofed_kernel
branch=${1:-ofed_kernel}

#checkout nfs headerfiles and put them in their new location
if [ ! -e ofed_scripts/nfs_header_checkout_files ]; then
	ex git checkout ${branch} ofed_scripts/nfs_header_checkout_files
fi

xex xargs -rt git ls-tree -r --name-only ${branch} <ofed_scripts/nfs_header_checkout_files |
	xex xargs -rt git checkout ${branch}

ex mv include include-nfs

# make sure we have the list of all the other files and dirs
if [ ! -e ofed_scripts/checkout_files ]; then
	ex git checkout ${branch} ofed_scripts/checkout_files
fi

xex xargs -rt git ls-tree -r --name-only ${branch} <ofed_scripts/checkout_files |
	xex xargs -rt git checkout ${branch}

ex git update-ref HEAD ${branch}

ln -snf ofed_scripts/configure
ln -snf ofed_scripts/Makefile
ln -snf ofed_scripts/makefile
(cd drivers/scsi/; rm -f Makefile; cp ../../ofed_scripts/iscsi_scsi_makefile Makefile)
