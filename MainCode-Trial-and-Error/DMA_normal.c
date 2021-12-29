#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "string.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "misc.h"
#include "math.h"

//#ifndef DHT11_H
#define DHT11_H

#define DHT11_SUCCESS         1
#define DHT11_ERROR_CHECKSUM  2
#define DHT11_ERROR_TIMEOUT   3

int res;
int i;
int a = 1;
int b = 2;
int c = 3;
char buff[32];

uint16_t humid;
uint16_t light;

typedef struct DHT11_Dev {
	uint8_t temparature;
	uint8_t humidity;
	GPIO_TypeDef* port;
	uint16_t pin;
} DHT11_Dev;

int DHT11_init(struct DHT11_Dev* dev, GPIO_TypeDef* port, uint16_t pin);
int DHT11_read(struct DHT11_Dev* dev);

struct DHT11_Dev dev;

volatile uint32_t ADC_Value[2];

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void Init_USART(void);
void NVIC_Configure(void);
void sendDHL(uint16_t T, uint16_t H);


void EXTI15_10_IRQHandler(void);

void sendDataUART1(uint16_t data);
void sendDataUART2(uint16_t data);

void Delay(void);


void Delay_us(uint32_t us){  
                if(us>1){
                         uint32_t count=us*7200;
                         while(count--); 
                 }else{
                         uint32_t count=2;
                         while(count--); 
                  }
}


void RCC_Configure(void)
{
    /* UART TX/RX port clock enable */ // PA9, PA10
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /* USART1 clock enable */ //
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    /* USART2 clock enable */ //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    /* Alternate Function IO clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    // light clock enable
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    // dht11
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // humid sensor
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    

}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* UART1 pin setting */
    //TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    
    // light
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* UART2 pin setting */
    //TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // humid sensor
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

}

void Init_USART(void)
{
    USART_InitTypeDef USART1_InitStructure;
    USART_InitTypeDef USART2_InitStructure;

    // Enable the USART1, 2 peripheral
    USART_Cmd(USART1, ENABLE);
    USART_Cmd(USART2, ENABLE);
    
    USART1_InitStructure.USART_BaudRate = 9600;
    USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART1_InitStructure.USART_StopBits = USART_StopBits_1;
    USART1_InitStructure.USART_Parity = USART_Parity_No;
    USART1_InitStructure.USART_Mode = (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);
    USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART1_InitStructure);

    USART2_InitStructure.USART_BaudRate = 9600;
    USART2_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART2_InitStructure.USART_StopBits = USART_StopBits_1;
    USART2_InitStructure.USART_Parity = USART_Parity_No;
    USART2_InitStructure.USART_Mode = (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);
    USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART2_InitStructure);
    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}


void EXTI_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    /* USART1: RX*/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource10);
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* USART2: RX*/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3);
    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}



void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    // UART1
    // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IPR0_PRI_0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // UART2
    // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IPR0_PRI_0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // humid sensor

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void ADC_Configure(void){

  // light sensor and humid sensor ADC Configuration
  ADC_InitTypeDef ADC_10;
  ADC_DeInit(ADC1);
  ADC_10.ADC_Mode = ADC_Mode_Independent;
  ADC_10.ADC_ScanConvMode = ENABLE;
  ADC_10.ADC_ContinuousConvMode = ENABLE;
  ADC_10.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_10.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_10.ADC_NbrOfChannel = 2;
  ADC_Init(ADC1, &ADC_10);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11,1, ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12,2, ADC_SampleTime_239Cycles5);
  ADC_DMACmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);

  ADC_ResetCalibration(ADC1);
  while (ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while (ADC_GetCalibrationStatus(ADC1));
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);
  
 
}


void DMA_Configure(void){
    DMA_InitTypeDef  DMA_InitStructure;
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 8;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Value;
    
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE); //Enable the DMA1 - Channel1
    
}   

