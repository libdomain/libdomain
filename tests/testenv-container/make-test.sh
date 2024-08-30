#!/bin/bash -ex

export BUILD_DIR=build
export KRB5CCNAME=`mktemp`

echo '10.5.0.2      dc0.domain.alt dc0' >> /etc/hosts
cp /krb5.conf /etc/krb5.conf
cat /resolv.conf > /etc/resolv.conf
chown -R builder2:builder2 /app/
echo 'password145Qw!' | kinit administrator@DOMAIN.ALT || :
cd /app/ && rm -rf $BUILD_DIR && mkdir $BUILD_DIR && cd $BUILD_DIR && pwd 

if [[ "$PIPELINE_TYPE" == "MEMCHECK" ]]; then
  cmake cmake -DLIBDOMAIN_BUILD_TESTS:BOOL=ON -DCMAKE_BUILD_TYPE=Debug -B . .. && make # && ctest --output-on-failure
else
  cmake -DLIBDOMAIN_BUILD_TESTS:BOOL=ON -B . .. && make && ctest --verbose
fi
