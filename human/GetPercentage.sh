#!/bin/bash
#
input="$1";
#
IDX=0;
TOTAL=0;
PART=0;
SUM_PART=0;
while IFS= read -r line
  do
  if [[ $IDX == 0 ]];
    then
    TOTAL=`echo $line | awk '{ print $2}'`;
    else
    #
    PART_1=`echo "$line" | awk '{ print $1;}'`;
    PART_2=`echo "$line" | awk '{ print $2;}'`;
    #
    PART=`echo "$PART_2-$PART_1+1" | bc -l`;
    #
    ((SUM_PART=SUM_PART+PART));
    #
    fi
  ((++IDX));
  #
done < "$input"
#
PROPORTION=`echo "scale=3;$SUM_PART / $TOTAL * 100; " | bc -l`;
echo "Redudant parts: $PROPORTION % [ $SUM_PART (of $TOTAL )]" ;

