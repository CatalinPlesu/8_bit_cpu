SRC = main
BUILD = asm

FOLDERS={help,preprocessor,error}
DEPS = $(FOLDERS)/*.c
# DEPS =
INCLUDE = -I$(FOLDERS)
# INCLUDE = 

all: compile test

compile:
	gcc $(CFLAGS) $(INCLUDE) -o $(BUILD) $(SRC).c $(DEPS)

test:
	./asm test.asm
	bat preprocessed.asm
clean:
	rm $(BUILD)
