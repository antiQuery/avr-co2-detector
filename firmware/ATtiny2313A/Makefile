#
# Makefile
#
# AVR CO2 detector
#

OBJECTS = main.o LiquidCrystal/src/LiquidCrystal.o ATTinyCore/avr/cores/tiny/wiring_digital.o ATTinyCore/avr/cores/tiny/Print.o

CC = avr-g++

CFLAGS= -Os -mmcu=attiny2313 -D__AVR_ATtiny2313__ -DF_CPU=4000000UL -IATTinyCore/avr/cores/tiny -IATTinyCore/avr/variants/tinyx313 -ILiquidCrystal/src -ffunction-sections -fdata-sections
LDFLAGS = -mmcu=attiny2313 -Wl,-Map,output.map -Wl,--gc-sections

TARGET=ATtiny2313-SU.elf

.PHONY: clean

all: $(TARGET) ATtiny2313-SU.hex

$(TARGET): $(OBJECTS)
	$(CC) -s -o $(TARGET) $^ $(LDFLAGS)

%.o : %.cpp $(INCLUDES)
	$(CC) -c $(CFLAGS) -o $@ $<

%.hex: $(TARGET)
	avr-objcopy -O ihex $< $@

program-avrdude:
	avrdude -c usbasp -p t2313 -U lfuse:w:0xdd:m -U flash:w:ATtiny2313-SU.hex

clean:
	$(RM) -rf $(TARGET) $(OBJECTS) *.map *.hex
