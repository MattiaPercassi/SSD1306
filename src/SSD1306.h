#ifndef _SSD1306_H_
#define _SSD1306_H_
#include <pigpio.h>
#include <array>
#include <vector>

class SSD1306
{
protected:
    const int addr;
    int fd;
    const int height;
    const int width;

    // relevant 2 and 4 byte messages
    std::array<char, 4> Message4b; // 4 bytes message
    std::array<char, 2> Message2b; // 2bytes message

    struct
    {
        const char setMode{0x20};
        const char deactivateScroll{0x2e};
        const char setStar{0x40};
        const char setContrast{0x81};
        const char chargePump{0x8d};
        const char setRemap{0xa0};
        const char setRemapInv{0xa1};
        const char displayRAM{0xa4};
        const char allON{0xa5};
        const char setNormDis{0xa6};
        const char setInvertDis{0xa7};
        const char setMUX{0xa8};
        const char displayOFF{0xae};
        const char displayON{0xaf};
        const char setCOMdir{0xc8};
        const char setOffset{0xd3};
        const char setOsc{0xd5};
        const char setPrecharge{0xd9};
        const char setCOMpins{0xda};
        const char setVCOMdetect{0xdb};
        const char noOP{0xe3};
    } cmd;

    // command bytes
    struct
    {
        const char dataCo{0b01000000};
        const char cmdCo_single{0b00000000};
        const char cmdCo_continue{0b10000000}; // in case there is additional bytes after the control bit
    } CObyte;

    void build4bMes(char co1, char d1, char co2, char d2);
    void build2bMes(char co1, char d1);

    // resources
    std::vector<std::array<char, 4 * 128>> Images;

public:
    SSD1306(int h, int w);
    int init();
    int closeConnection();
    int litAll();
    int showRAM();
    int turnOFF();
    int writeImage(int);
    int emptyRAM();
    int resetCursor();
    int fillwithbyte(char);
    int invertDisplay();
    int loadCustomImage(std::string);
};

#endif