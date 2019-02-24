#!/bin/sh

set -x

cd $(dirname $0)

. ./config

realparentpath=$(realpath ../)

podman container run -v ${realparentpath}:/dRonin.git -it --rm ${IMAGE_NAME}

cd -

