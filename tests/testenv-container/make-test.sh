#!/bin/bash -ex

export BUILD_DIR=build
export KRB5CCNAME=`mktemp`

cp /krb5.conf /etc/krb5.conf
cat /resolv.conf > /etc/resolv.conf
chown -R builder2:builder2 /app/
echo 'password145Qw!' | kinit administrator@DOMAIN.ALT || :
cd /app/ && rm -rf $BUILD_DIR && mkdir $BUILD_DIR && cd $BUILD_DIR && pwd && cmake -DLIBDOMAIN_BUILD_TESTS:BOOL=ON -B . .. && make && ctest --output-on-failure
