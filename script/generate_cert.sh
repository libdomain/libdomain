#!/bin/bash
set -euxo pipefail

CERT_PATH="${1:-/var/lib/samba/private/tls}"

# Generate a key for a CA using RSA with key length 4096 bits
openssl genrsa -out "${CERT_PATH}/ca.key" 4096
# Generate a root certificate based on a key for a period of 1 year. CN must be specified, it requires to be the domain's FQDN in uppercase.
openssl req -new -x509 -nodes -days 365 -key "${CERT_PATH}/ca.key" -out "${CERT_PATH}/ca.pem" -subj "/O=Test Inc/OU=Samba CA Cert/CN=domain.alt"
# Generate key
openssl genrsa -out "${CERT_PATH}/dc0.domain.alt.key" 4096
# Add CSR
openssl req -new -sha256 -key "${CERT_PATH}/dc0.domain.alt.key" -subj "/O=Test Inc/OU=Samba CA Cert/CN=dc0.domain.alt" -out "${CERT_PATH}/dc0.domain.alt.csr"
# And sign the CSR with the CA key and certificate 
openssl x509 -req -in "${CERT_PATH}/dc0.domain.alt.csr" -CA "${CERT_PATH}/ca.pem" -CAkey "${CERT_PATH}/ca.key" -CAcreateserial -out "${CERT_PATH}/dc0.domain.alt.pem" -days 365
# Validate certificate
openssl verify -CAfile "${CERT_PATH}/ca.pem" "${CERT_PATH}/dc0.domain.alt.pem"
