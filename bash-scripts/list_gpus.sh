#! /bin/bash
# $1 = line

#lspci | grep VGA | sed 's/.*\[//' | sed 's/\].*//'
#lspci -nn | grep '\[03' | sed 's/^[^\[]*\[//' | sed 's/^[^\[]*\[//' | sed 's/\].*//' | sed 's/\].*//'
lspci | grep $(lspci -nn | grep '\[03' | sed 's/ .*//') | sed 's/.*: //' | sed -n $1p

#create a condition where the output would be reduced based on what GPU is used
