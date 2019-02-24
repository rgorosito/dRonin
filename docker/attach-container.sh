#!/bin/sh

set -x

cd $(dirname $0)

. ./config

podman container attach ${CONTAINER_NAME}

cd -

