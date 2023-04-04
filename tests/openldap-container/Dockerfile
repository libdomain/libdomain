ARG DISTR

# Container image that runs your code
FROM $DISTR

ARG USER_ID
ARG GROUP_ID

RUN apt-get update \
    && apt-get install -y \
    openldap-servers

# Copy entry point
COPY start-ldap.sh /build.sh

# Code file to execute when the docker container starts up (`start-ldap.sh`)
ENTRYPOINT ["/start-ldap.sh"]
