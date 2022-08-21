#!/bin/bash
# $1 iso_path
# $2 rom_path

iso_file
rom_file

if [ $(echo $1 | grep '\"') == 0]
then
  $iso_file = $1

else
  $iso_file = $(echo "\"$1\"")

fi

if [ $(echo $2 | grep '\"') == 0]
then
  $rom_file = $2

else
  $rom_file = $(echo "\"$2\"")

fi

mkdir files
mv $iso_file files/input.iso
mv $rom_file files/input.rom
