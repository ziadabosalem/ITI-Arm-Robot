#ifndef HAL_LCD_I2C_LCD_I2C_INT_H_
#define HAL_LCD_I2C_LCD_I2C_INT_H_


#include "../../LIB/STD_TYPES.h"


#define DDRAM_BASE_ADDR_R0       0x80
#define DDRAM_BASE_ADDR_R1       0xC0




/* Initialize LCD */
void HLCD_I2C_vInit(void);


/* Send LCD Command */
void HLCD_I2C_vSendCmd(u8 A_u8Cmd);


/* Send Character */
void HLCD_I2C_vSendChar(u8 A_u8Char);


/* Display String */
void HLCD_I2C_vDisplayString(
        const u8 A_u8Str[]
);


/* Display 16-bit Number */
void HLCD_I2C_vDisplayNumber(
        u16 A_u16Number
);


/* Display 32-bit Number */
void HLCD_I2C_vPrintNumber(
        u32 A_u32Number
);


/* Set Cursor Position */
void HLCD_I2C_vSetCursorPosition(
        u8 A_u8Row,
        u8 A_u8Col
);


/* Clear LCD */
void HLCD_I2C_vClearScreen(void);


/* Write Custom Character */
void HLCD_I2C_vWriteCGRAM(
        u8 A_u8PatternIdx,
        u8* A_pu8PatternValue
);


/* Display Custom Character */
void HLCD_I2C_vDisplayCGRAM(
        u8 A_u8PatternIdx,
        u8 A_u8Row,
        u8 A_u8Col
);


#endif /* HAL_LCD_I2C_LCD_I2C_INT_H_ */
