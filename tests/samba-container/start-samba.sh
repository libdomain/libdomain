#!/bin/bash

mkdir /tmp/samba

if [ $? -ne 0 ]; then
    echo "Error while configuring slapd service!"
    cat /tmp/slapd.log
    exit 1
fi

# Without this line our container will fail to keep running
tail -f /dev/null
