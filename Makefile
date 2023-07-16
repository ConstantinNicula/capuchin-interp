### OS SPECIFICS ###
CLEANUP=rm -f
MKDIR=mkdir -p

#.PHONY: clean
#.PHONY: test

### PATHS AND SOURCES ### 
PATHU = unity/src/
PATHS = src/
PATHT = test/

PATHB = build/
PATHD = build/depends/
PATHO = build/objs/
PATHR = build/results/

BUILD_PATHS = $(PATHB) $(PATHD) $(PATHO) $(PATHR)


# all obj files 
SRC = $(wildcard $(PATHS)*.c)
SRC_OBJ = $(patsubst $(PATHS)%.c, $(PATHO)%.o, $(SRC))

# tell compiler where to look for tests 
SRCT = $(wildcard $(PATHT)*.c)

### TOOLCHAIN SETUP ###
COMPILE=gcc -c
LINK=gcc
DEPEND=gcc -MM -MG -MF
CFLAGS=-I. -I$(PATHU) -I$(PATHS) -DTEST -Wall

RESULTS = $(patsubst $(PATHT)test_%.c, $(PATHR)test_%.txt, $(SRCT))


### RULES ### 
PASSED = `grep -s PASS $(PATHR)*.txt`
FAIL = `grep -s FAIL $(PATHR)*.txt`
IGNORE = `grep -s IGNORE $(PATHR)*.txt`

test: $(BUILD_PATHS) $(RESULTS)
	@echo "-----------------------\nIGNORES:\n-----------------------"
	@echo "$(IGNORE)"
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo "$(FAIL)"
	@echo "-----------------------\nPASSED:\n-----------------------"
	@echo "$(PASSED)"
	@echo "\nDONE"


$(PATHR)%.txt: $(PATHB)%.out
	-./$< > $@ 2>&1

#$(PATHB)test_%.out: $(PATHO)test_%.o $(PATHO)%.o $(PATHO)unity.o #$(PATHD)test_%.d
$(PATHB)test_%.out: $(PATHO)test_%.o $(SRC_OBJ) $(PATHO)unity.o 
	$(LINK) -o $@ $^

$(PATHO)%.o:: $(PATHT)%.c 
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o:: $(PATHS)%.c 
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o:: $(PATHU)%.c $(PATHU)%.h
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHD)%.d:: $(PATHT)%.c
	$(DEPEND) $@ $<

$(PATHB):
	$(MKDIR) $(PATHB)

$(PATHD):
	$(MKDIR) $(PATHD)

$(PATHO):
	$(MKDIR) $(PATHO)

$(PATHR):
	$(MKDIR) $(PATHR)


clean: 
	$(CLEANUP) $(PATHO)*.o
	$(CLEANUP) $(PATHB)*.out
	$(CLEANUP) $(PATHR)*.txt 

.PRECIOUS: $(PATHB)test_%.out
.PRECIOUS: $(PATHD)%.d 
.PRECIOUS: $(PATHO)%.o 
.PRECIOUS: $(PATHR)%.txt 
