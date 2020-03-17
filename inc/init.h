#ifndef INIT_H
#define INIT_H

void initializeSystem(void);
void initializeMinSystem(void);

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

//GPIOA
#define USB_SENSE_Pin LL_GPIO_PIN_15
#define USB_SENSE_GPIO_Port GPIOA
#define INIT_MASK_A (USB_SENSE_Pin)
//GPIOB
#define M_CS2_Pin LL_GPIO_PIN_4
#define M_CS2_GPIO_Port GPIOB
#define R_CS2_Pin LL_GPIO_PIN_10
#define R_CS2_GPIO_Port GPIOB
#define M_RESET_Pin LL_GPIO_PIN_13
#define M_RESET_GPIO_Port GPIOB
#define R_RESET_Pin LL_GPIO_PIN_14
#define R_RESET_GPIO_Port GPIOB
#define INIT_MASK_B (M_CS2_Pin | R_CS2_Pin | M_RESET_Pin | R_RESET_Pin)
#define INIT_MASK_B_OUT INIT_MASK_B
//GPIOC
#define R_CS3_Pin LL_GPIO_PIN_6
#define R_CS3_GPIO_Port GPIOC
#define R_D_S_Pin LL_GPIO_PIN_7
#define R_D_S_GPIO_Port GPIOC
#define R_CS0_Pin LL_GPIO_PIN_8
#define R_CS0_GPIO_Port GPIOC
#define R_CS1_Pin LL_GPIO_PIN_11
#define R_CS1_GPIO_Port GPIOC
#define L_RESET_Pin LL_GPIO_PIN_13
#define L_RESET_GPIO_Port GPIOC
#define INIT_MASK_C (R_CS3_Pin | R_D_S_Pin | R_CS0_Pin | R_CS1_Pin | L_RESET_Pin)
#define INIT_MASK_C_OUT INIT_MASK_C
//GPIOD
#define ETH_RXER_Pin LL_GPIO_PIN_5
#define ETH_RXER_GPIO_Port GPIOD
#define DSI_RESET_Pin LL_GPIO_PIN_6
#define DSI_RESET_GPIO_Port GPIOD
#define M_CS3_Pin LL_GPIO_PIN_7
#define M_CS3_GPIO_Port GPIOD
#define INIT_MASK_D (ETH_RXER_Pin | DSI_RESET_Pin | M_CS3_Pin)
#define INIT_MASK_D_OUT (DSI_RESET_Pin | M_CS3_Pin)
//GPIOE
#define L_INT_EXTI3_Pin LL_GPIO_PIN_3
#define L_INT_EXTI3_GPIO_Port GPIOE
#define M_INT_EXTI4_Pin LL_GPIO_PIN_4
#define M_INT_EXTI4_GPIO_Port GPIOE
#define BOOT_LOAD_Pin LL_GPIO_PIN_5
#define BOOT_LOAD_GPIO_Port GPIOE
#define R_INT_EXTI6_Pin LL_GPIO_PIN_6
#define R_INT_EXTI6_GPIO_Port GPIOE
#define INIT_MASK_E (                   \
    L_INT_EXTI3_Pin | M_INT_EXTI4_Pin | \
    BOOT_LOAD_Pin | R_INT_EXTI6_Pin)
//GPIOF
//GPIOG
#define DSI_INT_10_Pin LL_GPIO_PIN_10
#define DSI_INT_10_GPIO_Port GPIOG
#define ETH_RESET_Pin LL_GPIO_PIN_12
#define ETH_RESET_GPIO_Port GPIOG
#define INIT_MASK_G (DSI_INT_10_Pin | ETH_RESET_Pin)
#define INIT_MASK_G_OUT ( ETH_RESET_Pin)
//GPIOH
#define EE_CS_Pin LL_GPIO_PIN_8
#define EE_CS_GPIO_Port GPIOH
#define POE_T2P_Pin LL_GPIO_PIN_9
#define POE_T2P_GPIO_Port GPIOH
#define M_CS0_Pin LL_GPIO_PIN_10
#define M_CS0_GPIO_Port GPIOH
#define M_CS1_Pin LL_GPIO_PIN_12
#define M_CS1_GPIO_Port GPIOH
#define M_D_S_Pin LL_GPIO_PIN_15
#define M_D_S_GPIO_Port GPIOH
#define INIT_MASK_H ( \
    EE_CS_Pin | POE_T2P_Pin | M_CS0_Pin | M_CS1_Pin | M_D_S_Pin)
