default:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o slave.o slave.c
	avr-gcc -mmcu=atmega328p -o slave.bin slave.o
	avr-objcopy -O ihex -R .eeprom slave.bin slave.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 115200 -U flash:w:slave.hex
