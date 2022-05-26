<p align="center">

![Conda](https://img.shields.io/conda/dn/bioconda/alcor)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)
[![Speed](https://img.shields.io/static/v1.svg?label=Ultra-Fast&message=High%20speed%20performance&color=green)](#)
[![NoFP](https://img.shields.io/static/v1.svg?label=Testing&message=No%20false%20positives&color=orange)](#)
[![AFM](https://img.shields.io/static/v1.svg?label=AFM&message=alignment-free%20method&color=yellow)](#)

</p>

<p align="center"><img src="imgs/logo_trans.png" alt="AlcoR" width="250" border="0" /></p>
<p align="center">
<b>Alignment-free simulation, mapping, and visualization of low-complexity regions in FASTA data</b>. 
</p>

<p align="justify">
This method provides the ability of fast sequence characterization through low-complexity regions, ideally for scenarios entangling the presence of new or unknown sequences. AlcoR offers important analysis advantages, such as the high-sensitivity, speed, and does not provide false positives, ideally to be applied in the current Telomere-to-telomere (T2T) sequencing and assembly methodologies. AlcoR is implemented in C language using multi-threading to increase the computational speed, is flexible for multiple applications, and does not contain external dependencies. The tool accepts any sequence in FASTA format.

The AlcoR tool contains one main menu (command: AlcoR) with the four sub menus for computing the features that it provides, namely
<ul>
<li><b>extract</b>: extracts a sequence of a FASTA file using positional coordinates (independent from the existing headers of the FASTA files);</li>
<li><b>mapper</b>: computes the low-complexity regions of a FASTA read while providing bidirectional complexity profiles and further structural similarity analysis;</li>
<li><b>simulation</b>: FASTA sequence simulation with features: file extraction, random generation, sequence modeling. Additionally, it allwos to apply specific SNPs probability mutations;</li>
<li><b>visual</b>: computes an SVG file with the respective map containing the low-complexity regions.</li>
</ul>
</p>

## INSTALLATION ##

### Conda
Install [Miniconda](https://docs.conda.io/en/latest/miniconda.html), then run the following:
```bash
conda install -y -c bioconda alcor
```

Otherwise, CMake is needed for installation (http://www.cmake.org/). You can download it directly from http://www.cmake.org/cmake/resources/software.html or use an appropriate packet manager. In the following instructions we show the procedure to install, compile and run AlcoR:

<pre>
sudo apt-get install cmake git
git clone https://github.com/cobilab/alcor.git
cd alcor/src/
cmake .
make
</pre>

## EXECUTION

### Run AlcoR

Run AlcoR FASTA simulation tool:

<pre>
AlcoR simulation -rs 2000:0:1:0:0:0 -rs 2000:0:11:0:0:0 -rs 2000:0:1:0:0:0 -rs 2000:0:71:0:0:0 > sample.fasta;
</pre>

Run AlcoR low-complexity mapper:
<pre>
AlcoR mapper -v -n -m 13:50:0:1:10:0.9/5:10:0.9 --dna -w 3 -t 0.5 sample.fasta
</pre>

## PARAMETERS

To see the possible options type
<pre>
AlcoR
</pre>
or
<pre>
AlcoR -h
</pre>

If you are not interested in viewing each sub-program option, type 
<pre>
AlcoR extract -h
AlcoR mapper -h
AlcoR simulation -h
AlcoR visual -h
</pre>

## CITATION ##

On using this software/method please cite:

* pending

## ISSUES ##

For any issue let us know at [issues link](https://github.com/cobilab/alcor/issues).

## LICENSE ##

GPL v3.

For more information:
<pre>http://www.gnu.org/licenses/gpl-3.0.html</pre>

