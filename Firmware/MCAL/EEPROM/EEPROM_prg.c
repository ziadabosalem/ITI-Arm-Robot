#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../LIB/STD_TYPES.h"

#include "EEPROM_int.h"
#include "EEPROM_prv.h"


/* =====================================================
 * WRITE BYTE
 * ===================================================== */

void MEEPROM_vWriteByte(
        u16 Copy_u16Address,
        u8 Copy_u8Data
)
{
    u8 Local_u8SREG;


    /* =====================================
     * CHECK ADDRESS
     * ===================================== */

    if(Copy_u16Address > EEPROM_MAX_ADDRESS)
    {
        return;
    }


    /* =====================================
     * WAIT FOR PREVIOUS EEPROM WRITE
     * ===================================== */

    while(
        EECR & (1 << EEWE)
    )
    {
        /* Wait */
    }


    /* =====================================
     * WAIT FOR SELF PROGRAMMING
     * ===================================== */

#ifdef SPMEN

    while(
        SPMCR & (1 << SPMEN)
    )
    {
        /* Wait */
    }

#endif


    /* =====================================
     * SET ADDRESS
     * ===================================== */

    EEAR =
            Copy_u16Address;


    /* =====================================
     * SET DATA
     * ===================================== */

    EEDR =
            Copy_u8Data;


    /* =====================================
     * SAVE INTERRUPT STATE
     * ===================================== */

    Local_u8SREG =
            SREG;


    cli();


    /* =====================================
     * CRITICAL EEPROM WRITE SEQUENCE
     *
     * IMPORTANT:
     *
     * EEMWE -> EEWE must happen within
     * 4 CPU clock cycles.
     *
     * We use SBI instructions directly
     * so compiler optimization cannot
     * destroy the timing.
     * ===================================== */

    __asm__ __volatile__
    (
        "sbi %0, %1" "\n\t"
        "sbi %0, %2"
        :
        : "I" (_SFR_IO_ADDR(EECR)),
          "I" (EEMWE),
          "I" (EEWE)
    );


    /* =====================================
     * RESTORE INTERRUPT STATE
     * ===================================== */

    SREG =
            Local_u8SREG;


    /* =====================================
     * WAIT UNTIL WRITE REALLY FINISHES
     *
     * Important for our test.
     * ===================================== */

    while(
        EECR & (1 << EEWE)
    )
    {
        /* Wait */
    }
}


/* =====================================================
 * READ BYTE
 * ===================================================== */

u8 MEEPROM_u8ReadByte(
        u16 Copy_u16Address
)
{
    if(Copy_u16Address > EEPROM_MAX_ADDRESS)
    {
        return 0U;
    }


    /* =====================================
     * WAIT FOR WRITE COMPLETION
     * ===================================== */

    while(
        EECR & (1 << EEWE)
    )
    {
        /* Wait */
    }


    /* =====================================
     * SET ADDRESS
     * ===================================== */

    EEAR =
            Copy_u16Address;


    /* =====================================
     * START EEPROM READ
     * ===================================== */

    EECR |=
            (1 << EERE);


    /* =====================================
     * RETURN DATA
     * ===================================== */

    return EEDR;
}


/* =====================================================
 * WRITE 16 BIT
 * ===================================================== */

void MEEPROM_vWriteWord(
        u16 Copy_u16Address,
        u16 Copy_u16Data
)
{
    u8 Local_u8LowByte;
    u8 Local_u8HighByte;


    /*
     * Need Address and Address + 1
     */

    if(Copy_u16Address >= EEPROM_MAX_ADDRESS)
    {
        return;
    }


    Local_u8LowByte =
            (u8)(
                Copy_u16Data &
                0x00FFU
            );


    Local_u8HighByte =
            (u8)(
                (Copy_u16Data >> 8)
                &
                0x00FFU
            );


    /* =====================================
     * LOW BYTE
     * ===================================== */

    MEEPROM_vWriteByte(
            Copy_u16Address,
            Local_u8LowByte
    );


    /* =====================================
     * HIGH BYTE
     * ===================================== */

    MEEPROM_vWriteByte(
            Copy_u16Address + 1U,
            Local_u8HighByte
    );
}


/* =====================================================
 * READ 16 BIT
 * ===================================================== */

u16 MEEPROM_u16ReadWord(
        u16 Copy_u16Address
)
{
    u8 Local_u8LowByte;
    u8 Local_u8HighByte;

    u16 Local_u16Data;


    if(Copy_u16Address >= EEPROM_MAX_ADDRESS)
    {
        return 0U;
    }


    Local_u8LowByte =
            MEEPROM_u8ReadByte(
                    Copy_u16Address
            );


    Local_u8HighByte =
            MEEPROM_u8ReadByte(
                    Copy_u16Address + 1U
            );


    Local_u16Data =
            (u16)Local_u8LowByte;


    Local_u16Data |=
            (
                (u16)Local_u8HighByte
                <<
                8
            );


    return Local_u16Data;
}
