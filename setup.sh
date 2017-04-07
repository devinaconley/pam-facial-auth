#!/bin/sh

# build and install facial authentication module and test app

cd "$(dirname "$0")"
mkdir build
cd build
cmake ..
make

sudo cp facialauth.so /lib/security/facialauth.so

sudo bash -c 'cat <<EOF > /etc/pam.d/pam_test
#-------pam_test config---------#
auth sufficient facialauth.so
account sufficient facialauth.so
EOF'
