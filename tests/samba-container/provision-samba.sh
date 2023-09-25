#!/bin/bash

set -e

print_message () {
    echo "[INFO] $@"
}

print_message "Running provision"

# Check if samba is already been provisioned.
[ -f /var/lib/samba/.ready ] && print_message "Already provisioned. Exiting." && exit 0

rm /etc/samba/smb.conf

samba-tool domain provision\
 --server-role=dc\
 --use-rfc2307\
 --dns-backend=SAMBA_INTERNAL\
 --realm=DOMAIN\
 --domain=DOMAIN.ALT\
 --adminpass=password145Qw!\
 --host-ip=`hostname -i`

mv /etc/samba/smb.conf /var/lib/samba/private/smb.conf

mv /smb.conf /etc/samba/smb.conf

mv /krb5.conf /etc/krb5.conf

touch /var/lib/samba/.ready

