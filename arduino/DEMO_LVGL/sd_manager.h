#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SD 카드 초기화 및 마운트 시도
 */
void sd_mgr_init();

/**
 * @brief SD 카드 마운트 여부 확인
 * @return true 마운트됨, false 마운트 실패
 */
bool sd_mgr_is_mounted();

/**
 * @brief SD 카드 상태 문자열 반환 (용량, 타입 등)
 * @return 상태 정보를 담은 문자열 포인터
 */
const char* sd_mgr_get_status();

/**
 * @brief SD 카드 파일 목록 문자열 반환 (ls -laR 스타일)
 * @return 재귀적으로 탐색된 파일 목록 문자열 포인터
 */
const char* sd_mgr_get_file_list();

#ifdef __cplusplus
}
#endif

#endif // SD_MANAGER_H
