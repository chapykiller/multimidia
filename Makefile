SRCDIR = src
OBJDIR = obj

PROGRAM1 = encode
PROGRAM2 = decode

CFLAGS = -I./include -g
lib= -lm
CXX = gcc

SRC1 = $(SRCDIR)/encode.c $(SRCDIR)/wave_reader.c $(SRCDIR)/bitvector.c $(SRCDIR)/huffman.c $(SRCDIR)/auxiliary.c $(SRCDIR)/runlength.c $(SRCDIR)/difference.c
OBJ1 = $(SRC1:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEP1 = $(OBJ1:.o=.d)

SRC2 = $(SRCDIR)/decode.c $(SRCDIR)/wave_reader.c $(SRCDIR)/bitvector.c $(SRCDIR)/huffman.c $(SRCDIR)/auxiliary.c $(SRCDIR)/runlength.c $(SRCDIR)/difference.c
OBJ2 = $(SRC2:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEP2 = $(OBJ2:.o=.d)

CFLAGS += -MMD -MP

.PHONY: all clean

all: bin/ bin/$(PROGRAM1) bin/$(PROGRAM2)

bin/: obj/
	mkdir -p bin/

obj/:
	mkdir -p obj/

bin/$(PROGRAM1): $(OBJ1)
	$(CXX) $^ -o $@ $(lib)

bin/$(PROGRAM2): $(OBJ2)
	$(CXX) $^ -o $@ $(lib)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CXX) $(CFLAGS) -o $@ -c $<

-include $(DEP)

clean:
	rm -f $(OBJDIR)/* bin/$(PROGRAM1) bin/$(PROGRAM2)
	rm -rf bin obj
