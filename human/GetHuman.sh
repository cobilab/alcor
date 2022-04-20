#!/bin/bash
#
# ==============================================================================
#
wget https://s3-us-west-2.amazonaws.com/human-pangenomics/T2T/CHM13/assemblies/analysis_set/chm13v2.0.fa.gz
gunzip chm13v2.0.fa.gz
#
conda install -y -c cobilab gto
gto_fasta_split_reads < chm13v2.0.fa
#
# ==============================================================================
