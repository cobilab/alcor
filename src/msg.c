#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include "keys.h"
#include "colors.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintWarning(char *s)
  {
  fprintf(stderr, "%s[x] Error: %s%s\n", error_color, s, normal_color);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintMessage(char *s)
  {
  fprintf(stderr, "%s[>] %s%s\n", normal_color, s, normal_color);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintVersion(void)
  {
  fprintf(stdout, "v%u.%u\n", VERSION, RELEASE);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintMenu(void)
  {
  fprintf(stderr,
  "                                                        \n"
  "Program: %s [Alignment-free Computation of Redundant regions]\n" 
  "Version: %u.%u                                          \n"
  "                                                        \n"
  "Usage: %s <command> [options]                           \n"
  "                                                        \n"
  "Commands:                                              \n"
  "      %-10s   Extracts a sequence of a FASTA file using  \n" 
  "                   positional coordinates (independent from\n"
  "                   the existing headers of the FASTA files).\n"
  "      %-10s   Computes the redundancy of a FASTA read while\n"
  "                   providing bidirectional complexity profiles \n"
  "                   and further structural similarity analysis.\n" 
  "      %-10s   FASTA sequence simulation with features: \n"
  "                   file extraction, random generation, sequence\n"
  "                   modeling (with SNPs specific mutations).\n"
  "      %-10s   Computes an SVG file with the respective \n"
  "                   map containning the redundant regions.\n"
  "                                                      \n"
  "Help: %s <command> -h for accessing each command menu.\n"
  "                                                      \n",
  PNAME, VERSION, RELEASE, PNAME, LT_KEYS[1].key, LT_KEYS[2].key, 
  LT_KEYS[3].key, LT_KEYS[4].key, PNAME);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintModels(void){
  fprintf(stderr,
  "                                                               \n"
  "-m [C]:[D]:[R]:[I]:[H]:[G]/[S]:[E]:[A]                         \n"
  "                                                               \n"
  "Template of a target context model.                            \n"
  "                                                               \n"
  "Parameters:                                                    \n"
  "     [C]: (integer [1;20]) order size of the regular context   \n"
  "          model. Higher values use more RAM but, usually, are  \n"
  "          related to a better compression score.               \n"
  "     [D]: (integer [1;5000]) denominator to build alpha, which \n"
  "          is a parameter estimator. Alpha is given by 1/[D].   \n"
  "          Higher values are usually used with higher [C],      \n"
  "          and related to confiant bets. When [D] is one,       \n"
  "          the probabilities assume a Laplacian distribution.   \n"
  "     [R]: (integer [0;99999999] memory model. The 0 uses the   \n"
  "          full memory.                                         \n"
  "     [I]: (integer {0,1,2}) number to define if a sub-program  \n"
  "          which addresses the specific properties of DNA       \n"
  "          sequences (Inverted repeats) is used or not. The     \n"
  "          number 2 turns ON this sub-program without the       \n"
  "          regular context model (only inverted repeats). The   \n"
  "          number 1 turns ON the sub-program using at the same  \n"
  "          time the regular context model. The number 0 does    \n"
  "          not contemple its use (Inverted repeats OFF). The    \n"
  "          use of this sub-program increases the necessary time \n"
  "          to compress but it does not affect the RAM.          \n"
  "     [H]: (integer [1;254]) size of the cache-hash for deeper  \n"
  "          context models, namely for [C] > 14. When the        \n"
  "          [C] <= 14 use, for example, 1 as a default. The      \n"
  "          RAM is highly dependent of this value (higher value  \n"
  "          stand for higher RAM).                               \n"
  "     [G]: (real [0;1)) real number to define gamma. This value \n"
  "          represents the decayment forgetting factor of the    \n"
  "          regular context model in definition.                 \n"
  "     [S]: (integer [0;20]) maximum number of editions allowed  \n"
  "          to use a substitutional tolerant model with the same \n"
  "          memory model of the regular context model with       \n"
  "          order size equal to [C]. The value 0 stands for      \n"
  "          turning the tolerant context model off. When the     \n"
  "          model is on, it pauses when the number of editions   \n"
  "          is higher that [C], while it is turned on when       \n"
  "          a complete match of size [C] is seen again. This     \n"
  "          is probabilistic-algorithmic model very usefull to   \n"
  "          handle the high substitutional nature of genomic     \n"
  "          sequences. When [S] > 0, the compressor used more    \n"
  "          processing time, but uses the same RAM and, usually, \n"
  "          achieves a substantial higher compression ratio. The \n"
  "          impact of this model is usually only noticed for     \n"
  "          [C] >= 14.                                           \n"
  "     [E]: (integer [1;5000]) denominator to build alpha for    \n"
  "          substitutional tolerant context model. It is         \n"
  "          analogous to [D], however to be only used in the     \n"
  "          probabilistic model for computing the statistics of  \n"
  "          the substitutional tolerant context model.           \n"
  "     [A]: (real [0;1)) real number to define gamma. This value \n"
  "          represents the decayment forgetting factor of the    \n"
  "          substitutional tolerant context model in definition. \n"
  "          Its definition and use is analogus to [G].           \n"
  "                                                               \n");
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintMenuLR(void){
  fprintf(stderr,
  "NAME                                                                \n"
  "      %s %s                                        \n"
  "                                                                    \n"
  "DESCRIPTION                                                         \n"
  "      Computes the redundant regions of a sequence (FASTA).         \n"
  "                                                                    \n"
  "PARAMETERS                                                          \n"
  "                                                                    \n"
  "      -h,  --help                                                   \n"
  "           usage guide (help menu),                                 \n"
  "                                                                    \n"
  "      -v,  --verbose                                                \n"
  "           verbose mode (more information),                         \n"
  "                                                                    \n"
  "      -d,  --dna                                                    \n"
  "           considers exclusively DNA alphabet {A,C,G,T},            \n"
  "           it also provides inverted repeats models,                \n"
  "           flag absence considers inversions (without complements), \n"
  "                                                                    \n"
  "      -t [FLOAT],  --threshold [FLOAT]                              \n"
  "           threshold to segment regions (real),                     \n"
  "                                                                    \n"
  "      -w [FLOAT],  --weight [FLOAT]                                 \n"
  "           weight to use in low-pass filter (real),                 \n"
  "                                                                    \n"
  "      -i [INT],  --ignore [INT]                                     \n"
  "           ignore lengths of segmented regions below this value,    \n"
  "                                                                    \n"
  "      -p,  --show-parameters                                        \n"
  "           show parameters of the models for optimization,          \n"
  "                                                                    \n"
  "      -s,  --show-levels                                            \n"
  "           show pre-computed compression levels (parameters),       \n"
  "                                                                    \n"
  "      -l [INT],  --level [INT]                                      \n"
  "           compression level (integer),                             \n"
  "           it defines compressibility in balance with computational \n"
  "           resources (RAM & time), use -s for levels perception,    \n"
  "                                                                    \n"
  "      [FILE]                                                        \n"
  "           input sequence filename (to analyze) -- MANDATORY,       \n"
  "           FASTA file for the analysis (last argument).             \n"
  "                                                                    \n"
  "SYNOPSIS                                                            \n"
  "      %s %s [OPTION]... [FILE]                         \n"
  "                                                                    \n"
  "EXAMPLE                                                             \n"
  "      %s %s -v -w 0.03 -m 11:50:0:1:0:0.9/0:0:0 seq.fa \n"
  "                                                                    \n",
  PNAME, LT_KEYS[2].key, PNAME, LT_KEYS[2].key, PNAME, LT_KEYS[2].key);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintMenuSi(void){
  fprintf(stderr,
  "NAME                                                                \n"
  "      %s %s                                              \n"
  "                                                                    \n"
  "DESCRIPTION                                                         \n"
  "      Simulation of FASTA sequences with specific features.         \n"
  "                                                                    \n"
  "PARAMETERS                                                          \n"
  "                                                                    \n"
  "      -h,  --help                                                   \n"
  "           usage guide (help menu),                                 \n"
  "                                                                    \n"
  "      -v,  --verbose                                                \n"
  "           verbose mode (more information),                         \n"
  "                                                                    \n"
  "      -n,  --no-dna                                                 \n"
  "           Does not consider a DNA alphabet {A,C,G,T},              \n"
  "           it provides possible inversions without complements,     \n"
  "                                                                    \n"
  "      -a [STRING],  --alphabet [STRING]                             \n"
  "           alphabet to consider (Default: ACGT),                    \n"
  "                                                                    \n"
  "      -fs [FEATURES],  --file-segment [FEATURES]                    \n"
  "           FASTA file segment features:                             \n"
  "           |    [init:end:ir:seed:subs:adds:dels:file]              \n"
  "           |    ...                                                 \n"
  "           init [INT]   - initial position of the segment,          \n"
  "           end  [INT]   - ending position of the segment,           \n"
  "           ir   [INT]   - segment inverted if 1, otherwise 0,       \n"
  "           seed [INT]   - initial number for random generation,     \n"
  "           subs [FLOAT] - probability of substitution mutation,     \n"
  "           adds [FLOAT] - probability of addition mutation,         \n"
  "           dels [FLOAT] - probability of deletion mutation,         \n"
  "           file [FILE]  - FASTA filename for extracting segment,    \n"
  "                                                                    \n"
  "      -rs [FEATURES],  --rand-segment [FEATURES]                    \n"
  "           Random segment features:                                 \n"
  "           |    [size:ir:seed:subs:adds:dels]                       \n"
  "           |    ...                                                 \n"
  "           size [INT]   - length of the segment,                    \n"
  "           ir   [INT]   - segment inverted if 1, otherwise 0,       \n"
  "           seed [INT]   - initial number for random generation,     \n"
  "           subs [FLOAT] - probability of substitution mutation,     \n"
  "           adds [FLOAT] - probability of addition mutation,         \n"
  "           dels [FLOAT] - probability of deletion mutation,         \n"
  "                                                                    \n"
  "      -ms [FEATURES],  --model-segment [FEATURES]                   \n"
  "           Model segment features:                                  \n"
  "           |    [size:ctx:bet:ir:seed:subs:adds:dels:file]          \n"
  "           |    ...                                                 \n"
  "           size [INT]   - length of the segment,                    \n"
  "           ctx  [INT]   - context to model and simulate,            \n"
  "           bet  [INT]   - intensity for higher bet strength,        \n"
  "           ir   [INT]   - segment inverted if 1, otherwise 0,       \n"
  "           seed [INT]   - initial number for random generation,     \n"
  "           subs [FLOAT] - probability of substitution mutation,     \n"
  "           adds [FLOAT] - probability of addition mutation,         \n"
  "           dels [FLOAT] - probability of deletion mutation,         \n"
  "           file [FILE]  - FASTA filename for learning model,        \n"
  "                                                                    \n"
  "SYNOPSIS                                                            \n"
  "      %s %s [OPTION]... > output.fa      \n"
  "                                                                    \n"
  "EXAMPLE                                                             \n"
  "      %s %s -rs 50:0:1:0.1:0:0 -ms 80:7:50:0:7:0:0:0:x.fa\n"
  "                                                                    \n",
  PNAME, LT_KEYS[3].key, PNAME, LT_KEYS[3].key, PNAME, LT_KEYS[3].key);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
