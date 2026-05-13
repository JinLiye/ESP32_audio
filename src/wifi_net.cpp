#include "wifi_net.h"

#include <WiFi.h>
#include <WiFiUdp.h>

#include "config.h"

static WiFiUDP udp_client;

static const char *wifi_status_text(wl_status_t status) {
  switch (status) {
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED:
      return "WL_SCAN_COMPLETED";
    case WL_CONNECTED:
      return "WL_CONNECTED";
    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
      return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";
    default:
      return "WL_UNKNOWN";
  }
}

bool wifi_connect() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("WiFi connecting to: ");
  Serial.println(WIFI_SSID);

  unsigned long start_time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start_time > 10000) {
      Serial.print("WiFi connect timeout, status: ");
      Serial.println(wifi_status_text(WiFi.status()));
      return false;
    }
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("WiFi connected, local IP: ");
  Serial.println(WiFi.localIP());

  return true;
}

bool wifi_is_connected() {
  return WiFi.status() == WL_CONNECTED;
}

bool udp_send_message(const char *message) {
  if (message == NULL) {
    return false;
  }

  if (!wifi_connect()) {
    return false;
  }

  Serial.print("UDP target: ");
  Serial.print(UDP_TARGET_IP);
  Serial.print(":");
  Serial.println(UDP_TARGET_PORT);

  if (!udp_client.beginPacket(UDP_TARGET_IP, UDP_TARGET_PORT)) {
    return false;
  }

  udp_client.print(message);
  return udp_client.endPacket() == 1;
}

const char *wifi_local_ip_string() {
  static String ip_text;
  ip_text = WiFi.localIP().toString();
  return ip_text.c_str();
}

void wifi_print_status() {
  Serial.print("WiFi status: ");
  Serial.println(wifi_status_text(WiFi.status()));

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  }
}
