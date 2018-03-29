
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "MKUART/mkuart.h"
#include "LCD/lcd44780.h"
#include <stdlib.h>

int16_t adc_measurement(uint8_t adc_canal); // adc_canal = wartośc od 0 do 7, odpowiada pinom portu A

int main (void)
{
	ADMUX 	|= (1<<REFS0)|(1<<REFS1); // Wewnętrzne napięcie odniesienia Vref=2,56.
	ADCSRA	|= (1<<ADEN);		  // Włączenie ADC.
	ADCSRA	|= (1<<ADPS1)|(1<<ADPS2); // Ustawienie preskalera dla ADC na 64 (8MHz/64=125kHz). Wg. noty dla rozdzielczosci 10 bitowej ADC wymaga taktowania 50-200 kHz.

	USART_Init( __UBRR );		  // Inicjalizacja UART.
	lcd_init();			  // Inicjalizacja LCD.
	sei();				  // Globalne zezwolenie na przerwania, na potrzeby biblioteki UART.


	while(1)
		{

		int16_t left_fotoresistor_value = adc_measurement(6);	// Pomiar napięcia na kanale ADC6 - lewy fotorezystor
		int16_t right_fotoresistor_value = adc_measurement(7);	// Pomiar napięcia na kanale ADC7 - prawy fotorezystor
		int16_t right_fotoresistor_value_correct =right_fotoresistor_value-(right_fotoresistor_value*0.16); // Korekta odczytu bardziej czułego fotorezystora
		int16_t difference=left_fotoresistor_value-right_fotoresistor_value_correct; 			    // Różnica napięc.

		float a;			// Zmienna pomocnicza do wyznaczenia wartosci intenisty.
		int8_t intensity; 		// Natężenie światła, im źródło światła znajduje się bliżej fotorezystora tym większy odczyt ADC.
		int8_t max_intenisty = 7; 	// Maksymalne zmierzone natężenie światła.
		int8_t limit_value = 100;	// Wyznacznie granic LEFT/FRONT/RIGHT.


		// Pomocniczy fragment kodu, dla odczytu zmian wartosci ADC
		//uart_puts("\r ADC lewego/prawego fotorezystora: ");
		//uart_putint(left_fotoresistor_value,10);
		//uart_putc("/");
		//uart_putint(right_fotoresistor_value,10);

		if(abs(difference)<=limit_value) // Źródło światła na wprost.
			{
			lcd_cls();
			lcd_locate(1,5);
			lcd_str("FRONT");
			a=((left_fotoresistor_value>right_fotoresistor_value_correct)? left_fotoresistor_value:right_fotoresistor_value_correct)/100; // Wyznaczanie ADC max. Dla przedziału FRONT lewy lub prawy fotorezystor może miec najwieksza wartosc.
			}
		else if(difference<-limit_value) // Źródło światła na lewo.
				{
				lcd_cls();
				lcd_locate(1,11);
				lcd_str("RIGHT");
				a=right_fotoresistor_value_correct/100;
				}
		else if(abs(difference)>limit_value)// Źródło światła na prawo.
				{
				lcd_cls();
				lcd_locate(1,0);
				lcd_str("LEFT");
				a=left_fotoresistor_value/100;
				}

		intensity=abs((int8_t)a);				// Jawne rzutowanie typów, przypisanie części całkowitej
		int8_t distance_marker =max_intenisty-intensity;	// Określenie znacznika odległości

		uint8_t i=0;
		for (i=0; i<distance_marker+1;i++)			// Graficzne przedstawienie odległości, im więcej ">" tym dalej znajduje się źróło światła
			{
				lcd_locate(0,i);
				lcd_str(">");
			}

		_delay_ms(200);
		}
}

int16_t adc_measurement(uint8_t adc_canal)
{
	_delay_us(250);					// Opóźnienie na potrzeby zmiany kanału ADC
	ADMUX= (ADMUX&0xF8)|adc_canal;			// ADMUX&0xF8 - Maska bitowa zabezpieczająca 5 starszych bitów przed modyfikacją i zerująca 3 młodsze dzięki czemu mozliwe jest ustawienie nowego kanału(|adc_canal)
	ADCSRA|=(1<<ADSC);				// Ustawienie bitu rozpoczynającego konwersję.
	while (ADCSRA&(1<<ADSC)){};			// W trakcie trwania pomiaru ADSC=1. Po pomiarze ADSC zeruje się automatycznie
							// Oczekiwanie  na zakończenie pomiaru.
	return ADCW;					// Funkcja zwraca wartośc rejestrów ADCH i ADCL
}

