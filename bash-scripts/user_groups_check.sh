#!/bin/bash
user=$(whoami)
groups | grep libvirt
if [ $? != 0 ]
then
	pkexec usermod -a -G libvirt $user
	exit 1
else
	exit 0
fi
