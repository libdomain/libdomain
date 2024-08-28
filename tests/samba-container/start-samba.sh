#!/bin/bash

mkdir /tmp/samba

samba --foreground  --no-process-group --debug-stdout &

if [ $? -ne 0 ]; then
    echo "Error while configuring samba service!"
    cat /var/log/samba/*.log
    exit 1
fi

/bin/bash /create-test-data.sh

# Without this line our container will fail to keep running
tail -f /dev/null
