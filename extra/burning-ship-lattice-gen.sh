#!/bin/sh

ourdir="$(dirname $0)"

exec "${ourdir}/../burning-ship-gen" -x -1.755 -y -0.037 -z 0.09 $@
