#!/bin/bash

mkdir /tmp/ldap

# Start slapd
slapd -d any -h "ldap://0.0.0.0:3890/ ldaps://0.0.0.0:6360" -f /tmp/slapd.conf 2>&1 > /tmp/slapd.log &

# Populating database
i=0
while [ $i -le 15 ]
do
    if ldapadd -x -f /tmp/domain.alt.ldif -H ldap://127.0.0.1:3890 -D "cn=admin,dc=domain,dc=alt" -w password ; then
        break
    else
        sleep 2
        i=$(( $i + 1 ))
    fi
done

if [ $? -ne 0 ]; then
    echo "Error while configuring slapd service!"
    cat /tmp/slapd.log
    exit 1
fi

# Without this line our container will fail to keep running
tail -f /dev/null

