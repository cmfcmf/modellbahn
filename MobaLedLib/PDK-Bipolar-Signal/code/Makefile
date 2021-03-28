SDCC=sdcc
SPDK=spdk
INCLUDE=-I./includes
EASYPDKPROG=~/dev/easy-pdk-programmer-software/easypdkprog

all: build write start

build: build/main.ihx

build/main.ihx: main.c
	mkdir -p build
	$(SDCC) -mpdk15 --std-sdcc11 --opt-code-speed -DPFS173 -DF_CPU=8000000 $(INCLUDE) -o build/ main.c
	bash fix-address.sh
	$(SDCC) -mpdk15 --std-sdcc11 --opt-code-speed -DPFS173 -DF_CPU=8000000 $(INCLUDE) -o build/ main.c

write: build/main.ihx
	$(EASYPDKPROG) --icname=PFS173 --serial=0x$(shell printf '%x\n' $(shell date +%s)) write build/main.ihx

start:
	$(EASYPDKPROG) --runvdd=5.0 start

erase:
	$(EASYPDKPROG) --icname=PFS173 erase

clean:
	rm -rf build

.PHONY: all write start erase clean
