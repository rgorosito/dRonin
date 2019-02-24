#!/bin/sh

set -x

cd $(dirname $0)

. ./config

podman exec -it ${CONTAINER_NAME} /bin/bash

cd -