void DMA1_Channel1_IRQHandler(void)
{
  static uint16_t tmp;
  if((DMA_GetITStatus(DMA1_IT_TC1)!= RESET) && (DMA_GetFlagStatus(DMA1_FLAG_TC1) != RESET))
  {
    humid = ADC_Value[0];
    light = ADC_Value[1];
    printf("hello\n");
    
    char msg[] = "TEAM03.\r\n";
      //sprintf(buf, "%d,%d,%d", dev.temparature, dev.humidity, light);
      int i = 0;
      while (msg[i] != 0) {
          sendDataUART1(msg[i]);
          i++;
      } 
    
    if (humid < 2000){
      
      
    }
    
    else{
      
      
    }
    DMA_ClearITPendingBit(DMA1_IT_TC1);
    
  }
  
}
void UltrasonicWave_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;    
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//Turn on the GPIOB clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //Turn on the TIM2 clock    

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;    //  Corresponding to the trig pin
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Push-pull output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB,GPIO_Pin_1);    

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    //  Corresponding to echo pin
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //Drop down input
  GPIO_Init(GPIOB, &GPIO_InitStructure);    

  //The timer is initialized, the frequency division coefficient is 71, the frequency is 1MHZ, and each count is 1us, (the higher the frequency, the more accurate)
  //The reload value is 65535, and the overflow time is 1us*65536=65.536ms=0.065536s
  //One counting cycle can measure 0.065536s * 340m/s / 2 = 11.14112m
  TIM_TimeBaseStructure.TIM_Period = 65535; 
  TIM_TimeBaseStructure.TIM_Prescaler = 71; 
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //    
}

void UltrasonicWave_StartMeasure(void)
{
  GPIO_SetBits(GPIOB, GPIO_Pin_1); //Pull up PB1 level
  Delay_us(20); 
  GPIO_ResetBits(GPIOB, GPIO_Pin_1); //Pull down the PB1 level
}

/***** Ranging *****/
float UltrasonicWave_Measure(void) //
{
  uint32_t bef, aft;
  while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==1);//When echo is high, wait to low level before starting ultrasound
  UltrasonicWave_StartMeasure(); //Start ultrasound    
  while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0);//Waiting for the high level of echo    
  TIM_SetCounter(TIM2,0); //Clear counter
  TIM_Cmd(TIM2, ENABLE); //Enable timer 2, start counting
  bef = TIM_GetCounter(TIM2);
  while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 1);//Wait for the high level of echo to end
  aft = TIM_GetCounter(TIM2);
  TIM_Cmd(TIM2, DISABLE);    //Disable timer 2, cut off count    
  return (aft - bef);///1000000*340/2 *100;    //Here the unit is converted to cm
}

int DHT11_init(struct DHT11_Dev* dev, GPIO_TypeDef* port, uint16_t pin) {
	TIM_TimeBaseInitTypeDef TIM_TimBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	dev->port = port;
	dev->pin = pin;

	//Initialise TIMER2
	TIM_TimBaseStructure.TIM_Period = 84000000 - 1;
	TIM_TimBaseStructure.TIM_Prescaler = 84;
	TIM_TimBaseStructure.TIM_ClockDivision = 0;
	TIM_TimBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimBaseStructure);
	TIM_Cmd(TIM3, ENABLE);

	//Initialise GPIO DHT11
	GPIO_InitStructure.GPIO_Pin = dev->pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(dev->port, &GPIO_InitStructure);

	return 0;
}


