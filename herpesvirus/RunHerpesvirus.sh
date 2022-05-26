#!/bin/bash
#
THRESHOLD=" 1.2 ";
#
declare -a GENOMES=("HSV-1" "HSV-2" "VZV" "EBV" "HCMV" "HHV6A" "HHV6B" "HHV7" "KSHV");
#
for GENOME in "${GENOMES[@]}"
  do
  ./AlcoR mapper -v --hide --color 100 --threshold $THRESHOLD --ignore 50 --dna -w 50 -m 13:50:0:1:10:0.9/3:10:0.9 $GENOME.fa > $GENOME-d.txt
  ./AlcoR mapper -v --no-size --hide --threshold $THRESHOLD --color 1 --ignore 50 --dna -w 50 -m 13:50:5000:1:10:0.9/3:10:0.9 $GENOME.fa > $GENOME-l.txt
  cat $GENOME-d.txt $GENOME-l.txt > $GENOME.txt;
  done
#
./AlcoR visual -o mapv.svg --strict-corner -s 10 -w 10 -e 0 --border-color cccccc HSV-1.txt:HSV-2.txt:VZV.txt:EBV.txt:HCMV.txt:HHV6A.txt:HHV6B.txt:HHV7.txt:KSHV.txt
#
