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

RUN git clone https://github.com/august-alt/cgreen/ \
    && cd cgreen \
    && git checkout alt_build \
    && chown builder2:builder2 /cgreen

USER builder2

RUN git config --global --add safe.directory /cgreen \
    && cd cgreen && export CC=`which gcc` && export CXX=`which g++` && echo $CC $CXX && gear-rpm -ba

USER root

RUN apt-get install /home/builder2/RPM/RPMS/x86_64/cgreen-1.6.2-alt1.x86_64.rpm && rm -rf /cgreen && rm -rf /home/builder2/RPM/

# Copy entry point
COPY make-test.sh /make-test.sh

# Code file to execute when the docker container starts up (`make-test.sh`)
ENTRYPOINT ["/make-test.sh"]
