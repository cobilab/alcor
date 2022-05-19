#!/bin/bash
#
rm -f C-per-out.txt;
for((x=1;x<=18;++x)); 
  do 
  printf "$x\t" >> C-per-out.txt ;
  #
  # H1
  head -n 1 h1-c-d-$x.txt > alcor-tmp.tmp;
  cat alcor-tmp.tmp h1-c-l-$x.txt > F-h1-c-l-$x.txt;
  ./GetPercentage.sh F-h1-c-l-$x.txt >> C-per-out.txt; 
  rm -f alcor-tmp.tmp F-h1-c-l-$x.txt;
  printf "\t" >> C-per-out.txt ;
  ./GetPercentage.sh h1-c-d-$x.txt >> C-per-out.txt ; 
  head -n 1 h1-c-d-$x.txt > alcor-tmp.tmp;
  #
  #H2
  cat alcor-tmp.tmp h2-c-l-$x.txt > F-h2-c-l-$x.txt;
  ./GetPercentage.sh F-h2-c-l-$x.txt >> C-per-out.txt;
  rm -f alcor-tmp.tmp F-h2-c-l-$x.txt;
  printf "\t" >> C-per-out.txt ;
  ./GetPercentage.sh h2-c-d-$x.txt >> C-per-out.txt ;
  #
  printf "\n" >> C-per-out.txt ;
  done;
#
cat C-per-out.txt | awk '{ print $1"\t"$4"\t"$13"\t"$21"\t"$30"\t"$9;}'
#
