/*
 * KPD_int.h
 *
 *  Created on: Jul 26, 2026
 *      Author: Hager Adel
 */
#include "../../LIB/STD_TYPES.h"
#ifndef HAL_KPD_KPD_INT_H_
#define HAL_KPD_KPD_INT_H_

#define KPD_ROWS       3
#define KPD_COLS       3

#define NO_KEY         0xFF
void HKPD_vInit(void);

u8 HKPD_u8GetPressedKey(
        const u8 KeyMap[KPD_ROWS][KPD_COLS]
);

u8 HKPD_u8GetHeldKey(
        const u8 KeyMap[KPD_ROWS][KPD_COLS]
);


#endif /* HAL_KPD_KPD_INT_H_ */
