#!/usr/bin/env bash
set -xeuo pipefail

docker build -t "${DOCKER_IMAGE_NAME}" -f Dockerfile .

docker run \
   -it --rm \
   -v "${HIFI_REPO_PATH}":/hifi \
   -e "RELEASE_NUMBER=${RELEASE_NUMBER}" \
   -e "RELEASE_TYPE=${RELEASE_TYPE}" \
   -e "ANDROID_BUILD_TARGET=${ANDROID_BUILD_TARGET}" \
   "${DOCKER_IMAGE_NAME}" \
   sh -c "${ANDROID_BUILD_COMMAND}"
