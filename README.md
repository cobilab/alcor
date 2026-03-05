<!-- README.md -->

<div align="center">

<!-- Badges -->
![Conda](https://img.shields.io/conda/dn/bioconda/alcor?style=flat-square&logo=anaconda&logoColor=white)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg?style=flat-square&logo=gnu&logoColor=white)](LICENSE)
![Method](https://img.shields.io/static/v1?label=Method&message=Alignment-free&color=yellow&style=flat-square)
![Testing](https://img.shields.io/static/v1?label=Testing&message=High-speed&color=green&style=flat-square)
![Testing](https://img.shields.io/static/v1?label=Testing&message=High-sensitivity&color=blue&style=flat-square)
![Testing](https://img.shields.io/static/v1?label=Testing&message=No%20false%20positives&color=orange&style=flat-square)

</div>

<p align="center">
  <img src="imgs/logo_trans.png" alt="AlcoR" width="260" />
</p>

<p align="center">
  <b>🧬 Alignment-free simulation, mapping, and visualization<br/> of low-complexity regions (LCRs) in FASTA data</b>
  <br/>
  <a href="https://cobilab.github.io/alcor"><b>cobilab.github.io/alcor</b></a>
</p>

---

## ✨ What is AlcoR?

**AlcoR** is a fast, alignment-free toolkit to **characterize, simulate, map, and visualize low-complexity regions (LCRs)** in biological sequences (FASTA).

It is designed for scenarios where sequences may be **novel, unknown, or difficult to align**, and for workflows that benefit from rapid characterization (e.g., **Telomere-to-Telomere (T2T)** sequencing and assembly).

### ✅ Highlights
- ⚡ **High speed** (multi-threaded **C** implementation)
- 🔍 **High sensitivity** LCR detection (bidirectional profiles + structural similarity analysis)
- 🎯 **No false positives** (as reported by the project)
- 🧩 **No external dependencies**
- 📄 Works with **any FASTA** input

---

## 🧭 Contents

- [▶️ Tutorial video](#️-tutorial-video)
- [🧰 Commands](#-commands)
- [⚙️ Installation](#️-installation)
- [🚀 Quickstart](#-quickstart)
- [🧾 Help and parameters](#-help-and-parameters)
- [📚 Detailed CLI reference](#-detailed-cli-reference)
- [📝 Citation](#-citation)
- [🐛 Issues](#-issues)
- [📜 License](#-license)

---

## ▶️ Tutorial video

GitHub Markdown rendering is inconsistent for embedded `<video>` across platforms.

🎬 **Direct tutorial link (MP4):**  
https://cobilab.github.io/alcor/videos/AlcoR_tutorial.mp4

---

## 🧰 Commands

AlcoR provides a single entry point (`AlcoR`) with five subcommands:

- 🧾 **`info`** — report length and GC% for each FASTA record  
- ✂️ **`extract`** — extract a sequence region from a FASTA file using coordinates (independent of FASTA headers)  
- 🗺️ **`mapper`** — detect LCRs and produce bidirectional complexity profiles + structural similarity analysis  
- 🧪 **`simulation`** — simulate FASTA sequences (file extraction, random generation, model-based generation) with optional SNP/indel probabilities  
- 🎨 **`visual`** — generate an **SVG** map of LCRs from mapper outputs  

---

## ⚙️ Installation

### 🟩 Option A — Conda (recommended)

Install Miniconda (or Mambaforge), then:

```bash
conda install -y -c bioconda alcor
````

### 🐧 Option B — Precompiled binary (Linux)

A precompiled Linux binary is available in `bin/`.

```bash
cd bin/
chmod +x AlcoR
./AlcoR -h
```

**MD5 (Linux binary):** `159c777ffb2010be81cf2eafa01ee338`

Verify it (optional):

```bash
md5sum bin/AlcoR
```

### 🏗️ Option C — Build from source (CMake)

Requirements: `cmake`, `git`, and a C compiler toolchain.

```bash
sudo apt-get install -y cmake git build-essential
git clone https://github.com/cobilab/alcor.git
cd alcor
cmake -S src -B build
cmake --build build -j
./build/AlcoR -h
```

---

## 🚀 Quickstart

### 1) 🧪 Simulate a FASTA file

```bash
AlcoR simulation \
  -rs 2000:0:1:0:0:0 \
  -rs 2000:0:11:0:0:0 \
  -rs 2000:0:1:0:0:0 \
  -rs 2000:0:71:0:0:0 \
  > sample.fasta
```

### 2) 🗺️ Map low-complexity regions in a FASTA file

```bash
AlcoR mapper -v -n -m 13:50:0:1:10:0.9/5:10:0.9 --dna -w 3 -t 0.5 sample.fasta
```
### 2) 🗺️ Map and visualize low-complexity regions in a haplotype-resolved genome

This script, for a human haplotype-resolved genome, assumes that there is a fasta sequence for each human chromosome for each haplotype (for example, HG-C1-P.fa, HG-C1-M.fa, HG-C2-P.fa, HG-C2-M.fa, ..., HG-C23-M.fa, HG-C23-P.fa -- where HG-C23-M.fa and HG-C23-P.fa stands for CX and CY, respectively):
```bash
#!/bin/bash
THRESHOLD="0.6";
PARAM_DISTA=" -m 13:50:0:1:10:0.9/3:10:0.9 ";
PARAM_LOCAL=" -m 13:50:5000:1:10:0.9/3:10:0.9 ";
MIN_LEN=" --ignore 5000 ";
WINDOW=" -w 5000 ";
#
for((x=1;x<=23;++x));
  do
  ./AlcoR mapper -v --hide --color 100 --threshold $THRESHOLD --dna $MIN_LEN $WINDOW $PARAM_DISTA HG-C${x}-P.fa > p-c-d-$x.txt
  ./AlcoR mapper -v --hide --color 100 --threshold $THRESHOLD --dna $MIN_LEN $WINDOW $PARAM_DISTA HG-C${x}-M.fa > m-c-d-$x.txt
  ./AlcoR mapper -v --no-size --hide --threshold $THRESHOLD --color 1 --dna $MIN_LEN $WINDOW $PARAM_LOCAL HG-C${x}-P.fa > p-c-l-$x.txt
  ./AlcoR mapper -v --no-size --hide --threshold $THRESHOLD --color 1 --dna $MIN_LEN $WINDOW $PARAM_LOCAL HG-C${x}-M.fa > m-c-l-$x.txt
  done
#
for((x=1;x<=23;++x));
  do
  cat p-c-d-$x.txt p-c-l-$x.txt > p-$x.txt;
  cat m-c-d-$x.txt m-c-l-$x.txt > m-$x.txt;
  done
#
./AlcoR visual -o map.svg -s 6 -w 18 -e 0 p-1.txt:m-1.txt:p-2.txt:m-2.txt:p-3.txt:m-3.txt:p-4.txt:m-4.txt:p-5.txt:m-5.txt:p-6.txt:m-6.txt:p-7.txt:m-7.txt:p-8.txt:m-8.txt:p-9.txt:m-9.txt:p-10.txt:m-10.txt:p-11.txt:m-11.txt:p-12.txt:m-12.txt:p-13.txt:m-13.txt:p-14.txt:m-14.txt:p-15.txt:m-15.txt:p-16.txt:m-16.txt:p-17.txt:m-17.txt:p-18.txt:m-18.txt:p-19.txt:m-19.txt:p-20.txt:m-20.txt:p-21.txt:m-21.txt:p-22.txt:m-22.txt:m-23.txt:p-23.txt

```

### 4) 🧼 Mask a FASTA file

This example **detects LCRs** and writes a **masked FASTA** where LCR bases are converted to **lowercase**.

```bash
# Input: seq.fa
# Output: masked.fa (LCRs in lowercase)
AlcoR mapper \
  --dna \
  -m 13:50:0:1:10:0.9/5:10:0.9 \
  -w 3 \
  -t 0.5 \
  -k \
  -o masked.fa \
  seq.fa
```

---

## 🧾 Help and parameters

Top-level help:

```bash
AlcoR
# or
AlcoR -h
```

Per-subcommand help:

```bash
AlcoR info -h
AlcoR extract -h
AlcoR mapper -h
AlcoR simulation -h
AlcoR visual -h
```

---

## 📚 Detailed CLI reference

The full built-in help for each command is included below for convenience.

<details>
<summary><b>🧾 AlcoR info</b></summary>

```text
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
```

</details>

<details>
<summary><b>✂️ AlcoR extract</b></summary>

```text
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
```

</details>

<details>
<summary><b>🗺️ AlcoR mapper</b></summary>

```text
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
```

</details>

<details>
<summary><b>🧪 AlcoR simulation</b></summary>

```text
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
```

</details>

<details>
<summary><b>🎨 AlcoR visual</b></summary>

```text
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
```

</details>

---

## 📝 Citation

If you use AlcoR in academic work, please cite:

* J. M. Silva, W. Qi, A. J. Pinho, D. Pratas. **AlcoR: alignment-free simulation, mapping, and visualization of low-complexity regions in biological data.** *GigaScience*, Volume 12, 2023. [https://doi.org/10.1093/gigascience/giad101](https://doi.org/10.1093/gigascience/giad101)

---

## 🐛 Issues

Please report bugs and feature requests via GitHub Issues:
[https://github.com/cobilab/alcor/issues](https://github.com/cobilab/alcor/issues)

---

## 📜 License

This project is licensed under **GPL v3**. See [`LICENSE`](LICENSE).
GNU GPL v3: [http://www.gnu.org/licenses/gpl-3.0.html](http://www.gnu.org/licenses/gpl-3.0.html)
