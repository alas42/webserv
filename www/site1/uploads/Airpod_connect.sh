#!/bin/bash

# for device in $(bluetoothctl devices  | grep -o "[[:xdigit:]:]\{8,17\}"); do
#     echo "removing bluetooth device: $device | $(bluetoothctl remove $device)"
# done


bluetoothctl connect A0:A3:09:5C:76:95
# bluetoothctl connect 0C:3B:50:8C:C6:8A
