APUE_DIR = "../../../apue.3e"

all:ls

ls:ls.c
	gcc -o ls.out ls.c -I $(APUE_DIR)/include/ -L $(APUE_DIR)/lib/ -lapue
clean:
	rm -fr ./ls
