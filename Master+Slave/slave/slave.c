#include <avr/io.h>
#include <stdint.h>

void uart_send_number(uint16_t value)
{
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

    uint8_t start_conversion = (1 << 6);
    ADMUX = 0x40;
    UBRR0 = 103;
    UCSR0B = (1 << 3);
    UCSR0C = (1 << 2) | (1 << 1);

    while(1){

        ADCSRA = 0x87;
        ADCSRA = ADCSRA | start_conversion;

        while((ADCSRA & start_conversion) == start_conversion){
            //do nothing
            //run until we are done converting data
        }

        uint16_t adcl_value = ADCL;
        uint16_t adch_value = ADCH;

        uint16_t adc_value = (adch_value << 8) | adcl_value;

        uart_send_number(adc_value);
    }
}