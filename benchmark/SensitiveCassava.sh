#!/bin/bash
#
THRESHOLD=" 1.0 ";
#
# ------------------------------------------------------------------------------
#
for((x=1;x<=18;++x));
  do
  echo "Running $x ...";
  echo "[H1] ...";
  ./AlcoR mapper -v --hide --threshold $THRESHOLD --ignore 5 --dna -w 10 -m 14:50:0:1:10:0.9/3:10:0.9 -m 13:50:5000:1:10:0.9/3:10:0.9 -k -o masked-H1_C$x.fa H1_C$x.fa > positions-h1-$x.txt
  echo "[H2] ...";
  ./AlcoR mapper -v --hide --threshold $THRESHOLD --ignore 5 --dna -w 10 -m 14:50:0:1:10:0.9/3:10:0.9 -m 13:50:5000:1:10:0.9/3:10:0.9 -k -o masked-H2_C$x.fa H2_C$x.fa > positions-h2-$x.txt
  done
#
# ------------------------------------------------------------------------------
#
