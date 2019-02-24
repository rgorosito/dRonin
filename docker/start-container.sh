#!/bin/sh

set -x

cd $(dirname $0)

. ./config

podman container start ${CONTAINER_NAME}

cd -

