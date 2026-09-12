#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/TWI/TWI_int.h"

#include "LCD_I2C_cfg.h"
#include "LCD_I2C_prv.h"
#include "LCD_I2C_int.h"


/* =========================================================
 * Private Functions Prototypes
 * ========================================================= */

static void HLCD_I2C_vDelay(void);

static void HLCD_I2C_vWriteExpander(
        u8 A_u8Data
);

static void HLCD_I2C_vSendNibble(
        u8 A_u8Nibble
);

static void HLCD_I2C_vSendByte(
        u8 A_u8Byte,
        u8 A_u8RS
);


/* =========================================================
 * Simple Delay
 * ========================================================= */

static void HLCD_I2C_vDelay(void)
{
    volatile u32 Local_u32Counter;

    for(Local_u32Counter = 0;
        Local_u32Counter < 50000UL;
        Local_u32Counter++)
    {
        /* Do Nothing */
    }
}


/* =========================================================
 * Write One Byte To PCF8574
 *
 * Every transaction:
 *
 * START
 * 0x27 + WRITE
 * DATA
 * STOP
 *
 * This allows LCD and PCA9685 to share the same TWI bus.
 * ========================================================= */

static void HLCD_I2C_vWriteExpander(
        u8 A_u8Data
)
{
    /* Send START */
    MTWI_u8SendStartCondition();


    /* Send LCD I2C Address + WRITE */
    MTWI_u8SendSLA_RW(
            LCD_I2C_ADDRESS,
            TWI_WRITE
    );


    /* Send Data */
    MTWI_u8SendByte(
            A_u8Data
    );


    /* Send STOP */
    MTWI_u8SentStopCondition();
}


/* =========================================================
 * Send One 4-bit Nibble To LCD
 * ========================================================= */

static void HLCD_I2C_vSendNibble(
        u8 A_u8Nibble
)
{
    u8 Local_u8Data;


    Local_u8Data = A_u8Nibble;


    /* RW = 0 -> Write Mode */
    CLR_BIT(
            Local_u8Data,
            LCD_I2C_RW
    );


    /* Backlight ON */
    SET_BIT(
            Local_u8Data,
            LCD_I2C_BACKLIGHT
    );


    /* =====================================
     * EN = 1
     * ===================================== */

    SET_BIT(
            Local_u8Data,
            LCD_I2C_EN
    );

    HLCD_I2C_vWriteExpander(
            Local_u8Data
    );


    /* =====================================
     * EN = 0
     *
     * Falling edge allows LCD
     * to read the nibble.
     * ===================================== */

    CLR_BIT(
            Local_u8Data,
            LCD_I2C_EN
    );

    HLCD_I2C_vWriteExpander(
            Local_u8Data
    );
}


/* =========================================================
 * Send Complete 8-bit Byte
 *
 * LCD works in 4-bit mode:
 *
 * High Nibble
 * then
 * Low Nibble
 * ========================================================= */

static void HLCD_I2C_vSendByte(
        u8 A_u8Byte,
        u8 A_u8RS
)
{
    u8 Local_u8HighNibble;
    u8 Local_u8LowNibble;


    /* Get High Nibble */
    Local_u8HighNibble =
            A_u8Byte & 0xF0;


    /* Move Low Nibble To D4-D7 */
    Local_u8LowNibble =
            (A_u8Byte << 4) & 0xF0;


    /* =====================================
     * DATA
     * RS = 1
     * ===================================== */

    if(A_u8RS == LCD_I2C_DATA)
    {
        SET_BIT(
                Local_u8HighNibble,
                LCD_I2C_RS
        );

        SET_BIT(
                Local_u8LowNibble,
                LCD_I2C_RS
        );
    }


    /* =====================================
     * COMMAND
     * RS = 0
     * ===================================== */

    else
    {
        CLR_BIT(
                Local_u8HighNibble,
                LCD_I2C_RS
        );

        CLR_BIT(
                Local_u8LowNibble,
                LCD_I2C_RS
        );
    }


    /* Send High Nibble */
    HLCD_I2C_vSendNibble(
            Local_u8HighNibble
    );


    /* Send Low Nibble */
    HLCD_I2C_vSendNibble(
            Local_u8LowNibble
    );
}


/* =========================================================
 * LCD Initialization
 * ========================================================= */

void HLCD_I2C_vInit(void)
{
    /* Power-up delay */
    HLCD_I2C_vDelay();


    /* =====================================
     * LCD Initialization Sequence
     * ===================================== */

    HLCD_I2C_vSendNibble(
            0x30
    );

    HLCD_I2C_vDelay();


    HLCD_I2C_vSendNibble(
            0x30
    );

    HLCD_I2C_vDelay();


    HLCD_I2C_vSendNibble(
            0x30
    );

    HLCD_I2C_vDelay();


    /* =====================================
     * Switch To 4-bit Mode
     * ===================================== */

    HLCD_I2C_vSendNibble(
            0x20
    );

    HLCD_I2C_vDelay();


    /* =====================================
     * Function Set
     *
     * 4-bit
     * 2 Lines
     * 5x8 Font
     * ===================================== */

    HLCD_I2C_vSendByte(
            0x28,
            LCD_I2C_COMMAND
    );

    HLCD_I2C_vDelay();


    /* =====================================
     * Display ON
     * Cursor OFF
     * Blink OFF
     * ===================================== */

    HLCD_I2C_vSendByte(
            0x0C,
            LCD_I2C_COMMAND
    );

    HLCD_I2C_vDelay();


    /* =====================================
     * Clear Screen
     * ===================================== */

    HLCD_I2C_vSendByte(
            0x01,
            LCD_I2C_COMMAND
    );

    HLCD_I2C_vDelay();


    /* =====================================
     * Entry Mode
     *
     * Increment Cursor
     * No Display Shift
     * ===================================== */

    HLCD_I2C_vSendByte(
            0x06,
            LCD_I2C_COMMAND
    );

    HLCD_I2C_vDelay();
}


