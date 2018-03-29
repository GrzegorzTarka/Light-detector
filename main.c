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

uint16_t adc_measurement(uint8_t adc_canal); // adc_canal = warto�c od 0 do 7, odpowiada pinom portu A

int main (void)
{
	ADMUX 	|= (1<<REFS0)|(1<<REFS1); // Wewn�trzne napi�cie odniesienia Vref=2,56.
	ADCSRA	|= (1<<ADEN);			  // W��czenie ADC.
	ADCSRA	|= (1<<ADPS1)|(1<<ADPS2); // Ustawienie preskalera dla ADC na 64 (8MHz/64=125kHz). Wg. noty dla rozdzielczosci 10 bitowej ADC wymaga taktowania 50-200 kHz.

	USART_Init( __UBRR );			// inicjalizacja UART
	lcd_init();						// inicjalizacja LCD
	sei();							// globalne zezwolenie na przerwania, na potrzeby biblioteki UART


	while(1)
		{

		int16_t left_fotoresistor_value = adc_measurement(6);												// Pomiar napi�cia na kanale ADC6 - lewy fotorezystor
		int16_t right_fotoresistor_value = adc_measurement(7);												// Pomiar napi�cia na kanale ADC7 - prawy fotorezystor
		int16_t right_fotoresistor_value_correct =right_fotoresistor_value-(right_fotoresistor_value*0.16); // Korekta odczytu bardziej czu�ego fotorezystora
		int16_t difference=left_fotoresistor_value-right_fotoresistor_value_correct; 						// R�nica napi�c.

		float a;					// Zmienna pomocnicza do wyznaczenia wartosci intenisty
		int8_t intensity; 			// Nat�enie �wiat�a, im �r�d�o �wiat�a znajduje si� bli�ej fotorezystora tym wi�kszy odczyt ADC
		int8_t max_intenisty = 7; 	// Maksymalne zmierzone nat�enie �wiat�a
		int8_t limit_value = 100;	// Wyznacznie granic LEFT/FRONT/RIGHT


		// Pomocniczy fragment kodu, dla odczytu zmian wartosci ADC
		//uart_puts("\r ADC lewego/prawego fotorezystora: ");
		//uart_putint(left_fotoresistor_value,10);
		//uart_putc("/");
		//uart_putint(right_fotoresistor_value,10);

		if(abs(difference)<=limit_value) // �r�d�o �wiat�a na wprost.
			{
			lcd_cls();
			lcd_locate(1,5);
			lcd_str("FRONT");
			a=((left_fotoresistor_value>right_fotoresistor_value_correct)? left_fotoresistor_value:right_fotoresistor_value_correct)/100; // Wyznaczanie ADC max. Dla przedzia�u FRONT lewy lub prawy fotorezystor mo�e miec najwieksza wartosc.
			}
		else if(difference<-limit_value) // �r�d�o �wiat�a na lewo.
				{
				lcd_cls();
				lcd_locate(1,11);
				lcd_str("RIGHT");
				a=right_fotoresistor_value_correct/100;
				}
		else if(abs(difference)>limit_value)// �r�d�o �wiat�a na prawo.
				{
				lcd_cls();
				lcd_locate(1,0);
				lcd_str("LEFT");
				a=left_fotoresistor_value/100;
				}

		intensity=abs((int8_t)a);							// Jawne rzutowanie typ�w, przypisanie cz�ci ca�kowitej
		int8_t distance_marker =max_intenisty-intensity;	// Okre�lenie znacznika odleg�o�ci

		uint8_t i=0;
		for (i=0; i<distance_marker+1;i++)					// Graficzne przedstawienie odleg�o�ci, im wi�cej ">" tym dalej znajduje si� �r�o �wiat�a
			{
				lcd_locate(0,i);
				lcd_str(">");
			}

		_delay_ms(200);
		}
}

uint16_t adc_measurement(uint8_t adc_canal)
{
	_delay_us(250);							// Op�nienie na potrzeby zmiany kana�u ADC
	ADMUX= (ADMUX&0xF8)|adc_canal;			// ADMUX&0xF8 - Maska bitowa zabezpieczaj�ca 5 starszych bit�w przed modyfikacj� i zeruj�ca 3 m�odsze dzi�ki czemu mozliwe jest ustawienie nowego kana�u(|adc_canal)
	ADCSRA|=(1<<ADSC);						// Ustawienie bitu rozpoczynaj�cego konwersj�.
	while (ADCSRA&(1<<ADSC)){};				// W trakcie trwania pomiaru ADSC=1. Po pomiarze ADSC zeruje si� automatycznie
											// Oczekiwanie  na zako�czenie pomiaru.
	return ADCW;							// Funkcja zwraca warto�c rejestr�w ADCH i ADCL
}

