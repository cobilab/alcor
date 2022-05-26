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
./AlcoR mapper -v -n -m 13:50:0:1:10:0.9/5:10:0.9 --dna -w $WINDOW -t 0.5 sampleB.fasta 1> pos_05.txt
./AlcoR mapper -v -n -m 13:50:0:1:10:0.9/5:10:0.9 --dna -w $WINDOW -t 1.0 sampleB.fasta 1> pos_10.txt
./AlcoR mapper -v -n -m 13:50:0:1:10:0.9/5:10:0.9 --dna -w $WINDOW -t 1.5 sampleB.fasta 1> pos_15.txt
#
PROJECT "pos_05.txt" "3.0";
PROJECT "pos_10.txt" "3.5";
PROJECT "pos_15.txt" "4.0";
#
SIZE=`grep -v ">" sampleB.fasta | tr -d -c "ACGT" | wc -c`;
#
  gnuplot << EOF
    reset
    set terminal pdfcairo enhanced color font 'Verdana,10'
    set output "outB.pdf"
    set style line 101 lc rgb '#000000' lt 1 lw 4
    set border 3 front ls 101
    set tics nomirror out scale 0.5
    set format '%g'
    set size ratio 0.16
    set key out horiz center top
    set yrange [0:4.0]
    set xrange [:]
    set xtics auto
    #set ytics auto
    set ytics ("0.0" 0, "0.5" 0.5, "1.0" 1.0, "1.5" 1.5, "2.0" 2.0, "T_{0.5}" 3.0, "T_{1.0}" 3.5, "T_{1.5}" 4.0)
    set grid
    set format x '%.0s%c'
    set ylabel "Complexity   Map   "
    set xlabel "Length"
    set border linewidth 1.5
    set style line 1 lc rgb '#004C99' lt 1 lw 1.2 pt 5 ps 0.4 # --- blue
    #
    set style line 2 lc rgb '#CCCC00' lt 2 dashtype '---' lw 2 pt 5 ps 0.4 # --- green
    set style line 3 lc rgb '#006400' lt 2 dashtype '---' lw 2 pt 5 ps 0.4 # --- gold
    set style line 4 lc rgb '#CC0000' lt 2 dashtype '---' lw 2 pt 5 ps 0.4 # --- red
    #
    set style line 5 lc rgb '#CCCC00' lt 1 lw 10 pt 1 ps 0.01 # --- green
    set style line 6 lc rgb '#006400' lt 1 lw 10 pt 1 ps 0.01 # --- gold
    set style line 7 lc rgb '#CC0000' lt 1 lw 10 pt 1 ps 0.01 # --- red
    #
    set style line 9 lc rgb '#CCCC00' lt 1 lw 1 pt 5 ps 0.4 # ---
    set style line 10 lc rgb '#006400' lt 1 lw 1 pt 5 ps 0.4 # ---
    set style line 11 lc rgb '#CC0000' lt 1 lw 1 pt 5 ps 0.4 # ---
    set style line 12 lc rgb '#FFFFFF' lt 1 lw 1 pt 5 ps 0.4 # ---
    #
    plot "sampleB.fasta.info" using 1 t "Sample" with lines ls 1, 0.5 t "T_{0.5}" with lines ls 2, 1.0 t "T_{1.0}" with lines ls 3, 1.5 t "T_{1.5}" with lines ls 4, "pos_05.txt.pro" using 1:2 w points t "" ls 5, "pos_10.txt.pro" using 1:2 w points t "" ls 6, "pos_15.txt.pro" using 1:2 w points t "" ls 7, 3.0 t "" with lines ls 9, 3.5 t "" with lines ls 10, 4.0 t "" with lines ls 11, 2.5 t "" with lines ls 12
EOF

