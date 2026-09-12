/*
 * KPD_prg.c
 *
 * Created on: Jul 26, 2026
 *
 * Author: Hager Adel
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include <util/delay.h>

#include "../../MCAL/DIO/DIO_int.h"

#include "KPD_cfg.h"
#include "KPD_int.h"


/* =====================================================
 * Keypad Initialization
 * ===================================================== */

void HKPD_vInit(void)
{
    u8 i;


    /* =============================================
     * Configure Columns as OUTPUT
     *
     * PB0 -> Column 1
     * PB1 -> Column 2
     * PB2 -> Column 3
     * ============================================= */

    for(i = COL_INIT;
        i <= COL_END;
        i++)
    {
        MDIO_vSetPinDir(
                KPD_COL_PORT,
                i,
                DIO_OUTPUT
        );


        /*
         * All columns inactive initially.
         *
         * HIGH = inactive
         */
        MDIO_vSetPinVal(
                KPD_COL_PORT,
                i,
                DIO_HIGH
        );
    }


    /* =============================================
     * Configure Rows as INPUT
     *
     * PB4 -> Row 1
     * PB5 -> Row 2
     * PB6 -> Row 3
     * ============================================= */

    for(i = ROW_INIT;
        i <= ROW_END;
        i++)
    {
        MDIO_vSetPinDir(
                KPD_ROW_PORT,
                i,
                DIO_INPUT
        );


        /*
         * Enable internal pull-up resistor.
         */
        MDIO_vSetPinVal(
                KPD_ROW_PORT,
                i,
                DIO_HIGH
        );
    }
}


/* =====================================================
 * Normal Key Press Function
 *
 * Used when we want:
 *
 * Press
 *   ↓
 * Detect Key
 *   ↓
 * Wait until release
 *   ↓
 * Return key once
 *
 *
 * Good for:
 *
 * 1 -> Select J1
 * 2 -> Select J2
 * ...
 * 6 -> Select J6
 * 9 -> HOME
 * ===================================================== */

u8 HKPD_u8GetPressedKey(
        const u8 KeyMap[KPD_ROWS][KPD_COLS]
)
{
    u8 col;
    u8 row;


    /* =============================================
     * Scan all columns one by one
     * ============================================= */

    for(col = COL_INIT;
        col <= COL_END;
        col++)
    {
        /*
         * Activate current column.
         *
         * LOW = active
         */
        MDIO_vSetPinVal(
                KPD_COL_PORT,
                col,
                DIO_LOW
        );


        /* =========================================
         * Check all rows
         * ========================================= */

        for(row = ROW_INIT;
            row <= ROW_END;
            row++)
        {
            /*
             * If row becomes LOW,
             * a key is pressed.
             */
            if(
                MDIO_u8GetPinVal(
                    KPD_ROW_PORT,
                    row
                )
                ==
                DIO_LOW
            )
            {
                /*
                 * Debouncing.
                 */
                _delay_ms(20);


                /*
                 * Confirm the key is still pressed.
                 */
                if(
                    MDIO_u8GetPinVal(
                        KPD_ROW_PORT,
                        row
                    )
                    ==
                    DIO_LOW
                )
                {
                    /*
                     * Wait until user releases
                     * the key.
                     */
                    while(
                        MDIO_u8GetPinVal(
                            KPD_ROW_PORT,
                            row
                        )
                        ==
                        DIO_LOW
                    );


                    /*
                     * Restore current column HIGH
                     * before returning.
                     */
                    MDIO_vSetPinVal(
                            KPD_COL_PORT,
                            col,
                            DIO_HIGH
                    );


                    /*
                     * Return pressed key.
                     */
                    return
                        KeyMap
                        [row - ROW_INIT]
                        [col - COL_INIT];
                }
            }
        }


        /*
         * Deactivate current column
         * before scanning next column.
         */
        MDIO_vSetPinVal(
                KPD_COL_PORT,
                col,
                DIO_HIGH
        );
    }


    /*
     * No key pressed.
     */
    return NO_KEY;
}


/* =====================================================
 * Held Key Function
 *
 * REAL-TIME / FAST FUNCTION
 *
 * Used when we want:
 *
 * Hold 7
 *   ↓
 * 7
 * 7
 * 7
 * 7
 * 7
 * ...
 *
 * Hold 8
 *   ↓
 * 8
 * 8
 * 8
 * 8
 * ...
 *
 *
 * IMPORTANT:
 *
 * - No wait for release.
 * - No 20 ms debounce.
 * - No while loop.
 *
 * This makes it suitable for fast robot JOG control.
 * ===================================================== */

u8 HKPD_u8GetHeldKey(
        const u8 KeyMap[KPD_ROWS][KPD_COLS]
)
{
    u8 col;
    u8 row;


    /* =============================================
     * Scan Columns
     * ============================================= */

    for(col = COL_INIT;
        col <= COL_END;
        col++)
    {
        /*
         * Activate current column.
         */
        MDIO_vSetPinVal(
                KPD_COL_PORT,
                col,
                DIO_LOW
        );


        /* =========================================
         * Check Rows
         * ========================================= */

        for(row = ROW_INIT;
            row <= ROW_END;
            row++)
        {
            /*
             * If row LOW,
             * key is currently being held.
             */
            if(
                MDIO_u8GetPinVal(
                    KPD_ROW_PORT,
                    row
                )
                ==
                DIO_LOW
            )
            {
                /*
                 * Restore column HIGH
                 * before returning.
                 */
                MDIO_vSetPinVal(
                        KPD_COL_PORT,
                        col,
                        DIO_HIGH
                );


                /*
                 * Return immediately.
                 *
                 * NO debounce delay.
                 * NO wait for release.
                 */
                return
                    KeyMap
                    [row - ROW_INIT]
                    [col - COL_INIT];
            }
        }


        /*
         * Deactivate current column
         * before scanning next one.
         */
        MDIO_vSetPinVal(
                KPD_COL_PORT,
                col,
                DIO_HIGH
        );
    }


    /*
     * Nothing currently pressed.
     */
    return NO_KEY;
}
