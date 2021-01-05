/**
 * Copyright (C) 2018 Fuzhou Rockchip Electronics Co., Ltd
 * author: Chad.ma <Chad.ma@rock-chips.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SDBOOT_H_
#define _SDBOOT_H_

#define EX_SDCARD_ROOT  "/mnt/sdcard"

#ifdef  __cplusplus
extern "C" {
#endif

bool is_boot_from_SD(void);
void ensure_sd_mounted(bool *bSDMounted);
int get_cfg_Item(char *pFileName /*in*/, char *pKey /*in*/,
                  char * pValue/*in out*/, int * pValueLen /*out*/);
bool is_sdcard_update(void);

#define YELLOW 1
#define BLUE 2
static pthread_t tid_yellow_led;
static pthread_t tid_blue_led;
static bool isLedFlash = false;
void *thrd_yellow_led_func(void *arg);
void *thrd_blue_led_func(void *arg);
void startLedBlink(int led);
void startLed(int led);
void stopLed(int led);
void stopLedBlink(int led);

#ifdef  __cplusplus
}
#endif

#endif  //_SDBOOT_H_
