#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// WiFi 상태 정보
typedef enum {
    WIFI_STATUS_DISCONNECTED,
    WIFI_STATUS_SCANNING,
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_CONNECT_FAILED
} wifi_status_t;

// 초기화: 저장된 정보를 불러와서 자동 연결 시도
void wifi_mgr_init();

// 주기적 처리: 연결 상태 확인 등
void wifi_mgr_loop();

// WiFi 스캔 시작
void wifi_mgr_scan_start();

// 스캔 중인지 확인
bool wifi_mgr_is_scanning();

// 스캔 결과 수 가져오기
int wifi_mgr_get_scan_count();

// n번째 스캔된 SSID 가져오기
const char* wifi_mgr_get_scan_ssid(int index);

// n번째 스캔된 신호 강도 가져오기
int wifi_mgr_get_scan_rssi(int index);

// 연결 시도
void wifi_mgr_connect(const char* ssid, const char* password);

// 현재 연결 상태 가져오기
wifi_status_t wifi_mgr_get_status();

// 현재 접속된 SSID 가져오기
const char* wifi_mgr_get_current_ssid();

// 저장된 SSID 가져오기 (최근 성공한 것)
const char* wifi_mgr_get_saved_ssid();

// 현재 IP 주소 가져오기
const char* wifi_mgr_get_ip();

// 현재 시간 가져오기 (문자열 형식)
const char* wifi_mgr_get_time();

#ifdef __cplusplus
}
#endif

#endif // WIFI_MANAGER_H
