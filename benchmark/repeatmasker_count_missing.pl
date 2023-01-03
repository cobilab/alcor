#/usr/local/ngseq/bin/perl
#my $tot="TME204_HiFi_HiC_allmap.hap1.chr.fasta.out.gff";
#my $tot="TME204_HiFi_HiC_allmap.hap2.chr.fasta.out.gff";
my $tot=shift;
my $uni=shift; 
open IN, "<$tot";
my %hash=();
while(<IN>){
	chomp;
	my @fields=split(/\t/, $_);
	my @vals=split(/\s+/, $fields[8]);
	my @tmp=split(/\:/,$vals[1]);
	my $fam=$tmp[1];
	$fam =~ s/\"//;
	if(exists $hash{$fam}){
		$hash{$fam}=$hash{$fam}+1;
	}else{
		$hash{$fam}=1;
	}
}
close IN;

#foreach my $k (keys %hash){
	#print "$k\t$hash{$k}\n";
#}

#my $uni="hap1_repeatmasker_unique.bed";
#my $uni="hap2_repeatmasker_unique.bed";

open IN1, "<$uni";
my $c=0;
my %hash2=();
while(<IN1>){
	chomp;
	my @fields=split(/\t/, $_);
	my @vals=split(/\s+/, $fields[8]);
	my @tmp=split(/\:/,$vals[1]);
	my $fam=$tmp[1];
	$fam =~ s/\"//;
	if(exists $hash2{$fam}){
		$hash2{$fam}=$hash2{$fam}+1;
	}else{
		$hash2{$fam}=1;
		$c++;
	}
	if(exists $hash{$fam}){
		$hash{$fam}=$hash{$fam}-1;
	}else{
		$hash{$fam}=-1;
	}
}
close IN1;
print "$c\n\n";

foreach my $k (keys %hash){
	if($hash{$k}<1){
		print "$k\t$hash{$k}\n";
	}
}

