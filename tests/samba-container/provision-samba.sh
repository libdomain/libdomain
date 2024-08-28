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
 --realm=DOMAIN.ALT\
 --domain=DOMAIN\
 --adminpass=password145Qw!\
 --host-ip=`hostname -i`

control bind-chroot disabled

mv /krb5.conf /etc/krb5.conf

cat /resolv.conf > /etc/resolv.conf

mv /smb.conf /etc/samba/smb.conf

chown root:root -R /certs

chmod 600 /certs/*

echo 'dc0' > /etc/hostname

update_chrooted all || :

touch /var/lib/samba/.ready

