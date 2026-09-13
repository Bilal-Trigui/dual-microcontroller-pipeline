#include <avr/io.h>
#include <stdint.h>

// //converts the ADC Light Data Number from the LDR into a string that can be put into UDR0
// void uart_send_number(uint16_t value)
// {
//     char buffer[5];
//     uint8_t i = 0;

//     if (value == 0)
//     {
//         while (!(UCSR0A & (1 << UDRE0))) {}
//         UDR0 = '0';
//         return;
//     }

//     while (value > 0)
//     {
//         buffer[i++] = (value % 10) + '0';
//         value /= 10;
//     }

//     while (i > 0)
//     {
//         while (!(UCSR0A & (1 << UDRE0))) {}
//         UDR0 = buffer[--i];
//     }
// }

int main(void){

    uint8_t adcl_value;
    uint8_t adch_value;

    uint8_t start_conversion = (1 << 6);
    ADMUX = 0x40;
    UBRR0 = 103;
    UCSR0B = (1 << 3);
    UCSR0C = (1 << 2) | (1 << 1);

    TWAR = (0x18 << 1);
    TWCR = (1 << TWEN) | (1 << TWEA);

    while(1){

        while(!(TWCR & (1 << TWINT))) {
            //wait for the master mcu to contact slave mcu
        }

        uint8_t status = TWSR & 0xF8;

        if(status == 0x60){
            //slave sends acknowlegdement to master

            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
        }

        while(!(TWCR & (1 << TWINT))) {
            //wait for the master mcu to send command to slave mcu
        }

        status = TWSR & 0xF8;

        if(status == 0x80){

            uint8_t command = TWDR;
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

            if(command == 0x24){
                //master mcu sent request to slave for LDR data

                ADCSRA = 0x87;
                ADCSRA = ADCSRA | start_conversion;

                while((ADCSRA & start_conversion) == start_conversion){
                    //do nothing (wait)
                    //run until we are done converting data
                }

                adcl_value = ADCL; //low byte of the ldr data
                adch_value = ADCH; //high byte of the ldr data

                while (!(TWCR & (1 << TWINT))){
                    //wait for master mcu to send read request
                }

                status = TWSR & 0xF8;

                if(status == 0xA8){

                    TWDR = adcl_value; //gives master the low bit of adc data

                    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

                    while (!(TWCR & (1 << TWINT))){
                        //wait for master mcu to send acknowledgement
                    }

                    status = TWSR & 0xF8;

                    if(status == 0xB8){
                        //master recieved low bit and now needs high bit
                        TWDR = adch_value;
                        TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
                    }

                    while (!(TWCR & (1 << TWINT))) {
                        // wait for Master to finish
                    }

                    status = TWSR & 0xF8;

                    if (status == 0xC0){
                        //master is done reading data
                        TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
                    }
                    
                }
            }
        }

        // uart_send_number(adc_value);
    }
}