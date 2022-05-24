#!/bin/bash
#
cat H*_C*.fa > all.fa
FALCON -v -F -l 1 -t 100 -n 8 ref.txt all.fa
#
