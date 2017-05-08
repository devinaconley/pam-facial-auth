# pam-facial-auth

A pluggable authentication module that relies on facial recognition to
verify the user attempting to gain access. Looks for a match between
username and label associated with the recognized face.

**Please don't use facial recognition as a serious form of authentication.
This was literally written to be exploited in a CTF**

Requirements
------------
- OpenCV 3.0+ including extra modules (opencv_contrib)
- PAM development packages (libpam0g and libpam0g-dev on ubuntu)
- dirent.h available in PATH (already installed on ubuntu)

Quickstart
----------
Clone this repository
```
git clone https://github.com/devinaconley/pam-facial-auth.git
```
Build and install facial auth module
```
chmod +x setup.sh
./setup.sh
```
Train facial recognition model
```
./run_training [data_dir] [algorithm (optional) haar_cascade(optional)]
```
Where the data directory is expected to be of the following structure
```
data_dir/
-- username1/
---- image1
---- image2
---- ...
---- imageN
-- username2/
...
```
Configure facial auth settings in *config*. Setting imageCapture to
false will mean the module passively listens for a stream of images being
written to imageDir (for example by motionEye -
https://github.com/ccrisan/motioneye/wiki)
```
imageCapture=true
imageDir=/var/lib/motioneye/Camera1
timeout=10
threshold=1000.00
```
Install the config and model files to *etc/pam-facial-auth/*
```
chmod +x configure.sh
./configure.sh
```


Run test application
```
./run_test
```

Credit
------
h/t to https://github.com/beatgammit/simple-pam for starting template