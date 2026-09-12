#ifndef EEPROM_INT_H_
#define EEPROM_INT_H_

#include "../../LIB/STD_TYPES.h"


/* =========================================
 * INTERNAL EEPROM APIs
 * ========================================= */

void MEEPROM_vWriteByte(
        u16 Copy_u16Address,
        u8 Copy_u8Data
);


u8 MEEPROM_u8ReadByte(
        u16 Copy_u16Address
);


/* =========================================
 * 16-BIT FUNCTIONS
 *
 * We will need these later for PCA ticks:
 *
 * 92
 * 307
 * 522
 * etc...
 * ========================================= */

void MEEPROM_vWriteWord(
        u16 Copy_u16Address,
        u16 Copy_u16Data
);


u16 MEEPROM_u16ReadWord(
        u16 Copy_u16Address
);


#endif
