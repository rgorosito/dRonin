#!/bin/sh

set -x

cd $(dirname $0)

. ./config

podman container rm ${CONTAINER_NAME}

cd -