/* =========================================================
 * Send Command
 * ========================================================= */

void HLCD_I2C_vSendCmd(
        u8 A_u8Cmd
)
{
    HLCD_I2C_vSendByte(
            A_u8Cmd,
            LCD_I2C_COMMAND
    );


    HLCD_I2C_vDelay();
}


/* =========================================================
 * Send Character
 * ========================================================= */

void HLCD_I2C_vSendChar(
        u8 A_u8Char
)
{
    HLCD_I2C_vSendByte(
            A_u8Char,
            LCD_I2C_DATA
    );
}


/* =========================================================
 * Display String
 * ========================================================= */

void HLCD_I2C_vDisplayString(
        const u8 A_u8Str[]
)
{
    u8 Local_u8Counter = 0;


    while(
        A_u8Str[Local_u8Counter]
        != '\0'
    )
    {
        HLCD_I2C_vSendChar(
                A_u8Str[
                    Local_u8Counter
                ]
        );


        Local_u8Counter++;
    }
}


/* =========================================================
 * Display u16 Number
 * ========================================================= */

void HLCD_I2C_vDisplayNumber(
        u16 A_u16Number
)
{
    HLCD_I2C_vPrintNumber(
            (u32)A_u16Number
    );
}


/* =========================================================
 * Display u32 Number
 *
 * Example:
 *
 * 5   -> 5
 * 90  -> 90
 * 120 -> 120
 *
 * No leading zeros.
 * ========================================================= */

void HLCD_I2C_vPrintNumber(
        u32 A_u32Number
)
{
    u8 Local_u8Digits[10];

    u8 Local_u8Counter = 0;


    /* Special Case: Zero */
    if(A_u32Number == 0)
    {
        HLCD_I2C_vSendChar(
                '0'
        );

        return;
    }


    /* Store Digits Reversed */
    while(A_u32Number > 0)
    {
        Local_u8Digits[
                Local_u8Counter
        ] =
                (u8)(
                    A_u32Number % 10
                );


        A_u32Number =
                A_u32Number / 10;


        Local_u8Counter++;
    }


    /* Display Digits Correctly */
    while(Local_u8Counter > 0)
    {
        Local_u8Counter--;


        HLCD_I2C_vSendChar(
                Local_u8Digits[
                    Local_u8Counter
                ]
                +
                '0'
        );
    }
}


/* =========================================================
 * Set Cursor Position
 *
 * Row 0 = First Row
 * Row 1 = Second Row
 * ========================================================= */

void HLCD_I2C_vSetCursorPosition(
        u8 A_u8Row,
        u8 A_u8Col
)
{
    u8 Local_u8Address;


    if(A_u8Row == 0)
    {
        Local_u8Address =
                DDRAM_BASE_ADDR_R0
                +
                A_u8Col;
    }

    else
    {
        Local_u8Address =
                DDRAM_BASE_ADDR_R1
                +
                A_u8Col;
    }


    HLCD_I2C_vSendCmd(
            Local_u8Address
    );
}


/* =========================================================
 * Clear Screen
 * ========================================================= */

void HLCD_I2C_vClearScreen(void)
{
    HLCD_I2C_vSendCmd(
            0x01
    );


    HLCD_I2C_vDelay();
}


/* =========================================================
 * Write Custom Character To CGRAM
 *
 * Pattern Index:
 *
 * 0 -> 7
 *
 * Each Pattern:
 *
 * 8 Bytes
 * ========================================================= */

void HLCD_I2C_vWriteCGRAM(
        u8 A_u8PatternIdx,
        u8* A_pu8PatternValue
)
{
    u8 Local_u8Counter;


    if(A_pu8PatternValue == NULL)
    {
        return;
    }


    if(A_u8PatternIdx > 7)
    {
        return;
    }


    /* Select CGRAM Address */
    HLCD_I2C_vSendCmd(
            CGRAM_BASE_ADDR
            +
            (
                A_u8PatternIdx
                *
                8
            )
    );


    /* Write 8 Bytes */
    for(
        Local_u8Counter = 0;
        Local_u8Counter < 8;
        Local_u8Counter++
    )
    {
        HLCD_I2C_vSendChar(
                A_pu8PatternValue[
                    Local_u8Counter
                ]
        );
    }
}


/* =========================================================
 * Display Custom Character
 * ========================================================= */

void HLCD_I2C_vDisplayCGRAM(
        u8 A_u8PatternIdx,
        u8 A_u8Row,
        u8 A_u8Col
)
{
    if(A_u8PatternIdx > 7)
    {
        return;
    }


    HLCD_I2C_vSetCursorPosition(
            A_u8Row,
            A_u8Col
    );


    HLCD_I2C_vSendChar(
            A_u8PatternIdx
    );
}
