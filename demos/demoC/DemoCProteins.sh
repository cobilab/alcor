#!/bin/bash
#
WINDOW=" 20 ";
#
PROJECT (){
  mapfile -t DATA < $1
  rm -f $1.pro
  for line in "${DATA[@]}" #
    do
    #
    INI=`echo $line | awk '{ print $1 }'`;
    END=`echo $line | awk '{ print $2 }'`;
    COLOR=`echo $line | awk '{ print $3 }'`;
    #
    for(( x=$INI ; x<=$END ; ++x));
      do
      echo -e "$x\t$2" >> $1.pro ;
      done
    done
    }
#
./AlcoR redundancy -v -n -m 5:50:500:1:10:0.9/1:10:0.9 -w $WINDOW sampleCProteins.fasta 1> pos_500.txt
./AlcoR redundancy -v -n -m 5:50:5000:1:10:0.9/1:10:0.9 -w $WINDOW sampleCProteins.fasta 1> pos_5k.txt
./AlcoR redundancy -v -n -m 5:50:0:1:10:0.9/1:10:0.9 -w $WINDOW sampleCProteins.fasta 1> pos_all.txt
#
PROJECT "pos_500.txt" "0.25";
PROJECT "pos_5k.txt" "0.75";
PROJECT "pos_all.txt" "1.25";
#
SIZE=`grep -v ">" sampleCProteins.fasta | tr -d -c "ACGT" | wc -c`;
#
  gnuplot << EOF
    reset
    set terminal pdfcairo enhanced color font 'Verdana,10'
    set output "outCProteins.pdf"
    set style line 101 lc rgb '#000000' lt 1 lw 4
    set border 3 front ls 101
    set tics nomirror out scale 0.5
    set format '%g'
    set size ratio 0.16
    set key out horiz center top
    set yrange [0:1.5]
    set xrange [0:30000]
    set xtics auto
    #set ytics auto
    set ytics ("c=500" 0.25, "c=5k" 0.75, "c=all" 1.25)
    set grid
    set format x '%.0s%c'
    set ylabel "   Map   "
    set xlabel "Length"
    set border linewidth 1.5
    set style line 1 lc rgb '#004C99' lt 1 lw 1.2 pt 5 ps 0.4 # --- blue
    #
    set style line 5 lc rgb '#CCCC00' lt 1 lw 14 pt 1 ps 0.01 # --- green
    set style line 6 lc rgb '#006400' lt 1 lw 14 pt 1 ps 0.01 # --- gold
    set style line 7 lc rgb '#CC0000' lt 1 lw 14 pt 1 ps 0.01 # --- red
    #
    set style line 9 lc rgb '#CCCC00' lt 1 lw 1 pt 5 ps 0.4 # ---
    set style line 10 lc rgb '#006400' lt 1 lw 1 pt 5 ps 0.4 # ---
    set style line 11 lc rgb '#CC0000' lt 1 lw 1 pt 5 ps 0.4 # ---
    set style line 12 lc rgb '#FFFFFF' lt 1 lw 1 pt 5 ps 0.4 # ---
    #
    plot "pos_500.txt.pro" using 1:2 w points t "" ls 5, "pos_5k.txt.pro" using 1:2 w points t "" ls 6, "pos_all.txt.pro" using 1:2 w points t "" ls 7, 0.25 t "" with lines ls 9, 0.75 t "" with lines ls 10, 1.25 t "" with lines ls 11
EOF

