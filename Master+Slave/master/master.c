#include <avr/io.h>
#include <stdint.h>

int main(void){

    uint8_t adcl_value;
    uint8_t adch_value;
    uint16_t adc_value;
    
    //Configuring master mcu registers for 100kHz
    TWSR = 0x00;
    TWBR = 72;
    TWCR = (1 << TWEN);

    while(1){
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

        while(!(TWCR & (1 << TWINT))){
            //wait for slave to recieve start
        }

        uint8_t status = TWSR & 0xF8;

        if (status == 0x08){

            TWDR = (0x18 << 1) | 0;  //address of the salve + write command
            TWCR = (1 << TWINT) | (1 << TWEN);

            while(!(TWCR & (1 << TWINT))){
                //wait for slave to return address
            }

            status = TWSR & 0xF8;

            if(status == 0x18){
                //Slave returned address

                TWDR = 0x24;
                TWCR = (1 << TWINT) | (1 << TWEN);

                while(!(TWCR & (1 << TWINT))){
                    //wait for slave to transmit command
                }

                status = TWSR & 0xF8;

                if(status == 0x28){
                    //Slave recieved command

                    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

                    while(!(TWCR & (1 << TWINT))){
                        //wait for slave to receive restart
                    }

                    status = TWSR & 0xF8;

                    if (status == 0x10){
                        
                        TWDR = (0x18 << 1) | 1; //Slave address + read command

                        TWCR = (1 << TWINT) | (1 << TWEN);

                        while(!(TWCR & (1 << TWINT))){
                            //wait for slave to transmit address
                        }

                        status = TWSR & 0xF8;

                        if (status == 0x40){
                            //Slave recieved the read requests

                            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

                            while(!(TWCR & (1 << TWINT))){
                                //wait for slave to send adc low byte
                            }

                            status = TWSR & 0xF8;

                            if(status == 0x50) {
                                //Slave recieved low byte for adc data

                                adcl_value = TWDR;

                                TWCR = (1 << TWINT) | (1 << TWEN);

                                while(!(TWCR & (1 << TWINT))){
                                    //wait for slave to send adc high byte
                                }

                                status = TWSR & 0xF8;

                                if(status == 0x58) {
                                    adch_value = TWDR;

                                    adc_value = ((uint16_t)adch_value << 8) | adcl_value;
                                }
                            }

                        }
                    }

                }
            }
        }
    }
}