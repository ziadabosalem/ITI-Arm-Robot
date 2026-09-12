#ifndef HAL_LCD_I2C_LCD_I2C_CFG_H_
#define HAL_LCD_I2C_LCD_I2C_CFG_H_


/* =========================================
 * PCF8574 I2C Address
 * ========================================= */

#define LCD_I2C_ADDRESS           0x27


/* =========================================
 * LCD Data / Command
 * ========================================= */

#define LCD_I2C_COMMAND           0
#define LCD_I2C_DATA              1


/* =========================================
 * PCF8574 Pin Mapping
 * ========================================= */

#define LCD_I2C_RS                0
#define LCD_I2C_RW                1
#define LCD_I2C_EN                2
#define LCD_I2C_BACKLIGHT         3

#define LCD_I2C_D4                4
#define LCD_I2C_D5                5
#define LCD_I2C_D6                6
#define LCD_I2C_D7                7


/* =========================================
 * LCD Memory Addresses
 * ========================================= */

/* First LCD Row */
#define DDRAM_BASE_ADDR_R0        0x80

/* Second LCD Row */
#define DDRAM_BASE_ADDR_R1        0xC0

/* Custom Character Memory */
#define CGRAM_BASE_ADDR           0x40


#endif /* HAL_LCD_I2C_LCD_I2C_CFG_H_ */
