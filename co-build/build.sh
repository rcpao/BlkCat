#!/bin/bash -x

# build.sh
# /home/rcpao/Documents/github.com-rcpao/BlkCat/co-build/build.sh


UDK2014_IHV_zip=~/Downloads/UDK2014.IHV.zip
UDK2014_IHV_DIR=UDK2014.IHV
[ ! -f $UDK2014_IHV_zip ] && wget https://github.com/tianocore-docs/Docs/raw/master/Driver_Developer/UDK2014.IHV.zip
[ ! -d $UDK2014_IHV_DIR ] && unzip $UDK2014_IHV_zip

pushd $UDK2014_IHV_DIR 
  [ ! -f edksetup.sh ] && tar xvf 'BaseTools(Unix).tar'
popd
