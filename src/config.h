#include <stdint.h>
//#include "config_manage.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

extern json vmac_config;

extern void config_init();
extern uint16_t vMacScreenHeight;
extern uint16_t vMacScreenWidth;
extern uint16_t vMacScreenDepth;
extern uint32_t vMacScreenNumPixels;
extern uint32_t vMacScreenNumBits;
extern uint32_t vMacScreenNumBytes;
extern uint32_t vMacScreenBitWidth;
extern uint32_t vMacScreenByteWidth;
extern uint32_t vMacScreenMonoNumBytes;
extern uint32_t vMacScreenMonoByteWidth;

//extern bool UseLargeScreenHack;