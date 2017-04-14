#!/bin/sh

# NOTE: run only after training
# install trained model and user info for facial auth config

cd "$(dirname "$0")"

sudo mkdir -p /etc/pam-facial-auth
sudo cp model.xml /etc/pam-facial-auth/model.xml
sudo cp config /etc/pam-facial-auth/config
