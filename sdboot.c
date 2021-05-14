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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "roots.h"
#include "sdboot.h"

extern size_t strlcpy(char *dst, const char *src, size_t dsize);
extern size_t strlcat(char *dst, const char *src, size_t dsize);

bool is_boot_from_SD(void){
    bool bSDBoot = false;
    char param[1024];
    int fd, ret;
    char *s=NULL;
    printf("read cmdline\n");
    memset(param,0,1024);

    fd = open("/proc/cmdline", O_RDONLY);
    ret = read(fd, (char*)param, 1024);

    s = strstr(param,"sdfwupdate");
    if(s != NULL){
        bSDBoot = true;
        printf(">>> Boot from SDcard\n");
    }else{
        bSDBoot = false;
        printf(">>> Boot from non-SDcard\n");
    }

    close(fd);
    return bSDBoot;
}

void ensure_sd_mounted(bool *bSDMounted) {
    int i;
    for(i = 0; i < 3; i++) {
        if(0 == ensure_path_mounted(EX_SDCARD_ROOT)){
            *bSDMounted = true;
            break;
        }else {
            printf("delay 1sec\n");
            sleep(1);
        }
    }

    if (i == 3)
        *bSDMounted = false;
}

#define MaxLine 1024

//»ñÈ¡ÅäÖÃÏî
int get_cfg_Item(char *pFileName /*in*/, char *pKey /*in*/,
                  char * pValue/*in out*/, int * pValueLen /*out*/)
{
    int     ret = 0;
    FILE    *fp = NULL;
    char    *pTmp = NULL, *pEnd = NULL, *pBegin = NULL;

    char lineBuf[MaxLine];
    while(1) {
	fp = fopen(pFileName, "r");
        if (fp != NULL) break;
    }	
    while (!feof(fp)) {
        memset(lineBuf, 0, sizeof(lineBuf));
        fgets(lineBuf, MaxLine, fp);
        printf("lineBuf: %s ", lineBuf);

        pTmp = strchr(lineBuf, '=');
        if (pTmp == NULL)
            continue;

        pTmp = strstr(lineBuf, pKey);
        if (pTmp == NULL)
            continue;

        pTmp = pTmp + strlen(pKey);
        pTmp = strchr(pTmp, '=');
        if (pTmp == NULL)
            continue;

        pTmp = pTmp + 1;

        while (1) {
            if (*pTmp == ' ') {
                pTmp ++ ;
            } else {
                pBegin = pTmp;
                if (*pBegin == '\n') {
                    goto End;
                }
                break;
            }
        }

        while (1) {
            if ((*pTmp == ' ' || *pTmp == '\n'))
                break;
            else
                pTmp ++;
        }
        pEnd = pTmp;

        *pValueLen = pEnd-pBegin;
        memcpy(pValue, pBegin, pEnd-pBegin);
    }

End:
    if (fp == NULL)
        fclose(fp);

    return 0;
}

bool is_sdcard_update(void) {
    int  ret = 0;
    bool bSdMounted = false;
    char configFile[64] = {0};
    int vlen = 0;
    char str_val[10] = {0};
    char *str_key = "fw_update";

    printf("%s in\n",__func__);
    ensure_sd_mounted(&bSdMounted);
    if (!bSdMounted) {
        printf("Error! SDcard not mounted\n");
        return false;
    }

    strlcpy(configFile, EX_SDCARD_ROOT, sizeof(configFile));
    strlcat(configFile, "/sd_boot_config.config", sizeof(configFile));
    printf("configFile = %s \n", configFile);
    ret = get_cfg_Item(configFile, str_key, str_val, &vlen);

    if(ret != 0) {
        printf("func get_cfg_Item err:%d \n", ret);
        return false;
    }

    printf("\n %s:%s \n", str_key, str_val);

    if(strcmp(str_val, "1") != 0) {
        return false;
    }

    printf("firmware update will from SDCARD. \n");
    printf("%s out\n",__func__);
    return true;
}
#if 1
void *thrd_yellow_led_func(void *arg) {
        FILE * ledFd = NULL;
        bool onoff = false;

        while(isLedFlash) {
                ledFd = fopen("/sys/class/leds/firefly:yellow:user/brightness", "w");
				if (ledFd == NULL){
					ledFd = fopen("/sys/class/leds/firefly:blue:diy/brightness", "w");
				}
                if(onoff) {
                        fprintf(ledFd, "%d", 0);
                        onoff = false;
                }else {
                        fprintf(ledFd, "%d", 1);
                        onoff = true;
                }

                fclose(ledFd);
                usleep(500 * 1000);
        }

        printf("stopping led thread, close led and exit\n");
        ledFd = fopen("/sys/class/leds/firefly:yellow:user/brightness", "w");
		if (ledFd == NULL){
			ledFd = fopen("/sys/class/leds/firefly:blue:diy/brightness", "w");
		}
        fprintf(ledFd, "%d", 0);
        fclose(ledFd);
        pthread_exit(NULL);
        return NULL;
}

