APUE_DIR = "../../../apue.3e"

all:main.out

main.out:main.c
	gcc -o main.out main.c -I $(APUE_DIR)/include/ -L $(APUE_DIR)/lib/ -lapue
clean:
	rm -fr ./*.out
test:1K_out.tmp
1K.tmp:
	dd if=/dev/zero of=1K.tmp bs=1K count=1
1K_out.tmp:1K.tmp
	./main.out < 1K.tmp >1K_out.tmp
