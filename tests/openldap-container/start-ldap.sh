#!/bin/bash

# Start slapd
slapd -h "ldap://0.0.0.0:3890/" -f /tmp/slapd.conf

# Without this line our container will fail to keep runing
# tail -f /dev/null

