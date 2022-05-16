#!/bin/bash
#
THRESHOLD=" 0.6 ";
#
for((x=1;x<=18;++x));
  do
  ./AlcoR redundancy -v --hide --color 100 --threshold $THRESHOLD --ignore 5000 --dna -w 5000 -m 13:50:0:1:10:0.9/3:10:0.9 H1_C$x.fa > h1-c-d-$x.txt
  ./AlcoR redundancy -v --hide --color 100 --threshold $THRESHOLD --ignore 5000 --dna -w 5000 -m 13:50:0:1:10:0.9/3:10:0.9 H2_C$x.fa > h2-c-d-$x.txt
  ./AlcoR redundancy -v --no-size --hide --threshold $THRESHOLD --color 1 --ignore 5000 --dna -w 5000 -m 13:50:5000:1:10:0.9/3:10:0.9 H1_C$x.fa > h1-c-l-$x.txt
  ./AlcoR redundancy -v --no-size --hide --threshold $THRESHOLD --color 1 --ignore 5000 --dna -w 5000 -m 13:50:5000:1:10:0.9/3:10:0.9 H2_C$x.fa > h2-c-l-$x.txt
  done
#
for((x=1;x<=18;++x));
  do
  cat h1-c-d-$x.txt h1-c-l-$x.txt > h1-$x.txt;
  cat h2-c-d-$x.txt h2-c-l-$x.txt > h2-$x.txt;
  done
#
./AlcoR visual -o map1.svg -s 6 -w 18 -e 0 h1-1.txt:h2-1.txt:h1-2.txt:h2-2.txt:h1-3.txt:h2-3.txt:h1-4.txt:h2-4.txt:h1-5.txt:h2-5.txt:h1-6.txt:h2-6.txt:h1-7.txt:h2-7.txt:h1-8.txt:h2-8.txt:h1-9.txt:h2-9.txt:h1-10.txt:h2-10.txt:h1-11.txt:h2-11.txt:h1-12.txt:h2-12.txt:h1-13.txt:h2-13.txt:h1-14.txt:h2-14.txt:h1-15.txt:h2-15.txt:h1-16.txt:h2-16.txt:h1-17.txt:h2-17.txt:h1-18.txt:h2-18.txt
#
