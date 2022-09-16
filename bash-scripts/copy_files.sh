#!/bin/bash
# $1 iso_path
# $2 rom_path

mkdir -p .tmp
mkdir -p .tmp/files
cp $1 .tmp/files/input.iso
cp $2 .tmp/files/input.rom
