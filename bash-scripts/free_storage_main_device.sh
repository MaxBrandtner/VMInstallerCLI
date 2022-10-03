#!/bin/bash
df -H | grep '/n' | awk '{print $4}' | sed 's/G//'
