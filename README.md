![Conda](https://img.shields.io/conda/dn/bioconda/alcor)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)

<p align="center"><img src="imgs/logo.png" 
alt="AlcoR" width="300" height="260" border="0" /></p>
<b>Alignment-free computation and visualization of local and distant redundant regions in FASTA data</b>. This method provides the ability of fast sequence characterization through redundancy, ideally for scenarios entangling the presence of new or unknown sequences. AlcoR is implemented in C language using multi-threading to increase the computational speed, is flexible for multiple applications, and does not contain external dependencies. The tool accepts any sequence in FASTA format.
The AlcoR tool contains one main menu (command: AlcoR) with the four sub menus for computing the features that it provides, namely
<ul>
<li><b>extract</b>: extracts a sequence of a FASTA file using positional coordinates (independent from the existing headers of the FASTA files);</li>
<li><b>redundancy</b>: computes the redundancy of a FASTA read while providing bidirectional complexity profiles and further structural similarity analysis;</li>
<li><b>simulation</b>: FASTA sequence simulation with features: file extraction, random generation, sequence modeling (with SNPs specific mutations);</li>
<li><b>visual</b>: computes an SVG file with the respective map containing the redundant regions.</li>
</ul>

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

Run AlcoR redundancy mapper:
<pre>
AlcoR redundancy -v -n -m 13:50:0:1:10:0.9/5:10:0.9 --dna -w 1.0 -t 0.5 sample.fasta
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

If you are not interested in setting the template for each model, then use the levels mode. To see the possible levels type:
<pre>
AlcoR redundancy -s
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

                                                    

