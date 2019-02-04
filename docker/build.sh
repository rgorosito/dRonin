#!/bin/sh

set -x

cd $(dirname $0)

. ./config

realparentpath=$(realpath ../)
podman build -t dronin -v ${realparentpath}:/dRonin.git .

cd -
