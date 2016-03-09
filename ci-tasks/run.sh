#!/bin/bash

# expects myself to be in root of source tree

set -xe

cp ci-tasks/$TASK/Dockerfile .
docker build -t test-runner .

docker run -it test-runner
