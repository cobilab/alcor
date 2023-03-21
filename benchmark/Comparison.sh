#!/bin/bash
#
conda install -c bioconda bedtools=2.29.2 -y
conda install -c bioconda emboss -y 
#
# Get all
bedtools intersect -f 1 -a hap1.alcor.bed  -b TME204_HiFi_HiC_allmap.hap1.chr.fasta.out.gff -wa > hap1_alcor_full_overlap.bed
#
# Get unique
bedtools intersect -a hap1.alcor.bed -b TME204_HiFi_HiC_allmap.hap1.chr.fasta.out.gff -v > hap1_alcor_unique.bed
#
# Get unique repeat masker
bedtools intersect -a TME204_HiFi_HiC_allmap.hap1.chr.fasta.out.gff -b hap1.alcor.bed -v > hap1_repeatmasker_unique.bed
#
# Extracts sequences by the unique coordinates for measuring the GC and length
bedtools getfasta -fi TME204_HiFi_HiC_allmap.hap1.fasta.masked -bed hap1_alcor_unique.bed > hap1_alcor_unique.fa
geecee hap1_alcor_unique.fa hap1_alcor_unique.gc.txt
perl fas_getlen.pl hap1_alcor_unique.fa > hap1_alcor_unique.len.txt
#
# Same thing but for H2
bedtools intersect -f 1 -a hap2.alcor.bed -b TME204_HiFi_HiC_allmap.hap2.chr.fasta.out.gff -wa > hap2_alcor_full_overlap.bed
bedtools intersect -a hap2.alcor.bed -b TME204_HiFi_HiC_allmap.hap2.chr.fasta.out.gff -v > hap2_alcor_unique.bed
bedtools intersect -a TME204_HiFi_HiC_allmap.hap2.chr.fasta.out.gff -b hap2.alcor.bed -v > hap2_repeatmasker_unique.bed
bedtools getfasta -fi TME204_HiFi_HiC_allmap.hap2.fasta.masked -bed hap2_alcor_unique.bed > hap2_alcor_unique.fa
geecee hap2_alcor_unique.fa hap2_alcor_unique.gc.txt
perl fas_getlen.pl hap2_alcor_unique.fa > hap2_alcor_unique.len.txt
#
perl repeatmasker_count_missing.pl TME204_HiFi_HiC_allmap.hap1.chr.fasta.out.gff hap1_repeatmasker_unique.bed
perl repeatmasker_count_missing.pl TME204_HiFi_HiC_allmap.hap2.chr.fasta.out.gff hap2_repeatmasker_unique.bed
#
