SRCDIR = src
OBJDIR = obj

PROGRAM1 = encode
PROGRAM2 = decode

CFLAGS = -I./include
CXX = gcc

SRC1 = $(SRCDIR)/encode.c $(SRCDIR)/wave_reader.c
OBJ1 = $(SRC1:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEP1 = $(OBJ1:.o=.d)

SRC2 = $(SRCDIR)/decode.c $(SRCDIR)/wave_reader.c
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
	$(CXX) $^ -o $@

bin/$(PROGRAM2): $(OBJ2)
	$(CXX) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CXX) $(CFLAGS) -o $@ -c $<

-include $(DEP)

clean:
	rm -f $(OBJDIR)/* bin/$(PROGRAM1) bin/$(PROGRAM2)
	rm -rf bin obj