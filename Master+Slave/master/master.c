#include <avr/io.h>
#include <stdint.h>
#include <util/twi.h>

void init_i2c(){
    //Configuring master mcu registers for 100kHz
    TWSR = 0x00;
    TWBR = 72;
    TWCR = (1 << TWEN);
}

void init_uart(){
    UBRR0 = 103;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void master_write_i2c(uint8_t addr, uint8_t command){

    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    while(!(TWCR & (1 << TWINT))){
        //wait for slave to recieve start
    }

    if((TWSR & 0xF8) != TW_START) {
        return;
    }

    TWDR = (addr << 1);

    TWCR = (1 << TWINT) | (1 << TWEN);

    while(!(TWCR & (1 << TWINT))){
        //wait for slave to recieve start
    }

    if((TWSR & 0xF8) != TW_MT_SLA_ACK) {
        return;
    }

    TWDR = command;

    TWCR = (1 << TWINT) | (1 << TWEN);

    while(!(TWCR & (1 << TWINT))){
        //wait for slave to recieve start
    }

    if((TWSR & 0xF8) != TW_MT_DATA_ACK) {
        return;
    }

    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

uint16_t master_recieve_i2c(uint8_t addr){

    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    while(!(TWCR & (1 << TWINT))){
        //wait for slave to receive restart
    }

    if((TWSR & 0xF8) != TW_START) {
        return;
    }

    TWDR = (addr << 1) | 1;

    TWCR = (1 << TWINT) | (1 << TWEN);

    while(!(TWCR & (1 << TWINT))){
        //wait for slave to receive restart
    }

    if((TWSR & 0xF8) != TW_MR_SLA_ACK) {
        return;
    }

    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

    while(!(TWCR & (1 << TWINT))){
        //wait for slave to receive restart
    }

    if((TWSR & 0xF8) != TW_MR_DATA_ACK) {
        return;
    }

    uint8_t data_low = TWDR;

    TWCR = (1 << TWINT) | (1 << TWEN);

    while(!(TWCR & (1 << TWINT))){
        //wait for slave to receive restart
    }

    if((TWSR & 0xF8) != TW_MR_DATA_NACK) {
        return;
    }

    uint8_t data_high = TWDR;

    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

    uint16_t data_adc = ((uint16_t) data_high << 8) | data_low;

    return data_adc;
}

void uart_send_number(uint16_t value){
    char buffer[5];
    uint8_t i = 0;

    if (value == 0)
    {
        while (!(UCSR0A & (1 << UDRE0))) {}
        UDR0 = '0';
        return;
    }

    while (value > 0)
    {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }

    while (i > 0)
    {
        while (!(UCSR0A & (1 << UDRE0))) {}
        UDR0 = buffer[--i];
    }
}

int main(void){

    init_i2c();

    init_uart();

    while(1){

        master_write_i2c(0x18, 0x24);

        uint16_t data = master_recieve_i2c(0x18);

        uart_send_number(data);

    }

}