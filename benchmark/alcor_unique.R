library(ggplot2)
library(ggpubr)

h1gc<-read.table("hap1_alcor_unique.gc.txt", header=TRUE, comment.char ="", sep="", fill=TRUE)
h1gc<-h1gc[,-3]
h1gc$haplotype<-rep("H1", dim(h1gc)[1])
h2gc<-read.table("hap2_alcor_unique.gc.txt", header=TRUE, comment.char ="", sep="", fill=TRUE)
h2gc<-h2gc[,-3]
h2gc$haplotype<-rep("H2", dim(h2gc)[1])
gc<-rbind(h1gc, h2gc)
gcplot<-ggplot(gc, aes(x=haplotype, y=GC)) + geom_violin() 

h1len<-read.table("hap1_alcor_unique.len.txt", header=TRUE, sep="\t")
h2len<-read.table("hap2_alcor_unique.len.txt", header=TRUE, sep="\t")
h1len$haplotype<-rep("H1", dim(h1len)[1])
h2len$haplotype<-rep("H2", dim(h2len)[1])
len<-rbind(h1len, h2len)
colnames(len)<-c("Sequence", "length", "haplotype")
lenplot<-ggplot(len, aes(x=haplotype, y=log(length, 10))) + geom_violin()

multi.page <- ggarrange(gcplot,lenplot, nrow=2, ncol=1)
ggexport(multi.page, filename="alcor_unique.pdf")