void *thrd_blue_led_func(void *arg) {
        FILE * ledFd = NULL;
        bool onoff = false;

        while(isLedFlash) {
                ledFd = fopen("/sys/class/leds/firefly:blue:power/brightness", "w");
				if (ledFd == NULL){
					ledFd = fopen("/sys/class/leds/firefly:red:power/brightness", "w");
				}
                if(onoff) {
                        fprintf(ledFd, "%d", 0);
                        onoff = false;
                }else {
                        fprintf(ledFd, "%d", 1);
                        onoff = true;
                }

                fclose(ledFd);
                usleep(500 * 1000);
        }

        printf("stopping led thread, close led and exit\n");
        ledFd = fopen("/sys/class/leds/firefly:blue:power/brightness", "w");
		if (ledFd == NULL){
			ledFd = fopen("/sys/class/leds/firefly:red:power/brightness", "w");
		}
        fprintf(ledFd, "%d", 0);
        fclose(ledFd);
        pthread_exit(NULL);
        return NULL;
}

void startLedBlink(int led) {
        isLedFlash = true;
        if(led == YELLOW) {
                if (pthread_create(&tid_yellow_led,NULL,thrd_yellow_led_func,NULL)!=0) {
                        printf("Create led thread error!\n");
                }
                printf("tid in led pthread: %u\n",tid_yellow_led);
        }
        else if(led == BLUE) {
                if (pthread_create(&tid_blue_led,NULL,thrd_blue_led_func,NULL)!=0) {
                        printf("Create led thread error!\n");
                }
                printf("tid in led pthread: %u\n",tid_blue_led);
        }
}

void startLed(int led) {
        FILE * ledFd = NULL;
        if(led == YELLOW) {
                ledFd = fopen("/sys/class/leds/firefly:yellow:user/brightness", "w");
				if (ledFd == NULL){
					ledFd = fopen("/sys/class/leds/firefly:blue:diy/brightness", "w");
				}
                fprintf(ledFd, "%d", 1);
                fclose(ledFd);
        }
        else if (led == BLUE) {
                ledFd = fopen("/sys/class/leds/firefly:blue:power/brightness", "w");
				if (ledFd == NULL){
					ledFd = fopen("/sys/class/leds/firefly:red:power/brightness", "w");
				}
                fprintf(ledFd, "%d", 1);
                fclose(ledFd);
        }
}
void stopLed(int led) {
        FILE * ledFd = NULL;
        if(led == YELLOW) {
                ledFd = fopen("/sys/class/leds/firefly:yellow:user/brightness", "w");
				if (ledFd == NULL){
					ledFd = fopen("/sys/class/leds/firefly:blue:diy/brightness", "w");
				}
                fprintf(ledFd, "%d", 0);
                fclose(ledFd);
        }
        else if (led == BLUE) {
                ledFd = fopen("/sys/class/leds/firefly:blue:power/brightness", "w");
				if (ledFd == NULL){
					ledFd = fopen("/sys/class/leds/firefly:red:power/brightness", "w");
				}
                fprintf(ledFd, "%d", 0);
                fclose(ledFd);
        }
}
void stopLedBlink(int led) {
        void *tret;
        isLedFlash = false;

        if (led == YELLOW) {
                if (pthread_join(tid_yellow_led, &tret)!=0){
                        printf("Join led thread error!\n");
                }else {
                        printf("join led thread success!\n");
                }
        } else if (led == BLUE) {
                if (pthread_join(tid_blue_led, &tret)!=0){
                        printf("Join led thread error!\n");
                }else {
                        printf("join led thread success!\n");
                }
        }
}
#endif

