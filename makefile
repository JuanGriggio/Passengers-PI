COMPILER=gcc
OUTPUT=tpe_Griggio
SOURCE_FILES=main.c linesADT.c
ARGUMENTS=-pedantic -std=c99 -Wall -fsanitize=address -g


all:
	$(COMPILER) $(SOURCE_FILES) -o $(OUTPUT) $(ARGUMENTS)