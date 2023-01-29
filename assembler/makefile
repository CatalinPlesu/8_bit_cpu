SRC = main
BUILD = asm

FOLDERS={help,preprocessor,error}
#DEPS = $(FOLDERS)/*.c
DEPS = ./help/help.c ./preprocessor/preprocessor.c ./error/error.c
#INCLUDE = -I{help,preprocessor,error}
INCLUDE = -Ihelp -Ipreprocessor -Ierror

all: compile test

compile:
	echo $(INCLUDE)
	gcc $(CFLAGS) $(INCLUDE) -o $(BUILD) $(SRC).c $(DEPS)

test:
	./asm test.asm
print:
	cat preprocessed.asm
clean:
	rm $(BUILD)
