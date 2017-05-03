#!/bin/bash
#set -x
cd `dirname $0`/../proto || exit 1;

../bin/protoc --go_out=plugins=grpc:../src/pb *.proto

sh export.sh
