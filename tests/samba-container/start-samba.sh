#!/bin/bash

mkdir /tmp/samba

if [ $? -ne 0 ]; then
    echo "Error while configuring samba service!"
    cat /var/log/samba/*.log
    exit 1
fi

# Without this line our container will fail to keep running
tail -f /dev/null
