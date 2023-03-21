<div align="center">

![Conda](https://img.shields.io/conda/dn/bioconda/alcor)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](LICENSE)
[![Speed](https://img.shields.io/static/v1.svg?label=Testing&message=High-speed%20&color=green)](#)
[![NoFP](https://img.shields.io/static/v1.svg?label=Testing&message=High-sensitivity&color=blue)](#)
[![NoFP](https://img.shields.io/static/v1.svg?label=Testing&message=No%20false%20positives&color=orange)](#)
[![AFM](https://img.shields.io/static/v1.svg?label=Method&message=alignment-free&color=yellow)](#)

</div>

<p align="center"><img src="imgs/logo_trans.png" alt="AlcoR" width="250" border="0" /></p>
<p align="center">
<b>Alignment-free simulation, mapping, and visualization of low-complexity regions in FASTA data</b>. 
<a href="https://cobilab.github.io/alcor">https://cobilab.github.io/alcor</a>
</p>

<p align="justify">
AlcoR provides the ability of fast sequence characterization through low-complexity regions, ideally for scenarios entangling the presence of new or unknown sequences. AlcoR offers important analysis advantages, such as the high-sensitivity, speed, and does not provide false positives, ideally to be applied in the current Telomere-to-telomere (T2T) sequencing and assembly methodologies. AlcoR is implemented in C language using multi-threading to increase the computational speed, is flexible for multiple applications, and does not contain external dependencies. The tool accepts any sequence in FASTA format.
</p>

<video width="80%" height="40%" controls>
  <source src="cobilab.github.io/alcor/videos/AlcoR_tutorial.mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>

<p align="justify">
The AlcoR tool contains one main menu (command: <b>AlcoR</b>) with the five sub menus for computing the features that it provides, namely
<ul>
<li><b>info</b>: it provides information of the length and GC percentage for each FASTA read;</li>
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
AlcoR info -h
AlcoR extract -h
AlcoR mapper -h
AlcoR simulation -h
AlcoR visual -h
</pre>

The info menu contains the following information
<pre>
NAME                                                                
      AlcoR info                                        
                                                                    
DESCRIPTION                                                         
      It provides length and GC information of each FASTA read.     
                                                                    
PARAMETERS                                                          
                                                                    
      -h,  --help                                                   
           usage guide (help menu),                                 
                                                                    
      -v,  --verbose                                                
           verbose mode (more information),                         
                                                                    
      -m [INT],  --header-max [INT]                                 
           maximum header size for exporting information,           
                                                                    
      [FILE]                                                        
           input sequence filename (to analyze) -- MANDATORY,       
           FASTA file to retrieve information (last argument).      
                                                                    
SYNOPSIS                                                            
      AlcoR info [OPTION]... [FILE]                         
                                                                    
EXAMPLE                                                             
      AlcoR info -v -f seq.fa
</pre>

The extract menu contains the following information
<pre>
NAME                                                                
      AlcoR extract                                        
                                                                    
DESCRIPTION                                                         
      Extracts a sequence from a FASTA file using coordinates.      
                                                                    
PARAMETERS                                                          
                                                                    
      -h,  --help                                                   
           usage guide (help menu),                                 
                                                                    
      -v,  --verbose                                                
           verbose mode (more information),                         
                                                                    
      -f,  --fasta                                                  
           outputs a FASTA format with the sequence,                
                                                                    
      -i [INT],  --init [INT]                                       
           initial position for extracting the sequence,            
                                                                    
      -e [INT],  --end [INT]                                        
           end position for extracting the sequence,                
                                                                    
      [FILE]                                                        
           input sequence filename (to analyze) -- MANDATORY,       
           FASTA file for the extraction (last argument).           
                                                                    
SYNOPSIS                                                            
      AlcoR extract [OPTION]... [FILE]                         
                                                                    
EXAMPLE                                                             
      AlcoR extract -v -f -i 101 -e 301 seq.fa
</pre>

The mapper menu contains the following information
<pre>
NAME                                                                
      AlcoR mapper                                        
                                                                    
DESCRIPTION                                                         
      Computes the low-complexity regions of a sequence (FASTA).  
                                                                    
PARAMETERS                                                          
                                                                    
      -h,  --help                                                   
           usage guide (help menu),                                 
                                                                    
      -v,  --verbose                                                
           verbose mode (more information),                         
                                                                    
      -n,  --no-size                                                
           does not print the segmented map with the header size,   
                                                                    
      -e,  --hide                                                   
           it hides (deletes) the final smooth profile,             
                                                                    
      -d,  --dna                                                    
           considers exclusively DNA alphabet {A,C,G,T},            
           it also provides inverted repeats models,                
           flag absence considers inversions (without complements), 
                                                                    
      -c [INT],  --color [INT]                                      
           Color Hue to be used in visualization,                   
                                                                    
      -t [FLOAT],  --threshold [FLOAT]                              
           threshold to segment regions (real),                     
                                                                    
      -w [INT],  --window [INT]                                     
           window size to smooth the minimum of both directions     
           of compression using a moving average filter (int),      
                                                                    
      -i [INT],  --ignore [INT]                                     
           ignore lengths of segmented regions below this value,    
                                                                    
      -k,  --mask                                                   
           it masks a FASTA sequence (LCRs appear in lower case),   
                                                                    
      -o,  --output-mask                                            
           output FASTA filename with the masked sequence,          
                                                                    
      -r,  --renormalize                                            
           renormalize the positions of a multi-FASTA file,         
                                                                    
      -f [STRING],  --prefix [STRING]                               
           use this prefix for the output of renormalize (if on),   
                                                                    
      -p,  --show-parameters                                        
           show parameters of the models for optimization,          
                                                                    
      -s,  --show-levels                                            
           show pre-computed compression levels (parameters),       
                                                                    
      -l [INT],  --level [INT]                                      
           compression level (integer),                             
           it defines compressibility in balance with computational 
           resources (RAM and time), use -s for levels perception,  
                                                                    
      [FILE]                                                        
           input sequence filename (to analyze) -- MANDATORY,       
           FASTA file for the analysis (last argument).             
                                                                    
SYNOPSIS                                                            
      AlcoR mapper [OPTION]... [FILE]                         
                                                                    
EXAMPLE                                                             
      AlcoR mapper -v -w 10 -m 13:50:0:1:10:0.9/5:10:0.9 -k -o m.fa seq.fa
</pre>

The simulation menu contains the following information
<pre>
NAME                                                                
      AlcoR simulation                                              
                                                                    
DESCRIPTION                                                         
      Simulation of FASTA sequences with specific features.         
                                                                    
PARAMETERS                                                          
                                                                    
      -h,  --help                                                   
           usage guide (help menu),                                 
                                                                    
      -v,  --verbose                                                
           verbose mode (more information),                         
                                                                    
      -n,  --no-dna                                                 
           Does not consider a DNA alphabet {A,C,G,T},              
           it provides possible inversions without complements,     
                                                                    
      -a [STRING],  --alphabet [STRING]                             
           alphabet to consider (Default: ACGT),                    
                                                                    
      -fs [FEATURES],  --file-segment [FEATURES]                    
           FASTA file segment features:                             
           |    [init:end:ir:seed:subs:adds:dels:file]              
           |    ...                                                 
           init [INT]   - initial position of the segment,          
           end  [INT]   - ending position of the segment,           
           ir   [INT]   - segment inverted if 1, otherwise 0,       
           seed [INT]   - initial number for random generation,     
           subs [FLOAT] - probability of substitution mutation,     
           adds [FLOAT] - probability of addition mutation,         
           dels [FLOAT] - probability of deletion mutation,         
           file [FILE]  - FASTA filename for extracting segment,    
                                                                    
      -rs [FEATURES],  --rand-segment [FEATURES]                    
           Random segment features:                                 
           |    [size:ir:seed:subs:adds:dels]                       
           |    ...                                                 
           size [INT]   - length of the segment,                    
           ir   [INT]   - segment inverted if 1, otherwise 0,       
           seed [INT]   - initial number for random generation,     
           subs [FLOAT] - probability of substitution mutation,     
           adds [FLOAT] - probability of addition mutation,         
           dels [FLOAT] - probability of deletion mutation,         
                                                                    
      -ms [FEATURES],  --model-segment [FEATURES]                   
           Model segment features:                                  
           |    [size:ctx:bet:ir:seed:subs:adds:dels:file]          
           |    ...                                                 
           size [INT]   - length of the segment,                    
           ctx  [INT]   - context to model and simulate,            
           bet  [INT]   - intensity for higher bet strength,        
           ir   [INT]   - segment inverted if 1, otherwise 0,       
           seed [INT]   - initial number for random generation,     
           subs [FLOAT] - probability of substitution mutation,     
           adds [FLOAT] - probability of addition mutation,         
           dels [FLOAT] - probability of deletion mutation,         
           file [FILE]  - FASTA filename for learning model,        
                                                                    
SYNOPSIS                                                            
      AlcoR simulation [OPTION]... > output.fa      
                                                                    
EXAMPLE                                                             
      AlcoR simulation -rs 50:0:1:0.1:0:0 -ms 80:7:50:0:7:0:0:0:x.fa
</pre>

The visual menu contains the following information
<pre>
NAME                                                                
      AlcoR visual                                        
                                                                    
DESCRIPTION                                                         
      Creates an SVG map with the identified regions.               
                                                                    
PARAMETERS                                                          
                                                                    
      -h,  --help                                                   
           usage guide (help menu),                                 
                                                                    
      -v,  --verbose                                                
           verbose mode (more information),                         
                                                                    
      -w [INT],  --width [INT]                                      
           horizontal width (thickness) of each bar,                
                                                                    
      -s [INT],  --space [INT]                                      
           space between each bar,                                  
                                                                    
      -e [INT],  --enlarge [INT]                                    
           enlargement of each region (increase visibility),        
                                                                    
      -c,  --strict-corner                                          
           it paints each bar with strict corners (NO round),       
                                                                    
      -b [STRING],  --back-color [STRING]                           
           background color in RGB format (example: FFFFFF),        
                                                                    
      -a [STRING],  --border-color [STRING]                         
           bar border color in RGB format (example: 000000),        
                                                                    
      -o [FILE],  --output [FILE]                                   
           filename of the SVG output map,                          
                                                                    
      [FILE]:[FILE]:...                                             
           input position filenames (to analyze) -- MANDATORY,      
           multiple files can be used with : split token.           
                                                                    
SYNOPSIS                                                            
      AlcoR visual [OPTION]... [FILE]:...                   
                                                                    
EXAMPLE                                                             
      AlcoR visual -v -o map.svg pos1.txt:pos2.txt:pos3.txt 
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

