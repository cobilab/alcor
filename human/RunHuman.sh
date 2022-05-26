#!/bin/bash
#
for((x=1;x<=24;++x));
  do 
  ./AlcoR mapper -v --hide --color 100 --ignore 5000 --dna -w 5000 -m 13:50:0:1:10:0.9/3:10:0.9 out$x.fasta > pc-d-$x.txt
  ./AlcoR mapper -v --no-size --hide --color 1 --ignore 5000 --dna -w 5000 -m 13:50:5000:1:10:0.9/3:10:0.9 out$x.fasta > pc-l-$x.txt
  cat pc-d-$x.txt pc-l-$x.txt > pc-$x.txt;
  done
for((x=1;x<=24;++x));
  do
  cat pc-d-$x.txt pc-l-$x.txt > pc-$x.txt;
  done
#
./AlcoR visual -o map1.svg -s 8 -d 20 -e 0 pc-1.txt:pc-2.txt:pc-3.txt:pc-4.txt:pc-5.txt:pc-6.txt:pc-7.txt:pc-8.txt:pc-9.txt:pc-10.txt:pc-11.txt:pc-12.txt:pc-13.txt:pc-14.txt:pc-15.txt:pc-16.txt:pc-17.txt:pc-18.txt:pc-19.txt:pc-20.txt:pc-21.txt:pc-22.txt:pc-23.txt:pc-24.txt
#
