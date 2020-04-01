/* Declarations of command registration functions.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern char* ssid;
extern char* passwd;
extern char* ap_ssid;
extern char* ap_passwd;

void preprocess_string(char* str);
int set_sta(int argc, char **argv);
int set_ap(int argc, char **argv);
int save_ip_addr(int *ipabcdn);
int set_staAlt(int argc, char **argv, int stanum);
int set_LFCP(int argc, char **argv, int stanum); // LFCP set server A, server B and udp port info
void LFCPsend();
void LFCPsendlog(char *LFCPmessage);

httpd_handle_t start_webserver(void);

#ifdef __cplusplus
}
#endif
