#!/bin/bash

# Start slapd
slapd -h "ldap://0.0.0.0:3890/" -f /tmp/slapd.conf

# Without this line our container will fail to keep running
tail -f /dev/null

