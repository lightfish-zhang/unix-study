APUE_DIR = ../../../apue.3e
CC = gcc

all:main.out

main.out:main.c
	$(CC) -o main.out main.c -I $(APUE_DIR)/include/ -L $(APUE_DIR)/lib/ -lapue
clean:
	rm -fr ./*.out && rm -fr ./*.tmp