#define INIT_MASK_H_OUT (EE_CS_Pin | M_CS0_Pin | M_CS1_Pin | M_D_S_Pin)
//GPIOI
#define M_PWM0_F_Pin LL_GPIO_PIN_0
#define M_PWM0_F_GPIO_Port GPIOI
#define M_PWM0_R_Pin LL_GPIO_PIN_1
#define M_PWM0_R_GPIO_Port GPIOI
#define M_PWM2_F_Pin LL_GPIO_PIN_2
#define M_PWM2_F_GPIO_Port GPIOI
#define M_PWM1_R_Pin LL_GPIO_PIN_3
#define M_PWM1_R_GPIO_Port GPIOI
#define L_PWM2_F_Pin LL_GPIO_PIN_4
#define L_PWM2_F_GPIO_Port GPIOI
#define L_PWM2_R_Pin LL_GPIO_PIN_5
#define L_PWM2_R_GPIO_Port GPIOI
#define M_PWM3_R_Pin LL_GPIO_PIN_6
#define M_PWM3_R_GPIO_Port GPIOI
#define L_PWM3_R_Pin LL_GPIO_PIN_7
#define L_PWM3_R_GPIO_Port GPIOI
#define L_PWM0_F_Pin LL_GPIO_PIN_8
#define L_PWM0_F_GPIO_Port GPIOI
#define L_PWM0_R_Pin LL_GPIO_PIN_9
#define L_PWM0_R_GPIO_Port GPIOI
#define M_PWM1_F_Pin LL_GPIO_PIN_10
#define M_PWM1_F_GPIO_Port GPIOI
#define L_PWM1_R_Pin LL_GPIO_PIN_11
#define L_PWM1_R_GPIO_Port GPIOI
#define L_PWM1_F_Pin LL_GPIO_PIN_12
#define L_PWM1_F_GPIO_Port GPIOI
#define M_PWM2_R_Pin LL_GPIO_PIN_13
#define M_PWM2_R_GPIO_Port GPIOI
#define M_PWM3_F_Pin LL_GPIO_PIN_14
#define M_PWM3_F_GPIO_Port GPIOI
#define L_PWM3_F_Pin LL_GPIO_PIN_15
#define L_PWM3_F_GPIO_Port GPIOI
#define INIT_MASK_I (0xFFFF)
#define INIT_MASK_I_OUT (0xFFFF)
//GPIOJ
#define R_PWM3_R_Pin LL_GPIO_PIN_0
#define R_PWM3_R_GPIO_Port GPIOJ
#define R_PWM3_F_Pin LL_GPIO_PIN_1
#define R_PWM3_F_GPIO_Port GPIOJ
#define R_PWM2_R_Pin LL_GPIO_PIN_2
#define R_PWM2_R_GPIO_Port GPIOJ
#define R_PWM2_F_Pin LL_GPIO_PIN_3
#define R_PWM2_F_GPIO_Port GPIOJ
#define R_PWM1_R_Pin LL_GPIO_PIN_4
#define R_PWM1_R_GPIO_Port GPIOJ
#define R_PWM1_F_Pin LL_GPIO_PIN_5
#define R_PWM1_F_GPIO_Port GPIOJ
#define R_PWM0_R_Pin LL_GPIO_PIN_12
#define R_PWM0_R_GPIO_Port GPIOJ
#define R_PWM0_F_Pin LL_GPIO_PIN_13
#define R_PWM0_F_GPIO_Port GPIOJ
#define WATCHDOG_Pin LL_GPIO_PIN_14
#define WATCHDOG_GPIO_Port GPIOJ
#define LED_ALIVE_Pin LL_GPIO_PIN_15
#define LED_ALIVE_GPIO_Port GPIOJ
#define INIT_MASK_J (             \
    R_PWM3_R_Pin | R_PWM3_F_Pin | \
    R_PWM2_R_Pin | R_PWM2_F_Pin | \
    R_PWM1_R_Pin | R_PWM1_F_Pin | \
    R_PWM0_R_Pin | R_PWM0_F_Pin | \
    WATCHDOG_Pin | LED_ALIVE_Pin)
#define INIT_MASK_J_OUT INIT_MASK_J

//GPIOK
#define L_CS0_Pin LL_GPIO_PIN_3
#define L_CS0_GPIO_Port GPIOK
#define L_CS1_Pin LL_GPIO_PIN_4
#define L_CS1_GPIO_Port GPIOK
#define L_CS2_Pin LL_GPIO_PIN_5
#define L_CS2_GPIO_Port GPIOK
#define L_CS3_Pin LL_GPIO_PIN_6
#define L_CS3_GPIO_Port GPIOK
#define L_D_S_Pin LL_GPIO_PIN_7
#define L_D_S_GPIO_Port GPIOK
#define INIT_MASK_K (       \
    L_CS0_Pin | L_CS1_Pin | \
    L_CS2_Pin | L_CS3_Pin | \
    L_D_S_Pin)
#define INIT_MASK_K_OUT (       \
    L_CS0_Pin | L_CS1_Pin | \
    L_CS2_Pin | L_CS3_Pin | \
    L_D_S_Pin)

#endif