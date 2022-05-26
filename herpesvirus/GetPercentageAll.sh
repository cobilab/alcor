#!/bin/bash
#
declare -a GENOMES=("HSV-1" "HSV-2" "VZV" "EBV" "HCMV" "HHV6A" "HHV6B" "HHV7" "KSHV");
#
rm -f V-per-out.txt;
for GENOME in "${GENOMES[@]}"
  do
  ./AlcoR mapper -v --hide --color 100 --threshold $THRESHOLD --ignore 50 --dna -w 50 -m 13:50:0:1:10:0.9/3:10:0.9 $GENOME.fa > $GENOME-d.txt
  ./AlcoR mapper -v --hide --threshold $THRESHOLD --color 1 --ignore 50 --dna -w 50 -m 13:50:5000:1:10:0.9/3:10:0.9 $GENOME.fa > $GENOME-l.txt 
  printf "$GENOME\t" >> V-per-out.txt ;
  ./GetPercentage.sh $GENOME-l.txt >> V-per-out.txt ; 
  printf "\t" >> V-per-out.txt ;
  ./GetPercentage.sh $GENOME-d.txt >> V-per-out.txt ; 
  printf "\n" >> V-per-out.txt ;
  done; 
#
cat V-per-out.txt | awk '{ print $1"\t"$4"\t"$7"\t"$13"\t"$16"\t"$18;}'
#
