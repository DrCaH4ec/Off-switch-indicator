#define F_CPU 9600000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define PRECISION 5										//чувствительность к изменению положения стика, указывается в процентах

#define TIM_SEC 30										//количество секунд которые должен подсчитать таймер перед сигнализированием 
#define TIM_MIN 5										//количество минут которые должен подсчитать таймер перед сигнализированием

#define BEEP_DELAY 50									//длительность писка пьезоэлемента, указывается в милисекундах
#define SOUND_DELAY 100									//задержка между чередой писков пьезоэлемента, указывается в милисекундах

#define BUZER 0											//вывод порта МК к которому подключен пьезоизлучатель

uint8_t ten_milis = 0, com_sec = 0, com_min = 0, sound_flag = 0;

void beep();											//функция отвечающая за писк
void ADC_init();										//инициализация АЦП
uint16_t ADC_read();									//функция которая возвращает считаное значение АЦП
void PORT_init();										//инициализаци портов МК
void TIMER_init();										//инициализация таймера
void Sound();											//функция отвечающая за "мелодию" которую будет играть пьезоизлучатель


ISR(TIM0_OVF_vect){

	ten_milis++;

	if(ten_milis == 100){								//поскольку таймер может считать только с шагом в 10мс, тогда считаем 100 "тиков"
		com_sec++;										//когда насчитывается 100 "тиков" счетчик секунд инкрементируется на 1
		ten_milis = 0;									//а счетчик десятков милисекунд обнуляем
	}

	if(com_sec == 60){									
		com_sec = 0;									//когда насчитывается 60 секунд, счетчик секунд обнуляется
		com_min++;										//а счетчик минут инкрементируется на 1
	}

	if(com_min == TIM_MIN && com_sec == TIM_SEC)		//как только подсчитанные секунды и минуты совпадут с установленными
		sound_flag = 1;									//разрешаем издание звука устройством путем установки звукового флага в 1

	TCNT0=0xA2;											//устанавливаем значение таймера в начальное значение
} 

int main(void)
{
	PORT_init();
	ADC_init();
	TIMER_init();

	uint16_t cur_val, prev_val;

	prev_val = ADC_read();										//считывание начального значения АЦП

	while(1){

		cur_val = ADC_read();									//считывание "текущего" значения АЦП

		if(cur_val < prev_val-(prev_val/100)*PRECISION || cur_val > prev_val + (prev_val/100)*PRECISION){			//сравнения предыдущего и текущего значений АЦП
			prev_val = cur_val;
			ten_milis = 0;
			com_sec = 0;
			com_min = 0;
			if(sound_flag == 1)
				sound_flag = 0;	
		}

		if(sound_flag == 1)
			Sound();
		
	}
	return 0;
}

void ADC_init(){
	ADMUX |= (1<<MUX1) | (1<<MUX0);
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	DIDR0 |= (1<<ADC3D);
	_delay_us(10);
}

uint16_t ADC_read(){
	uint16_t ADC_res = 0;
	uint8_t i;

	for(i = 0; i < 100; i++){									//делаем 100 измерений значения АЦП
		ADCSRA |= (1<<ADSC);									//и ищем среднее арифметическое
		while(!(ADCSRA&(1<<ADIF)));								//тем самым повышая точность измерения
		ADCSRA |= (1<<ADIF);

		ADC_res += ADCW/100;
	}
	

	return ADC_res;
}

void PORT_init(){
	DDRB = 0x00;
	PORTB = 0x00;

	DDRB |= (1<<BUZER);
}

void TIMER_init(){
	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 9,375 kHz
	// Mode: Normal top=0xFF
	// OC0A output: Disconnected
	// OC0B output: Disconnected
	// Timer Period: 10,027 ms
	TCCR0B |= (1<<CS02) | (1<<CS00);
	TCNT0 = 0xA2;

	// Timer/Counter 0 Interrupt(s) initialization
	TIMSK0 |= (1<<TOIE0);

	asm("sei");
}

void beep(){
	PORTB |= (1<<0);
	_delay_ms(BEEP_DELAY);
	PORTB &= ~(1<<0);
	_delay_ms(BEEP_DELAY);
}

void Sound(){
	beep();
	beep();
	beep();
	_delay_ms(SOUND_DELAY);
	beep();
	_delay_ms(SOUND_DELAY);
	beep();
	beep();
	beep();
	_delay_ms(SOUND_DELAY);
}