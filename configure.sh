#!/bin/sh

# NOTE: run only after training
# install trained model and user info for facial auth config

cd "$(dirname "$0")"

sudo mkdir -p /etc/pam-facial-auth
sudo cp trained_model.xml /etc/pam-facial-auth/trained_model.xml

# change image source here
sudo bash -c 'cat <<EOF > /etc/pam-facial-auth/config
imageDir=/var/lib/motioneye/Camera1
imageWindow=5
EOF'