default:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o master.o master.c
	avr-gcc -mmcu=atmega328p -o master.bin master.o
	avr-objcopy -O ihex -R .eeprom master.bin master.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 115200 -U flash:w:master.hex
