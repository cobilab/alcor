#!/usr/local/bin/perl

my $file=shift;
#my $anno=shift;
$/=">";
open IN, "<$file";
my $junk=<IN>;
print "Sequence\tbp\n";
while(<IN>){
	chomp;
	my $seq="";	
	my @lines=split(/\n/, $_);
	my @fields=split(/\s+/, $lines[0]);
	my $id=$fields[0];
	for(my $i=1; $i<@lines; $i++){
		my $temp=$lines[$i];
		$temp =~ s/\n//;
		$temp =~ s/\r//;
		$seq.="$temp";
	}
	my $len=length($seq);
	#print "$id\t$len\t$anno\n";	
	print "$id\t$len\n";	
}
close IN;

