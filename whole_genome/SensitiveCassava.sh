#!/bin/bash
#
THRESHOLD=" 0.75 ";
#
# ------------------------------------------------------------------------------
#
./AlcoR mapper -v --hide --threshold $THRESHOLD --ignore 50 --dna -w 100 -m 14:50:0:1:10:0.9/3:10:0.9 -m 13:50:5000:1:10:0.9/3:10:0.9 -k -o masked-Cassava-H1.fa --renormalize --prefix WG- CASSAVA_H1.fa
./AlcoR visual -v -o map2.svg -s 6 -w 18 -e 0 WG-1.txt:WG-2.txt:WG-3.txt:WG-4.txt:WG-5.txt:WG-6.txt:WG-7.txt:WG-8.txt:WG-9.txt:WG-10.txt:WG-11.txt:WG-12.txt:WG-13.txt:WG-14.txt:WG-15.txt:WG-16.txt:WG-17.txt:WG-18.txt
#
# ------------------------------------------------------------------------------
#
