#!/bin/sh

# Start slapd
slapd -h "ldap://127.0.0.1:3890/" -f slapd.conf &

# Wait for LDAP to start
sleep 1
