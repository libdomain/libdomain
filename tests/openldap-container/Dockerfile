ARG DISTR

# Container image that runs your code
FROM $DISTR

ARG USER_ID
ARG GROUP_ID

RUN apt-get update \
    && apt-get install -y \
    openldap-servers

# Copies your code file from your action repository to the filesystem path `/` of the container
COPY ../script/start-ldap.sh /start-ldap.sh

# Code file to execute when the docker container starts up (`build.sh`)
ENTRYPOINT ["/start-ldap.sh"]
