#include <avr/io.h>
#include <stdint.h>
#include <util/twi.h>

void init_i2c(uint8_t addr){
    TWAR = (addr << 1);
    TWSR = 0;
    TWCR = (1 << TWEN) | (1 << TWEA);
}

void init_adc(){;
    ADMUX = 0x40;
    ADCSRA = 0x87;
}

uint16_t read_adc(){
    uint8_t start_conversion = (1 << 6);

    ADCSRA = ADCSRA | start_conversion;

    while((ADCSRA & start_conversion) == start_conversion){
        //do nothing (wait)
        //run until we are done converting data
    }

    uint16_t = adcl_value = ADCL; //low byte of the ldr data
    uint16_t adch_value = (ADCH << 8); //high byte of the ldr data

    return (adch_value | adcl_value);
}

uint8_t receive_command(){
   return TWDR;
}

void send_adc(uint16_t data){
    uint8_t adc_high = data >> 8;
    uint8_t adc_low = data & 0x00FF;

    TWDR = adc_low;

    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

    while (!(TWCR & (1 << TWINT))){
        //wait for master mcu to send acknowledgement
    }

    if((TWSR & 0xF8) != TW_ST_DATA_ACK){
        return;
    }

    TWDR = adc_high;

    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

    while (!(TWCR & (1 << TWINT))){
        //wait for master mcu to send acknowledgement
    }

    if((TWSR & 0xF8) != TW_ST_DATA_NACK){
        return;
    }

    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}


int main(void){
    init_i2c(0x18);

    init_adc();

    uint8_t desired_command = 0x24;

    while(1){
        
        while(!(TWCR & (1 << TWINT))){}
        
        uint8_t command = receive_command();

        if(command == desired_command){
            uint16_t data = read_adc();
            send_adc(data);
        }
    }
}