#!/bin/bash
#
WINDOW=50;
IGNORE=10;
#
# 
./AlcoR mapper -v --hide --color 100 --ignore $IGNORE --dna -w $WINDOW -m 13:50:0:1:10:0.9/3:10:0.9 $1 > pc-d-seq.txt
./AlcoR mapper -v --no-size --hide --color 1 --ignore $IGNORE --dna -w $WINDOW -m 13:50:5000:1:10:0.9/3:10:0.9 $1 > pc-l-seq.txt
cat pc-d-seq.txt pc-l-seq.txt > pc-seq.txt;
#
./AlcoR mapper -v --hide --color 100 --ignore $IGNORE --dna -w $WINDOW -m 13:50:0:2:10:0.9/3:10:0.9 $1 > ir-pc-d-seq.txt
./AlcoR mapper -v --no-size --hide --color 1 --ignore $IGNORE --dna -w $WINDOW -m 13:50:5000:2:10:0.9/3:10:0.9 $1 > ir-pc-l-seq.txt
cat ir-pc-d-seq.txt ir-pc-l-seq.txt > ir-pc-seq.txt;
#
./AlcoR visual -o mapseq.svg -c -s 8 -d 20 -e 0 pc-seq.txt:ir-pc-seq.txt
#
