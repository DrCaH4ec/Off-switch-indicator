MCU=attiny13a
FILE=main

avr-gcc:
	avr-gcc -g -Os -mmcu=$(MCU) -c $(FILE).c
	avr-gcc -g -mmcu=$(MCU) -o $(FILE).elf $(FILE).o
	avr-objcopy -j .text -j .data -O ihex $(FILE).elf $(FILE).hex

avrdude:PROGRAMMER=arduino
avrdude:SPEED=57600
avrdude:PORT=COM8
avrdude:MEMTYPE=flash
avrdude:
	avrdude -p $(MCU) -b $(SPEED) -c $(PROGRAMMER) -P $(PORT) -U $(MEMTYPE):w:$(FILE).hex