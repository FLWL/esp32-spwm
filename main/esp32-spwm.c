#include "driver/gpio.h"
#include "driver/ledc.h"
#include "soc/ledc_reg.h"

#define SIGNAL_PIN GPIO_NUM_12
#define STEP_FREQUENCY 48000
#define SINE_TABLE_SIZE 32
// generated using https://daycounter.com/Calculators/Sine-Generator-Calculator.phtml
static int SINE_LOOKUP_TABLE[SINE_TABLE_SIZE] =
{
	128,152,176,198,218,234,245,253,
	255,253,245,234,218,198,176,152,
	128,103,79,57,37,21,10,2,
	0,2,10,21,37,57,79,103,
};
static int sine_table_index = 0;

void IRAM_ATTR ledc_timer0_overflow_isr(void *arg)
{
	// clear the interrupt
	REG_SET_BIT(LEDC_INT_CLR_REG, LEDC_HSTIMER0_OVF_INT_CLR);
	
	// update duty, shift the duty 4 bits to the left due to ESP32 register format
	REG_WRITE(LEDC_HSCH0_DUTY_REG, SINE_LOOKUP_TABLE[sine_table_index] << 4);
	REG_SET_BIT(LEDC_HSCH0_CONF1_REG, LEDC_DUTY_START_HSCH0);
	
	// increment the sine table index
	if (++sine_table_index >= SINE_TABLE_SIZE) sine_table_index = 0;
}

void app_main()
{
	// configure ledc timer0
	ledc_timer_config_t ledc_timer0;
	ledc_timer0.duty_resolution = LEDC_TIMER_8_BIT;
	ledc_timer0.freq_hz = STEP_FREQUENCY;
	ledc_timer0.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_timer0.timer_num = LEDC_TIMER_0;
	ledc_timer_config(&ledc_timer0);
	
	// configure ledc channel0
	ledc_channel_config_t ledc_channel0;
	ledc_channel0.channel = LEDC_CHANNEL_0;
	ledc_channel0.duty = 0;
	ledc_channel0.gpio_num = SIGNAL_PIN;
	ledc_channel0.hpoint = 0;
	ledc_channel0.timer_sel = LEDC_TIMER_0;
	ledc_channel0.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_channel0.intr_type = LEDC_INTR_DISABLE;
	ledc_channel_config(&ledc_channel0);
	
	// register overflow interrupt handler for timer0
	ledc_isr_register(ledc_timer0_overflow_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);
	// enable the overflow interrupt
	REG_SET_BIT(LEDC_INT_ENA_REG, LEDC_HSTIMER0_OVF_INT_ENA);
}
