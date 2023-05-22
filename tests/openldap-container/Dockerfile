ARG DISTR

# Container image that runs your code
FROM $DISTR

ARG USER_ID
ARG GROUP_ID

RUN apt-get update \
    && apt-get install -y \
    openldap-servers \
    openldap-clients

# Copy entry point
COPY start-ldap.sh /start-ldap.sh

# Copy configuration files
COPY slapd.conf /tmp/slapd.conf
COPY domain.alt.ldif /tmp/domain.alt.ldif

# Code file to execute when the docker container starts up (`start-ldap.sh`)
ENTRYPOINT ["/start-ldap.sh"]
