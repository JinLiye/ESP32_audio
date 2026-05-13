#ifndef WIFI_NET_H
#define WIFI_NET_H

#include <Arduino.h>

bool wifi_connect();
bool wifi_is_connected();
bool udp_send_message(const char *message);
const char *wifi_local_ip_string();
void wifi_print_status();

#endif
