#!/bin/sh

set -x

thispath=$(dirname $0)

cd $thispath

realparentpath=$(realpath ../)
podman build -t dronin -v ${realparentpath}:/dRonin.git .

cd -
