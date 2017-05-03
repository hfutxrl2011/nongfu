#!/usr/bin/env bash

fileName="game.proto.export"

cd `dirname $0`

if [ ! -f "game.proto" ]; then
    touch "$fileName"
fi

echo  'package pb;'  > "$fileName"

cat command.proto | grep -v 'syntax = "proto2";' | grep -v 'package pb;' | grep -v 'import "room.proto";' >> "$fileName"
cat common.proto | grep -v 'syntax = "proto2";' | grep -v 'package pb;' | grep -v 'import "room.proto";' >> "$fileName"
cat error.proto | grep -v 'syntax = "proto2";' | grep -v 'package pb;' | grep -v 'import "room.proto";' >> "$fileName"
cat hall.proto | grep -v 'syntax = "proto2";' | grep -v 'package pb;' | grep -v 'import "room.proto";' >> "$fileName"
cat rcenter.proto | grep -v 'syntax = "proto2";' | grep -v 'package pb;' | grep -v 'import "room.proto";' >> "$fileName"
cat room.proto | grep -v 'syntax = "proto2";' | grep -v 'package pb;' | grep -v 'import "room.proto";' >> "$fileName"
cat room_scene.proto | grep -v 'syntax = "proto2";' | grep -v 'package pb;' | grep -v 'import "room.proto";' >> "$fileName"

