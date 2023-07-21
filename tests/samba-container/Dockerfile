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

# Copy entry point
COPY start-samba.sh /start-samba.sh

ENTRYPOINT [ "/start-samba.sh" ]
