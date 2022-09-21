#!/bin/bash
groups | grep libvirt
if [ $? != 0 ]
then
	pkexec usermod -a -G libvirt maximilian
	exit 1
else
	exit 0
fi
