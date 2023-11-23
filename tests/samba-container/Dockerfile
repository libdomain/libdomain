ARG DISTR

# Container image that runs your code
FROM $DISTR

ARG USER_ID
ARG GROUP_ID

RUN apt-get update \
    && apt-get install -y \
    bind-utils \
    krb5-kinit \
    ldb-tools \
    samba-dc-common \
    samba-dc \
    samba-dc-client \
    bind \
    bind-utils

# Copy provision script
COPY provision-samba.sh /provision-samba.sh

# Copy bind options
COPY options.conf /options.conf

# Copy entry point
COPY start-samba.sh /start-samba.sh

COPY krb5.conf /krb5.conf

COPY kdc.conf /etc/kdc.conf

COPY rndc.conf /etc/bind/rndc.conf

COPY resolv.conf /resolv.conf

COPY smb.conf /smb.conf

COPY create-test-data.sh /create-test-data.sh

EXPOSE 53 53/udp 88 88/udp 135 137-138/udp 139 389 389/udp 445 464 464/udp 636 3268-3269 49152-65535

CMD /provision-samba.sh && /start-samba.sh
