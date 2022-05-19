#!/bin/bash
#
rm -f H-per-out.txt;
for((x=1;x<=24;++x)); 
  do 
  printf "$x\t" >> H-per-out.txt ;
  head -n 1 pc-d-$x.txt > alcor-tmp.tmp;
  cat alcor-tmp.tmp pc-l-$x.txt > h-pc-l-$x.txt;
  ./GetPercentage.sh h-pc-l-$x.txt >> H-per-out.txt; 
  rm -f alcor-tmp.tmp h-pc-l-$x.txt;
  printf "\t" >> H-per-out.txt ;
  ./GetPercentage.sh pc-d-$x.txt >> H-per-out.txt ; 
  printf "\n" >> H-per-out.txt ;
  done;
#
cat H-per-out.txt | awk '{ print $1"\t"$4"\t"$7"\t"$13"\t"$16"\t"$18;}'
#