int DHT11_read(struct DHT11_Dev* dev) {

	//Initialisation
	uint8_t i, j, temp;
	uint8_t data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	GPIO_InitTypeDef GPIO_InitStructure;

	//Generate START condition

	GPIO_InitStructure.GPIO_Pin = dev->pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(dev->port, &GPIO_InitStructure);


	//Put LOW for at least 18ms
	GPIO_ResetBits(dev->port, dev->pin);
        
	//wait 18ms
	TIM3->CNT = 0;
	while((TIM3->CNT) <= 18000);

	//Put HIGH for 20-40us
	GPIO_SetBits(dev->port, dev->pin);

	//wait 40us
	TIM3->CNT = 0;
	while((TIM3->CNT) <= 40);
	//End start condition


	//Input mode to receive data
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(dev->port, &GPIO_InitStructure);

	//DHT11 ACK
	//should be LOW for at least 80us

	TIM3->CNT = 0;
	while(!GPIO_ReadInputDataBit(dev->port, dev->pin)) {
		if(TIM3->CNT > 100)
			return DHT11_ERROR_TIMEOUT;
	}

	//should be HIGH for at least 80us
	//while(GPIO_ReadInputDataBit(dev->port, dev->pin));
	TIM3->CNT = 0;
	while(GPIO_ReadInputDataBit(dev->port, dev->pin)) {
		if(TIM3->CNT > 100)
			return DHT11_ERROR_TIMEOUT;
	}

	//Read 40 bits (8*5)
	for(j = 0; j < 5; ++j) {
		for(i = 0; i < 8; ++i) {

			//LOW for 50us
			while(!GPIO_ReadInputDataBit(dev->port, dev->pin));
			/*TIM2->CNT = 0;
			while(!GPIO_ReadInputDataBit(dev->port, dev->pin)) {
				if(TIM2->CNT > 60)
					return DHT11_ERROR_TIMEOUT;
			}*/

			//Start counter
			TIM_SetCounter(TIM3, 0);

			//HIGH for 26-28us = 0 / 70us = 1
			while(GPIO_ReadInputDataBit(dev->port, dev->pin));
			/*while(!GPIO_ReadInputDataBit(dev->port, dev->pin)) {
				if(TIM2->CNT > 100)
					return DHT11_ERROR_TIMEOUT;
			}*/

			//Calc amount of time passed
			temp = TIM_GetCounter(TIM3);

			//shift 0
			data[j] = data[j] << 1;

			//if > 30us it's 1
			if(temp > 40)
				data[j] = data[j]+1;
		}
	}

	//verify the Checksum
	//if(data[4] != (data[0] + data[2]))
	//	return DHT11_ERROR_CHECKSUM;

	//set data
	dev->temparature = data[2];
	dev->humidity = data[0];

	return DHT11_SUCCESS;
}

void init_DHT(void)
{
            res = DHT11_read(&dev);

            if(res == b) {
                    printf("DHT11_ERROR_CHECKSUM\n");
                                     }
            else if(res == a) {

                    printf("dht11 success\n");
                    printf("TEMPRATURE %d - HUMIDITY %d\n", dev.temparature, dev.humidity);

                    char example[100];

                    /* Copy the first string into the variable */
                    char result[50];
                    char resultt[50];

                    sprintf(result, "%d", dev.temparature);
                    sprintf(resultt, "%d", dev.humidity);
                    /* Concatenate the following two strings to the end of the first one */
                    strcpy(example, "{\"Temp\":");
                    strcat(example, result);
                    strcat(example, ",");
                    strcat(example, "\"Hum\":");
                    strcat(example, resultt);
                    strcat(example, "}");
                    //USARTSend(example);

                    printf("data : %s\n", example);
                   

                     }
                    else {
                     printf("TIMEOUT \r \n");
                             }
                    
                    //delay 5 second
                    int c, d;
                    for (c = 1; c <= 5000; c++)
                    for (d = 1; d <= 5000; ++d);

}


void USART1_IRQHandler() {
    uint16_t word;
    if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
        // the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART1);

        
        sendDataUART2(word);
        
        // clear 'Read data register not empty' flag
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}

// TODO!!!: USART2 handler
void USART2_IRQHandler() {

    uint16_t word;
    if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET){
        // the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART2);

        sendDataUART1(word);
       

        // clear 'Read data register not empty' flag
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}



void sendDataUART1(uint16_t data) {
    
    USART_SendData(USART1, data);
}

void sendDataUART2(uint16_t data) {
    USART_SendData(USART2, data);
}

/*void initRelay(void)
{
    // relay moudle
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void initHumid(void)
{
    // humid sensor
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}*/


  
int main(void)
{
    float distance;

    SystemInit();

    RCC_Configure();

    GPIO_Configure();
    
    EXTI_Configure();

    Init_USART();

    NVIC_Configure();
    
    ADC_Configure();
    
    DMA_Configure();
    
    //UltrasonicWave_Init();

    DHT11_init(&dev, GPIOB, GPIO_Pin_6);

    
    while (1){
      init_DHT();
      //ADC_DMACmd(ADC1, ENABLE); // To use light sensor
      //distance = UltrasonicWave_Measure();
      //printf("distance:%5.2f\n",distance);
     
      printf("light: %d\n", light);
      printf("humid: %d\n", humid);
     
      

      DMA_Cmd(DMA1_Channel1, DISABLE);
      DMA1_Channel1->CMAR = (uint32_t)ADC_Value;
      DMA1_Channel1->CNDTR = 8;
      DMA_Cmd(DMA1_Channel1, ENABLE);
      
      printf("-----------------------------\n");
      Delay_us(90);
    }
}
