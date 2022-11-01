#ifndef _SSD1306_H_
#define _SSD1306_H_
#include <pigpio.h>
#include <array>
#include <vector>
#include <string>
#include <map>

class SSD1306
{
protected:
    const int addr;
    int fd;
    const int height;
    const int width;
    int cursor{};
    std::map<char, std::vector<char>> bitmapFont;

    // relevant 2 and 4 byte messages
    std::array<char, 4> Message4b; // 4 bytes message
    std::array<char, 2> Message2b; // 2bytes message
    struct
    {
        std::vector<char> letterA{0b10000011, 0b11101101, 0b11101101, 0b10000011};
        std::vector<char> letterB{0b10000001, 0b10110101, 0b10110101, 0b11001011};
        std::vector<char> letterC{0b11000011, 0b10111101, 0b10111101};
        std::vector<char> letterD{0b10000001, 0b10111101, 0b10111101, 0b11000011};
        std::vector<char> letterE{0b11000011, 0b10110101, 0b10110101};
        std::vector<char> letterF{0b10000001, 0b11110101, 0b11111101};
        std::vector<char> letterG{0b11000011, 0b10111101, 0b10101101, 0b10001101};
        std::vector<char> letterH{0b10000001, 0b11101111, 0b11101111, 0b10000001};
        std::vector<char> letterI{0b10000001};
        std::vector<char> letterJ{0b11001111, 0b10111111, 0b11000001};
        std::vector<char> letterK{0b10000001, 0b11110111, 0b11101011, 0b10011101};
        std::vector<char> letterL{0b10000001, 0b10111111, 0b10111111};
        std::vector<char> letterM{0b10000001, 0b11111011, 0b11110111, 0b11111011, 0b10000001};
        std::vector<char> letterN{0b10000001, 0b11111011, 0b11110111, 0b11101111, 0b10000001};
        std::vector<char> letterO{0b11000011, 0b10111101, 0b10111101, 0b11000011};
        std::vector<char> letterP{0b10000001, 0b11101101, 0b11110011};
        std::vector<char> letterQ{0b11000011, 0b10111101, 0b10011101, 0b10000011, 0b10111111};
        std::vector<char> letterR{0b10000001, 0b11110101, 0b10001011};
        std::vector<char> letterS{0b10111011, 0b10110101, 0b11001101};
        std::vector<char> letterT{0b11111101, 0b10000001, 0b11111101};
        std::vector<char> letterU{0b11000001, 0b10111111, 0b10111111, 0b11000001};
        std::vector<char> letterV{0b11110001, 0b11001111, 0b10111111, 0b11001111, 0b11110001};
        std::vector<char> letterW{0b11111001, 0b11000111, 0b10111111, 0b11001111, 0b10111111, 0b11000111, 0b11111001};
        std::vector<char> letterX{0b10011101, 0b11101011, 0b11110111, 0b11101011, 0b10011101};
        std::vector<char> letterY{0b11111101, 0b11111011, 0b10000111, 0b11111011, 0b11111101};
        std::vector<char> letterZ{0b10011101, 0b10101101, 0b10110101, 0b10111001};
        std::vector<char> letterSpace{0b11111111, 0b11111111};
        std::vector<char> letter0{0b11000011, 0b10111101, 0b11000011};
        std::vector<char> letter1{0b11111011, 0b10000001};
        std::vector<char> letter2{0b10011011, 0b10101101, 0b10110011};
        std::vector<char> letter3{0b10111101, 0b10110101, 0b11001011};
        std::vector<char> letter4{0b11100001, 0b11101111, 0b10000111};
        std::vector<char> letter5{0b11011001, 0b10110101, 0b11001101};
        std::vector<char> letter6{0b11000111, 0b10101011, 0b11001101};
        std::vector<char> letter7{0b11111101, 0b11111101, 0b10000001};
        std::vector<char> letter8{0b11001011, 0b10110101, 0b11001011};
        std::vector<char> letter9{0b11111011, 0b10110101, 0b11000011};
    } letters;

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
    SSD1306(int h, int w, std::string startupFilepath);
    // general functions
    int init();
    int closeConnection();
    // setup functions
    int litAll();
    int showRAM();
    int turnOFF();
    int resetStart();
    int invertDisplay();
    int loadCustomImage(std::string);
    // image writing functions
    int writeImage(int);
    int emptyRAM();
    int fillwithbyte(char);
    // string writing functions
    int writeStr(std::string);
    int resetCursor();
};

#endif