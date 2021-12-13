#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "DHT11.h"
#include "misc.h"

#define DHT11_DATA_GPIO_Port    GPIOA
#define DHT11_DATA_Pin          GPIO_Pin_14

DHT11DATA dht11_data;

volatile uint32_t ADC_Value[1];
uint32_t light;

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void Init_USART(void);
void NVIC_Configure(void);

void EXTI15_10_IRQHandler(void);

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
}

void ADC_Configure(void){

  ADC_InitTypeDef ADC_10;
  ADC_DeInit(ADC1);
  ADC_10.ADC_Mode = ADC_Mode_Independent;
  ADC_10.ADC_ScanConvMode = DISABLE;
  ADC_10.ADC_ContinuousConvMode = ENABLE;
  ADC_10.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_10.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_10.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_10);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12,1, ADC_SampleTime_239Cycles5);
  ADC_DMACmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);

  ADC_ResetCalibration(ADC1);
  while (ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while (ADC_GetCalibrationStatus(ADC1));
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}

void DMA_Configure(void){
    DMA_InitTypeDef DMA_Instructure;

    DMA_Instructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_Instructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Value[0];
    DMA_Instructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_Instructure.DMA_BufferSize = 4;
    DMA_Instructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_Instructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_Instructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_Instructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_Instructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Instructure.DMA_Priority = DMA_Priority_High;
    DMA_Instructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel1, &DMA_Instructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
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



int main(void)
{
    float distance;

    SystemInit();

    RCC_Configure();

    GPIO_Configure();
    
    //EXTI_Configure();

    Init_USART();

    NVIC_Configure();
    
    ADC_Configure();
    
    DMA_Configure();
    
    UltrasonicWave_Init();
    //delay_init();

    while (1){
      ADC_DMACmd(ADC1, ENABLE); // To use light sensor
      distance = UltrasonicWave_Measure();
      printf("distance:%5.2f\n",distance);
      printf("light: %d\n", ADC_Value[0]);
      Delay_us(90);
      
      
    }
}
