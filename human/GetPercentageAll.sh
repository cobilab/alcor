#!/bin/bash
#
for((x=1;x<=24;++x)); 
  do 
  ./GetPercentage.sh pc-d-$x.txt >> H-per-out.txt ; 
  done; 
#
cat H-per-out.txt | awk '{ print $3"\t"$6"\t"$8;}'
#
