#!/bin/bash

cd "$(dirname $(readlink -f $0))"

git clone --recursive https://github.com/StanHash/EventAssembler.git
cd EventAssembler
./build.sh
