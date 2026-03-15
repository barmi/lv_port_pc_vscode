#ifdef ARDUINO
#include <Arduino.h>
#include <SD_MMC.h>
#include <FS.h>
#include "sd_manager.h"

static String sd_status_msg = "SD Not Initialized";
static String sd_file_list = "";
static bool sd_mounted = false;

// 재귀적으로 디렉토리 탐색 (ls -laR 스타일)
void listDir(fs::FS &fs, const char * dirname, uint8_t levels, String indent) {
    File root = fs.open(dirname);
    if(!root){
        return;
    }
    if(!root.isDirectory()){
        return;
    }

    // 현재 디렉토리 제목 표시
    sd_file_list += indent + "[" + String(dirname) + "]\n";

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            if(levels > 0){
                listDir(fs, file.path(), levels - 1, indent + "  ");
            } else {
                sd_file_list += indent + "  [DIR] " + String(file.name()) + "\n";
            }
        } else {
            sd_file_list += indent + "  - " + String(file.name());
            sd_file_list += " (" + String(file.size()) + " bytes)\n";
        }
        file = root.openNextFile();
    }
}

void sd_mgr_init() {
    sd_file_list = ""; // 초기화 시 파일 목록도 초기화
    // JC3248W535 보드는 보통 SD_MMC (1-bit mode)를 사용함
    // 핀 설정: CLK=14, CMD=15, D0=2
    SD_MMC.setPins(14, 15, 2);
    
    // begin() 호출 전에는 cardType()이 정확하지 않을 수 있으나, 
    // 실패 시의 상태를 알기 위해 먼저 begin()을 시도합니다.
    if (!SD_MMC.begin("/sdcard", true)) { // true = 1-bit mode
        sd_mounted = false;
        
        uint8_t cardType = SD_MMC.cardType();
        if (cardType == CARD_NONE) {
            sd_status_msg = "No SD Card Inserted";
        } else {
            // 카드는 있는데 마운트 실패 -> 포맷 문제일 가능성 높음
            String typeStr = "Unknown";
            if(cardType == CARD_MMC) typeStr = "MMC";
            else if(cardType == CARD_SD) typeStr = "SDSC";
            else if(cardType == CARD_SDHC) typeStr = "SDHC";
            sd_status_msg = "Mount Failed (Type: " + typeStr + "). Check Format (FAT32/exFAT)";
        }
        Serial.println("SD Card Mount Failed: " + sd_status_msg);
    } else {
        sd_mounted = true;
        uint8_t cardType = SD_MMC.cardType();
        String typeStr = "Unknown";
        if(cardType == CARD_MMC) typeStr = "MMC";
        else if(cardType == CARD_SD) typeStr = "SDSC";
        else if(cardType == CARD_SDHC) typeStr = "SDHC";
        
        uint64_t totalBytes = SD_MMC.totalBytes();
        uint64_t usedBytes = SD_MMC.usedBytes();
        
        uint64_t cardSize = totalBytes / (1024 * 1024);
        uint64_t usedSize = usedBytes / (1024 * 1024);
        
        sd_status_msg = "Type: " + typeStr + ", Size: " + String((unsigned long)cardSize) + "MB (Used: " + String((unsigned long)usedSize) + "MB)";
        Serial.println("SD Card Mounted: " + sd_status_msg);
    }
}

bool sd_mgr_is_mounted() {
    return sd_mounted;
}

const char* sd_mgr_get_status() {
    return sd_status_msg.c_str();
}

const char* sd_mgr_get_file_list() {
    if (!sd_mounted) {
        // 백그라운드에서 다시 시도하지 않고 상태만 반환 (UI 프리징 방지)
        return "SD Card not mounted.";
    }
    
    if (sd_file_list.length() > 0) {
        return sd_file_list.c_str();
    }
    
    sd_file_list = ""; 
    listDir(SD_MMC, "/", 3, ""); // 깊이를 3으로 제한하여 메모리 보호
    
    if (sd_file_list.length() == 0) {
        return "No files found on SD Card.";
    }
    
    return sd_file_list.c_str();
}

#else // !ARDUINO (PC Simulator)

#include "sd_manager.h"
#include <stdio.h>
#include <string.h>

void sd_mgr_init() {}
bool sd_mgr_is_mounted() { return true; }
const char* sd_mgr_get_status() { return "PC Simulator (Internal Disk)"; }
const char* sd_mgr_get_file_list() {
    return "[/]\n"
           "  - system.log (1024 bytes)\n"
           "  - config.json (256 bytes)\n"
           "  [/images]\n"
           "    - wallpaper.jpg (512000 bytes)\n"
           "    - icon.png (12345 bytes)\n"
           "  [/data]\n"
           "    - user_db.sql (2048 bytes)";
}

#endif
