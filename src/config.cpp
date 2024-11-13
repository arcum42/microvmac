#include <fstream>
#include <iostream>

#include "config.h"
#include "spdlog/spdlog.h"
#include "HW/SCREEN/screen.h"

json vmac_config;

// Configuration variables
uint16_t vMacScreenHeight;
uint16_t vMacScreenWidth;
uint16_t vMacScreenDepth;

uint32_t vMacScreenNumPixels;
uint32_t vMacScreenNumBits;
uint32_t vMacScreenNumBytes;
uint32_t vMacScreenBitWidth;
uint32_t vMacScreenByteWidth;
uint32_t vMacScreenMonoNumBytes;
uint32_t vMacScreenMonoByteWidth;

void set_to_defaults()
{
    if (vmac_config["Video"]["Height"].is_null()) vmac_config["Video"]["Height"] = 342;
    if (vmac_config["Video"]["Width"].is_null()) vmac_config["Video"]["Width"] = 512;
    if (vmac_config["Video"]["Depth"].is_null()) vmac_config["Video"]["Depth"] = 0; // Crashes if not 0.

    if (vmac_config["Video"]["ColorBlack"].is_null()) vmac_config["Video"]["ColorBlack"] = "#000000";
    if (vmac_config["Video"]["ColorWhite"].is_null()) vmac_config["Video"]["ColorWhite"] = "#FFFFFF";
    
    if (vmac_config["Video"]["UseLargeScreenHack"].is_null()) vmac_config["Video"]["UseLargeScreenHack"] = false;

    if (vmac_config["Rom"]["Filename"].is_null()) vmac_config["Rom"]["Filename"] = "vMac.ROM";
}

void print_config()
{
    std::cout << "VideoHeight: " << vmac_config["Video"]["Height"] << "\n";
    std::cout << "VideoWidth: " << vmac_config["Video"]["Width"] << "\n";
    std::cout << "Video Depth: " << vmac_config["Video"]["Depth"] << "\n";
    std::cout << "VideoColorBlack: " << vmac_config["Video"]["ColorBlack"] << "\n";
    std::cout << "VideoColorWhite: " << vmac_config["Video"]["ColorWhite"] << "\n";
    std::cout << "UseLargeScreenHack: " << vmac_config["Video"]["UseLargeScreenHack"] << "\n";
    std::cout << "Rom Filename: " << vmac_config["Rom"]["Filename"] << "\n";
}

void config_init()
{
    std::ifstream f("vmac.json");
    if (f.good()) 
    {
        vmac_config = json::parse(f);
    }

    set_to_defaults();
    print_config();

    std::ofstream o("vmac.json");
    o << std::setw(4) << vmac_config << std::endl;

    Screen_LoadCfg();
}