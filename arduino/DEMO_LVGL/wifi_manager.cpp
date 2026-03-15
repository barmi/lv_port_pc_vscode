#ifdef ARDUINO
#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <time.h>
#include "wifi_manager.h"

static Preferences preferences;
static wifi_status_t current_status = WIFI_STATUS_DISCONNECTED;
static String current_ssid = "";
static String saved_ssid = "";
static String saved_password = "";
static bool is_scanning = false;
static int scan_count = 0;

void wifi_mgr_init() {
    preferences.begin("wifi_mgr", false);
    saved_ssid = preferences.getString("ssid", "");
    saved_password = preferences.getString("password", "");
    
    if (saved_ssid.length() > 0) {
        Serial.printf("Auto-connecting to: %s\n", saved_ssid.c_str());
        WiFi.begin(saved_ssid.c_str(), saved_password.c_str());
        current_status = WIFI_STATUS_CONNECTING;
    }

    // SNTP 초기화 (KST: UTC+9)
    configTime(9 * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void wifi_mgr_loop() {
    if (is_scanning) {
        int r = WiFi.scanComplete();
        if (r >= 0) {
            scan_count = r;
            is_scanning = false;
        }
    }

    wl_status_t wl_status = WiFi.status();
    if (wl_status == WL_CONNECTED) {
        if (current_status != WIFI_STATUS_CONNECTED) {
            current_status = WIFI_STATUS_CONNECTED;
            current_ssid = WiFi.SSID();
            
            // 성공 시 저장
            if (current_ssid != saved_ssid) {
                preferences.putString("ssid", current_ssid);
                preferences.putString("password", WiFi.psk()); // Note: WiFi.psk() might not always work depending on how it was set
                // If connecting manually, we should save from the connect params
                saved_ssid = current_ssid;
            }
        }
    } else if (wl_status == WL_CONNECT_FAILED || wl_status == WL_NO_SSID_AVAIL) {
        current_status = WIFI_STATUS_CONNECT_FAILED;
    } else if (wl_status == WL_IDLE_STATUS || wl_status == WL_DISCONNECTED) {
        if (current_status == WIFI_STATUS_CONNECTED) {
            current_status = WIFI_STATUS_DISCONNECTED;
        }
    }
}

void wifi_mgr_scan_start() {
    WiFi.scanNetworks(true); // 비동기 스캔
    is_scanning = true;
    scan_count = 0;
}

bool wifi_mgr_is_scanning() {
    return is_scanning;
}

int wifi_mgr_get_scan_count() {
    return scan_count;
}

const char* wifi_mgr_get_scan_ssid(int index) {
    if (index < 0 || index >= scan_count) return "";
    // WiFi.SSID() returns a String object which is destroyed after the statement.
    // We need to return a pointer that is valid at least for the caller's immediate use.
    static String ssid_tmp;
    ssid_tmp = WiFi.SSID(index);
    return ssid_tmp.c_str();
}

int wifi_mgr_get_scan_rssi(int index) {
    if (index < 0 || index >= scan_count) return 0;
    return WiFi.RSSI(index);
}

void wifi_mgr_connect(const char* ssid, const char* password) {
    Serial.printf("Connecting to %s...\n", ssid);
    WiFi.begin(ssid, password);
    current_status = WIFI_STATUS_CONNECTING;
    
    // 연결 시도 시 정보 미리 저장 (나중에 성공 확인 후 확정)
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    saved_ssid = ssid;
    saved_password = password;
}

wifi_status_t wifi_mgr_get_status() {
    return current_status;
}

const char* wifi_mgr_get_current_ssid() {
    return current_ssid.c_str();
}

const char* wifi_mgr_get_saved_ssid() {
    return saved_ssid.c_str();
}

const char* wifi_mgr_get_ip() {
    static String ip_str;
    if (WiFi.status() == WL_CONNECTED) {
        ip_str = WiFi.localIP().toString();
    } else {
        ip_str = "0.0.0.0";
    }
    return ip_str.c_str();
}

const char* wifi_mgr_get_time() {
    static char time_buf[64];
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        return "시간 동기화 안됨";
    }
    // format: 2024-03-20 14:30:05
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return time_buf;
}

#else // !ARDUINO

#include "wifi_manager.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

void wifi_mgr_init() {}
void wifi_mgr_loop() {}
void wifi_mgr_scan_start() {}
bool wifi_mgr_is_scanning() { return false; }
int wifi_mgr_get_scan_count() { return 0; }
const char* wifi_mgr_get_scan_ssid(int index) { return ""; }
int wifi_mgr_get_scan_rssi(int index) { return 0; }
void wifi_mgr_connect(const char* ssid, const char* password) {}
wifi_status_t wifi_mgr_get_status() { return WIFI_STATUS_CONNECTED; }
const char* wifi_mgr_get_current_ssid() { return "PC Ethernet/WiFi"; }
const char* wifi_mgr_get_saved_ssid() { return ""; }

const char* wifi_mgr_get_ip() {
    static char ip_buf[64] = "127.0.0.1";
#ifndef _WIN32
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) continue;
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (strcmp(ifa->ifa_name, "lo") != 0 && strcmp(ifa->ifa_name, "lo0") != 0) {
                strncpy(ip_buf, addressBuffer, sizeof(ip_buf));
                break;
            }
        }
    }
    if (ifAddrStruct != NULL) freeifaddrs(ifAddrStruct);
#endif
    return ip_buf;
}

const char* wifi_mgr_get_time() {
    static char time_buf[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", t);
    return time_buf;
}

#endif // ARDUINO
