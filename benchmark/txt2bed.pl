#/usr/local/ngseq/bin/perl

my $file=shift;
my $chr=shift;
open IN, "<$file";
my $header=<IN>;

while(<IN>){
	chomp;
	my @vals=split(/\t/, $_);
	#txt files use 1 base coordinate start
	#my $beg=$vals[0]-1;
	#print "$chr\t$beg\t$vals[1]\n";
	#txt files already used 0 base coordinate start
	print "$chr\t$vals[0]\t$vals[1]\n";
}
close IN;

