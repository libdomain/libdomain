#!/bin/bash

set -e

info () {
    echo "[INFO] $@"
}

info "Running provision"

# Check if samba is already been provisioned.
[ -f /var/lib/samba/.ready ] && info "Already provisioned. Exiting." && exit 0

rm /etc/samba/smb.conf

samba-tool domain provision\
 --server-role=dc\
 --use-rfc2307\
 --dns-backend=SAMBA_INTERNAL\
 --realm=domain\
 --domain=domain.alt\
 --adminpass=password145Qw!\
 --host-ip=`hostname -i`

mv /etc/samba/smb.conf /var/lib/samba/private/smb.conf

touch /var/lib/samba/.ready

