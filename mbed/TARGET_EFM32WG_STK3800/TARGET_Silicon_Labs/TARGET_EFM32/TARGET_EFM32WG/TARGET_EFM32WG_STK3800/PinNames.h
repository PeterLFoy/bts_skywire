/***************************************************************************//**
 * @file PinNames.h
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/
#ifndef MBED_PINNAMES_H
#define MBED_PINNAMES_H

#include "CommonPinNames.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EFM32_STANDARD_PIN_DEFINITIONS,

    LED0 = PC12,
    LED1 = PC13,
    LED2 = PC14,
    LED3 = LED0,
    LED4 = LED1,

    /* Push Buttons */
    SW0 = PA3,
    SW1 = PA4,
    BTN0 = SW0,
    BTN1 = SW1,

    /* Serial */
    SERIAL_TX   = PE10,
    SERIAL_RX   = PE11,
    USBTX       = PC2,
    USBRX       = PC3,

    /* Board Controller */
    STDIO_UART_TX = SERIAL_TX,
    STDIO_UART_RX = SERIAL_RX,

} PinName;

#ifdef __cplusplus
}
#endif

#endif
