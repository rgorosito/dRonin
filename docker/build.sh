#!/bin/sh

set -x

thispath=$(dirname $0)

cd $thispath

realparentpath=$(realpath ../)
podman build -t prueba -v ${realparentpath}:/dRonin.git .

cd -
