#include <xc.h>
#include <stdio.h>
#include "clcd.h"

#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)

#define _XTAL_FREQ 20000000

void init_config(void) {
    init_clcd();
    // Initialize ADC
    ADCON0 = 0x41; // ADC enabled, select AN0 channel
    ADCON1 = 0x8E; // Right justify result, VDD as reference, AN0 as analog input
}

unsigned int read_adc(void) {
    __delay_us(20); // Acquisition time
    GO_nDONE = 1; // Start conversion
    while (GO_nDONE); // Wait for conversion to finish
    return ((unsigned int)(ADRESH << 8) + ADRESL); // Return result
}

void main(void) {
    char buffer[16];
    unsigned int adc_value;
    float voltage;

    init_config();

    // Display static text on the first line
    clcd_print("Lithium Power", LINE1(2));

    while (1) {
        adc_value = read_adc(); // Read ADC value
        voltage = ((float)adc_value * 5.0f) / 1023.0f; // Scale to 0-5V

        // Assuming a voltage divider that scales 0-100V to 0-5V
        voltage = voltage * 20.0f; // Scale 0-5V to 0-100V

        // Ensure the voltage is displayed in the range 0-100V
        if (voltage > 100.0f) {
            voltage = 100.0f;
        } else if (voltage < 0.0f) {
            voltage = 0.0f;
        }

        sprintf(buffer, "Voltage: %.2fV", voltage);
        clcd_print(buffer, LINE2(0));

        __delay_ms(500); // Delay for display update
    }
    return;
}
