#!/bin/sh

# NOTE: run only after training
# install trained model and user info for facial auth config

cd "$(dirname "$0")"

sudo mkdir -p /etc/pam-facial-auth
sudo cp model /etc/pam-facial-auth/model
sudo cp model-raw.xml /etc/pam-facial-auth/model-raw.xml
sudo cp config /etc/pam-facial-auth/config
