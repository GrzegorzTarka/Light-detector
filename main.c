/*
 * main.c
 *
 *  Created on: 28 mar 2018
 *      Author: Grzesiek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "MKUART/mkuart.h"
#include "LCD/lcd44780.h"
#include <stdlib.h>

uint16_t adc_measurement(uint8_t adc_canal); // adc_canal = wartoúc od 0 do 7, odpowiada pinom portu A

int main (void)
{
	ADMUX 	|= (1<<REFS0)|(1<<REFS1); // WewnÍtrzne napiÍcie odniesienia Vref=2,56.
	ADCSRA	|= (1<<ADEN);			  // W≥πczenie ADC.
	ADCSRA	|= (1<<ADPS1)|(1<<ADPS2); // Ustawienie preskalera dla ADC na 64 (8MHz/64=125kHz). Wg. noty dla rozdzielczosci 10 bitowej ADC wymaga taktowania 50-200 kHz.

	USART_Init( __UBRR );			// inicjalizacja UART
	lcd_init();						// inicjalizacja LCD
	sei();							// globalne zezwolenie na przerwania, na potrzeby biblioteki UART


	while(1)
		{

		int16_t left_fotoresistor_value = adc_measurement(6);												// Pomiar napiÍcia na kanale ADC6 - lewy fotorezystor
		int16_t right_fotoresistor_value = adc_measurement(7);												// Pomiar napiÍcia na kanale ADC7 - prawy fotorezystor
		int16_t right_fotoresistor_value_correct =right_fotoresistor_value-(right_fotoresistor_value*0.16); // Korekta odczytu bardziej czu≥ego fotorezystora
		int16_t difference=left_fotoresistor_value-right_fotoresistor_value_correct; 						// RÛønica napiÍc.

		float a;					// Zmienna pomocnicza do wyznaczenia wartosci intenisty
		int8_t intensity; 			// NatÍøenie úwiat≥a, im ürÛd≥o úwiat≥a znajduje siÍ bliøej fotorezystora tym wiÍkszy odczyt ADC
		int8_t max_intenisty = 7; 	// Maksymalne zmierzone natÍøenie úwiat≥a
		int8_t limit_value = 100;	// Wyznacznie granic LEFT/FRONT/RIGHT


		// Pomocniczy fragment kodu, dla odczytu zmian wartosci ADC
		//uart_puts("\r ADC lewego/prawego fotorezystora: ");
		//uart_putint(left_fotoresistor_value,10);
		//uart_putc("/");
		//uart_putint(right_fotoresistor_value,10);

		if(abs(difference)<=limit_value) // èrÛd≥o úwiat≥a na wprost.
			{
			lcd_cls();
			lcd_locate(1,5);
			lcd_str("FRONT");
			a=((left_fotoresistor_value>right_fotoresistor_value_correct)? left_fotoresistor_value:right_fotoresistor_value_correct)/100; // Wyznaczanie ADC max. Dla przedzia≥u FRONT lewy lub prawy fotorezystor moøe miec najwieksza wartosc.
			}
		else if(difference<-limit_value) // èrÛd≥o úwiat≥a na lewo.
				{
				lcd_cls();
				lcd_locate(1,11);
				lcd_str("RIGHT");
				a=right_fotoresistor_value_correct/100;
				}
		else if(abs(difference)>limit_value)// èrÛd≥o úwiat≥a na prawo.
				{
				lcd_cls();
				lcd_locate(1,0);
				lcd_str("LEFT");
				a=left_fotoresistor_value/100;
				}

		intensity=abs((int8_t)a);							// Jawne rzutowanie typÛw, przypisanie czÍúci ca≥kowitej
		int8_t distance_marker =max_intenisty-intensity;	// Okreúlenie znacznika odleg≥oúci

		uint8_t i=0;
		for (i=0; i<distance_marker+1;i++)					// Graficzne przedstawienie odleg≥oúci, im wiÍcej ">" tym dalej znajduje siÍ ürÛ≥o úwiat≥a
			{
				lcd_locate(0,i);
				lcd_str(">");
			}

		_delay_ms(200);
		}
}

uint16_t adc_measurement(uint8_t adc_canal)
{
	_delay_us(250);							// OpÛünienie na potrzeby zmiany kana≥u ADC
	ADMUX= (ADMUX&0xF8)|adc_canal;			// ADMUX&0xF8 - Maska bitowa zabezpieczajπca 5 starszych bitÛw przed modyfikacjπ i zerujπca 3 m≥odsze dziÍki czemu mozliwe jest ustawienie nowego kana≥u(|adc_canal)
	ADCSRA|=(1<<ADSC);						// Ustawienie bitu rozpoczynajπcego konwersjÍ.
	while (ADCSRA&(1<<ADSC)){};				// W trakcie trwania pomiaru ADSC=1. Po pomiarze ADSC zeruje siÍ automatycznie
											// Oczekiwanie  na zakoÒczenie pomiaru.
	return ADCW;							// Funkcja zwraca wartoúc rejestrÛw ADCH i ADCL
}

