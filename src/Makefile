#============================================================================#
#
#	AlcoR 2023 IEETA/DETI, UNIVERSITY OF AVEIRO
#
#============================================================================#
BIN    = .
CC     = gcc
CPLP   =
#-----------------------------------------------------------------------------
CFLAGS = -Wall -Ofast -march=native -mtune=native $(CPLP)
#-----------------------------------------------------------------------------
LIBS   = -lm
DEPS   = defs.h mem.h
ALCOR_DEPS = keys.h msg.h dna.h param.h common.h levels.h strings.h lr.h si.h ex.h vi.h
ALHABET_DEPS = common.h
CM_DEPS = dna.h msg.h common.h alphabet.h tolerant.h rand.h carray.h buffer.h pmodels.h chash.h
COMMON_DEPS = msg.h 
DIST_DEPS = common.h alphabet.h
DNA_DEPS = common.h msg.h 
EX_DEPS = dna.h msg.h buffer.h common.h strings.h param.h
KMER_DEPS = common.h dna.h 
LEVELS_DEPS = msg.h
LR_DEPS = threads.h msg.h dna.h buffer.h levels.h param.h alphabet.h common.h cache.h positions.h pmodels.h strings.h cm.h
PAINT_DEPS = common.h msg.h 
PARAM_DEPS = common.h msg.h dna.h strings.h keys.h alphabet.h
PMODELS_DEPS = math.h common.h
SI_DEPS = cm.h dna.h msg.h rand.h buffer.h common.h strings.h param.h
STRINGS_DEPS = msg.h
TOLERANT_DEPS = strings.h pmodels.h buffer.h
VI_DEPS = dna.h msg.h buffer.h common.h strings.h param.h paint.h

PROG = $(BIN)/AlcoR
OBJS = mem.o positions.o stats.o rand.o math.o buffer.o chash.o carray.o \
		cache.o filter.o keys.o msg.o strings.o common.o alphabet.o dist.o \
		dna.o kmer.o levels.o ex.o paint.o param.o pmodels.o tolerant.o cm.o vi.o lr.o si.o
#-----------------------------------------------------------------------------

all:
	$(MAKE) progs

progs: $(PROG)
$(BIN)/AlcoR: alcor.c $(DEPS) $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN)/AlcoR alcor.c $(OBJS) $(LIBS)

alphabet.o: alphabet.c alphabet.h $(DEPS) $(ALHABET_DEPS)
	$(CC) -c $(CFLAGS) alphabet.c
buffer.o: buffer.c buffer.h $(DEPS)
	$(CC) -c $(CFLAGS) buffer.c
cache.o: cache.c cache.h $(DEPS)
	$(CC) -c $(CFLAGS) cache.c
carray.o: carray.c carray.h $(DEPS)
	$(CC) -c $(CFLAGS) carray.c
chash.o: chash.c chash.h $(DEPS)
	$(CC) -c $(CFLAGS) chash.c
cm.o: cm.c cm.h $(DEPS) $(CM_DEPS)
	$(CC) -c $(CFLAGS) cm.c
common.o: common.c common.h $(DEPS) $(COMMON_DEPS)
	$(CC) -c $(CFLAGS) common.c
dna.o: dna.c dna.h $(DEPS) $(DNA_DEPS)
	$(CC) -c $(CFLAGS) dna.c
dist.o: dist.c dist.h $(DEPS) $(DIST_DEPS)
	$(CC) -c $(CFLAGS) dist.c
ex.o: ex.c ex.h $(DEPS) $(EX_DEPS)
	$(CC) -c $(CFLAGS) ex.c
filter.o: filter.c filter.h $(DEPS)
	$(CC) -c $(CFLAGS) filter.c
keys.o: keys.c keys.h $(DEPS)
	$(CC) -c $(CFLAGS) keys.c
kmer.o: kmer.c kmer.h $(DEPS) $(KMER_DEPS)
	$(CC) -c $(CFLAGS) kmer.c
levels.o: levels.c levels.h $(DEPS) $(LEVELS_DEPS)
	$(CC) -c $(CFLAGS) levels.c
lr.o: lr.c lr.h threads.h $(DEPS) $(LR_DEPS)
	$(CC) -c $(CFLAGS) lr.c
math.o: math.c math.h $(DEPS)
	$(CC) -c $(CFLAGS) math.c
mem.o: mem.c mem.h $(DEPS)
	$(CC) -c $(CFLAGS) mem.c
msg.o: msg.c msg.h colors.h keys.h $(DEPS)
	$(CC) -c $(CFLAGS) msg.c
paint.o: paint.c paint.h $(DEPS) $(PAINT_DEPS)
	$(CC) -c $(CFLAGS) paint.c
param.o: param.c param.h $(DEPS) $(PARAM_DEPS)
	$(CC) -c $(CFLAGS) param.c
pmodels.o: pmodels.c pmodels.h $(DEPS) $(PMODELS_DEPS)
	$(CC) -c $(CFLAGS) pmodels.c
positions.o: positions.c positions.h $(DEPS)
	$(CC) -c $(CFLAGS) positions.c
rand.o: rand.c rand.h $(DEPS)
	$(CC) -c $(CFLAGS) rand.c
si.o: si.c si.h $(DEPS) $(SI_DEPS)
	$(CC) -c $(CFLAGS) si.c
stats.o: stats.c stats.h $(DEPS)
	$(CC) -c $(CFLAGS) stats.c
strings.o: strings.c strings.h $(DEPS) $(STRINGS_DEPS)
	$(CC) -c $(CFLAGS) strings.c
tolerant.o: tolerant.c tolerant.h $(DEPS) $(TOLERANT_DEPS)
	$(CC) -c $(CFLAGS) tolerant.c
vi.o: vi.c vi.h $(DEPS) $(VI_DEPS)
	$(CC) -c $(CFLAGS) vi.c

clean:
	/bin/rm -f *.o
cleanall:
	/bin/rm -f *.o $(BIN)/AlcoR
#-----------------------------------------------------------------------------

