#!/bin/bash -x

set -o errexit
set -o nounset
set -o pipefail

mkdir -p bin build
(
  cd build
  cmake -G"Unix Makefiles" ..
  make
  mv test ../bin
)
#rm -r build
