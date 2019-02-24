#!/bin/sh

set -x

cd $(dirname $0)

. ./config

realparentpath=$(realpath ../)

podman container run -v ${realparentpath}:/dRonin.git --name ${CONTAINER_NAME} -it ${IMAGE_NAME}

cd -

