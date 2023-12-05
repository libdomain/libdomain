ARG DISTR

# Container image that runs your code
FROM $DISTR

ARG USER_ID
ARG GROUP_ID

RUN apt-get update \
    && apt-get install -y git \
    gear \
    hasher \
    hasher-priv \
    hasher-rich-chroot \
    hasher-rich-chroot-user-utils \
    rpm-utils \
    rpm-build \
    rpm-build-licenses \
    rpm-macros-cmake \
    rpm-macros-make \
    rpm-macros-generic-compat \
    apt-repo \
    apt-repo-tools \
    cmake \
    ctest \
    rpm-macros-cmake \
    cmake-modules \
    gcc \
    gcc-c++ \
    doxygen \
    sudo \
    libldap-devel \
    libverto-devel \
    libverto-libev-devel \
    libverto-glib-devel \
    libverto-libevent-devel \
    libtalloc-devel \
    libsasl2-devel \
    libconfig-devel \
    cgreen \
    krb5-kinit \
    cyrus-sasl2 \
    libsasl2-plugin-gssapi \
    glib2-devel \
    ragel \
    && export CURRENT_PWD=`pwd` \
    && groupadd builder2 \
    && useradd --uid $USER_ID --gid builder2 -ms /bin/bash builder2 \
    && groupadd sudo \
    && usermod -aG rpm builder2 \
    && usermod -aG sudo root \
    && usermod -aG sudo builder2 \
    && echo "root ALL=(ALL) NOPASSWD:ALL" | tee -a /etc/sudoers \
    && echo "builder2 ALL=(ALL) NOPASSWD:ALL" | tee -a /etc/sudoers \
    && mkdir /app \
    && chown root:builder2 /app

USER root

# Copy entry point
COPY make-test.sh /make-test.sh

COPY resolv.conf /resolv.conf

COPY krb5.conf /krb5.conf

COPY config.ini /etc/config.ini

# Code file to execute when the docker container starts up (`make-test.sh`)
ENTRYPOINT ["/make-test.sh"]
