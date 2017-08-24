#!/bin/bash

sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential git-core
git clone git://git.drogon.net/wiringPi
cd wiringPi
git pull origin
./build
gpio -v
cd ..
make clean all target=raspberry

exit 1
