#include "stm32f4xx.h"
#include "stdio.h"

volatile uint32_t score_1 = 0;
volatile uint32_t score_2 = 0;
volatile uint8_t button_pressed_1 = 0;
volatile uint8_t button_pressed_2 = 0;

void USART1_Init(void);
void GPIO_Init(void);
void USART1_SendChar(char c);
void USART1_SendString(char *str);
void delay(uint32_t count);
void Display_Score1(uint32_t score);
void Display_Score2(uint32_t score);

#define SEGMENT1_PORT GPIOB
#define SEG_A_PIN_1 (1 << 0)
#define SEG_B_PIN_1 (1 << 1)
#define SEG_C_PIN_1 (1 << 2)
#define SEG_D_PIN_1 (1 << 3)
#define SEG_E_PIN_1 (1 << 4)
#define SEG_F_PIN_1 (1 << 5)
#define SEG_G_PIN_1 (1 << 6)

#define SEGMENT2_PORT GPIOA
#define SEG_A_PIN_2 (1 << 3)
#define SEG_B_PIN_2 (1 << 4)
#define SEG_C_PIN_2 (1 << 5)
#define SEG_D_PIN_2 (1 << 6)
#define SEG_E_PIN_2 (1 << 7)
#define SEG_F_PIN_2 (1 << 11)
#define SEG_G_PIN_2 (1 << 12)

const uint8_t digit_segments[] = {
    0x3F,
    0x06,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x07,
    0x7F,
    0x6F
};

int main(void) {
    USART1_Init();
    GPIO_Init();

    while (1) {
        if (button_pressed_1) {
            if (score_1 < 9) {
                score_1++;
                char score_str[30];
                snprintf(score_str, sizeof(score_str), "Score1: %lu, Score2: %lu\r\n", score_1, score_2);
                USART1_SendString(score_str);
                Display_Score1(score_1);
                button_pressed_1 = 0;
            }
        }
        if (button_pressed_2) {
            if (score_2 < 9) {
                score_2++;
                char score_str[30];
                snprintf(score_str, sizeof(score_str), "Score1: %lu, Score2: %lu\r\n", score_1, score_2);
                USART1_SendString(score_str);
                Display_Score2(score_2);
                button_pressed_2 = 0;
            }
        }
    }
}

void USART1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER |= GPIO_MODER_MODER9_1;
    GPIOA->AFR[1] |= 0x00000070;

    GPIOA->MODER |= GPIO_MODER_MODER10_1;
    GPIOA->AFR[1] |= 0x00000700;

    USART1->BRR = 0x0683;
    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void GPIO_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    GPIOA->MODER &= ~GPIO_MODER_MODER0;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0;

    GPIOA->MODER &= ~GPIO_MODER_MODER2;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR2_0;

    SEGMENT1_PORT->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2 |
                              GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5 |
                              GPIO_MODER_MODER6);
    SEGMENT1_PORT->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 |
                             GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 |
                             GPIO_MODER_MODER6_0);

    SEGMENT2_PORT->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5 |
                              GPIO_MODER_MODER6 | GPIO_MODER_MODER7 | GPIO_MODER_MODER11 |
                              GPIO_MODER_MODER12);
    SEGMENT2_PORT->MODER |= (GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 |
                             GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER11_0 |
                             GPIO_MODER_MODER12_0);

    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
    EXTI->IMR |= EXTI_IMR_MR0;
    EXTI->RTSR |= EXTI_RTSR_TR0;

    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA;
    EXTI->IMR |= EXTI_IMR_MR2;
    EXTI->RTSR |= EXTI_RTSR_TR2;

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {
        button_pressed_1 = 1;
        EXTI->PR = EXTI_PR_PR0;
    }
}

void EXTI2_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR2) {
        button_pressed_2 = 1;
        EXTI->PR = EXTI_PR_PR2;
    }
}

void USART1_SendChar(char c) {
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

void USART1_SendString(char *str) {
    while (*str) {
        USART1_SendChar(*str++);
    }
}

void delay(uint32_t count) {
    while (count--) {
        __NOP();
    }
}

void Display_Score1(uint32_t score) {
    uint8_t segment_pattern;
    if (score < 10) {
        segment_pattern = digit_segments[score];
        
        SEGMENT1_PORT->ODR |= (SEG_A_PIN_1 | SEG_B_PIN_1 | SEG_C_PIN_1 | SEG_D_PIN_1 | SEG_E_PIN_1 | SEG_F_PIN_1 | SEG_G_PIN_1);
        
        if (segment_pattern & 0x01) SEGMENT1_PORT->ODR &= ~SEG_A_PIN_1;
        if (segment_pattern & 0x02) SEGMENT1_PORT->ODR &= ~SEG_B_PIN_1;
        if (segment_pattern & 0x04) SEGMENT1_PORT->ODR &= ~SEG_C_PIN_1;
        if (segment_pattern & 0x08) SEGMENT1_PORT->ODR &= ~SEG_D_PIN_1;
        if (segment_pattern & 0x10) SEGMENT1_PORT->ODR &= ~SEG_E_PIN_1;
        if (segment_pattern & 0x20) SEGMENT1_PORT->ODR &= ~SEG_F_PIN_1;
        if (segment_pattern & 0x40) SEGMENT1_PORT->ODR &= ~SEG_G_PIN_1;
    } else {
        SEGMENT1_PORT->ODR |= (SEG_A_PIN_1 | SEG_B_PIN_1 | SEG_C_PIN_1 | SEG_D_PIN_1 | SEG_E_PIN_1 | SEG_F_PIN_1 | SEG_G_PIN_1);
    }
    delay(1000000);
}

void Display_Score2(uint32_t score) {
    uint8_t segment_pattern;
    if (score < 10) {
        segment_pattern = digit_segments[score];
        
        SEGMENT2_PORT->ODR |= (SEG_A_PIN_2 | SEG_B_PIN_2 | SEG_C_PIN_2 | SEG_D_PIN_2 | SEG_E_PIN_2 | SEG_F_PIN_2 | SEG_G_PIN_2);
        
        if (segment_pattern & 0x01) SEGMENT2_PORT->ODR &= ~SEG_A_PIN_2;
        if (segment_pattern & 0x02) SEGMENT2_PORT->ODR &= ~SEG_B_PIN_2;
        if (segment_pattern & 0x04) SEGMENT2_PORT->ODR &= ~SEG_C_PIN_2;
        if (segment_pattern & 0x08) SEGMENT2_PORT->ODR &= ~SEG_D_PIN_2;
        if (segment_pattern & 0x10) SEGMENT2_PORT->ODR &= ~SEG_E_PIN_2;
        if (segment_pattern & 0x20) SEGMENT2_PORT->ODR &= ~SEG_F_PIN_2;
        if (segment_pattern & 0x40) SEGMENT2_PORT->ODR &= ~SEG_G_PIN_2;
    } else {
        SEGMENT2_PORT->ODR |= (SEG_A_PIN_2 | SEG_B_PIN_2 | SEG_C_PIN_2 | SEG_D_PIN_2 | SEG_E_PIN_2 | SEG_F_PIN_2 | SEG_G_PIN_2);
    }
    delay(1000000);
}
