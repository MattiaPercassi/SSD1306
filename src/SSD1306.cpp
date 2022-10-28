#include "SSD1306.h"
#include <stdio.h>
#include <fstream>
#include <vector>
#include <bitset>
#include <iostream>

SSD1306::SSD1306(int h, int w) : addr{0x3c}, height{h}, width{w}
{
    fd = i2cOpen(1, addr, 0);
    std::ifstream ifile{"/home/mattia/Documents/i2ctest/resources/rie.bmp", std::ios::binary};
    if (ifile)
    {
        ifile.ignore(62); // header length in 1bpp bitmap
        char dt;
        std::array<char, 4 * 128> tempStor{};
        for (auto &byte : tempStor)
        {
            ifile.read(&dt, 1);
            byte = dt;
        }
        char temp{};
        for (int i{}; i < 64; ++i) // 64 blocks of 8 bytes
        {
            for (int j{}; j < 8; ++j) // go through each byte of the new image
            {
                for (int z{}; z < 8; ++z) // go through each byte of the original bitmap
                {
                    temp = 1 & (tempStor[128 * (4 - i / 16) - 16 + i % 16 - z * 16] >> (7 - j));
                    startupImage[i * 8 + j] += (temp << z);
                }
            }
        }
        ifile.close();
    }
};

void SSD1306::build2bMes(char co1, char d1)
{
    Message2b[0] = co1;
    Message2b[1] = d1;
};

int SSD1306::closeConnection()
{
    i2cClose(fd);
    return 0;
};

void SSD1306::build4bMes(char co1, char d1, char co2, char d2)
{
    Message4b[0] = co1;
    Message4b[1] = d1;
    Message4b[2] = co2;
    Message4b[3] = d2;
};

int SSD1306::init()
{
    if (fd < 0)
    {
        perror("i2cOpen");
        return -1;
    };
    build4bMes(cmdCo_continue, setMUX, cmdCo_single, 63);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("set mux");
        return -1;
    }
    build4bMes(cmdCo_continue, setOffset, cmdCo_single, 0);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("set offset");
        return -1;
    }
    build2bMes(cmdCo_single, setStar);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("set start");
        return -1;
    }
    build2bMes(cmdCo_single, setRemap | 1); // remap com0 to com 127
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("set remap");
        return -1;
    }
    build2bMes(cmdCo_single, setCOMdir);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("set COMdir");
        return -1;
    }
    build4bMes(cmdCo_continue, setCOMpins, cmdCo_single, 0x02);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("set COM pins");
        return -1;
    }
    build4bMes(cmdCo_continue, setContrast, cmdCo_single, 0x8f);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("Set contrast");
        return -1;
    }
    build2bMes(cmdCo_single, displayRAM);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("disable display");
        return -1;
    }
    build2bMes(cmdCo_single, setNormDis);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("set normal display");
        return -1;
    }
    build4bMes(cmdCo_continue, setOsc, cmdCo_single, 0x80);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("set oscillator");
        return -1;
    }
    build4bMes(cmdCo_continue, chargePump, cmdCo_single, 0x14);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("set chargepump");
        return -1;
    }
    // set precharge
    build4bMes(cmdCo_continue, setPrecharge, cmdCo_single, 0xf1);
    i2cWriteDevice(fd, Message4b.data(), 4);
    // set vcomdetect
    build4bMes(cmdCo_continue, setVCOMdetect, cmdCo_single, 0x20);
    i2cWriteDevice(fd, Message4b.data(), 4);
    std::vector<char> mess;
    // command for set address page
    mess.push_back(cmdCo_continue);
    mess.push_back(0x22);
    mess.push_back(cmdCo_continue);
    mess.push_back(4);
    mess.push_back(cmdCo_single);
    mess.push_back(7);
    i2cWriteDevice(fd, mess.data(), 6);
    // command for set column address
    mess.push_back(cmdCo_continue);
    mess.push_back(0x21);
    mess.push_back(cmdCo_continue);
    mess.push_back(0);
    mess.push_back(cmdCo_single);
    mess.push_back(127);
    i2cWriteDevice(fd, mess.data(), 6);
    // set horizontal mode
    build4bMes(cmdCo_continue, setMode, cmdCo_single, 0x00);
    i2cWriteDevice(fd, Message4b.data(), 4);
    //  set no scroll
    build2bMes(cmdCo_single, deactivateScroll);
    i2cWriteDevice(fd, Message2b.data(), 2);
    build2bMes(cmdCo_single, displayON);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("display on");
        return -1;
    }
    // Here below any code for display specific image on boot
    emptyRAM();
    return 0;
};

int SSD1306::litAll()
{
    build2bMes(cmdCo_single, allON);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("light all");
        return -1;
    }
    return 0;
};

int SSD1306::showRAM()
{
    build2bMes(cmdCo_single, displayRAM);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("display ram");
        return -1;
    }
    return 0;
};

int SSD1306::turnOFF()
{
    build2bMes(cmdCo_single, displayOFF);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("turn off");
        return -1;
    }
    return 0;
};

int SSD1306::invertDisplay()
{
    build2bMes(cmdCo_single, setInvertDis);
    i2cWriteDevice(fd, Message2b.data(), 2);
    return 0;
};

int SSD1306::resetCursor()
{
    build2bMes(cmdCo_single, setStar);
    i2cWriteDevice(fd, Message2b.data(), 2);
    return 0;
}

int SSD1306::writeImage()
{
    std::vector<char> mess;
    // // command for set column address
    // mess.push_back(cmdCo_continue);
    // mess.push_back(0x21);
    // mess.push_back(cmdCo_continue);
    // mess.push_back(0);
    // mess.push_back(cmdCo_single);
    // mess.push_back(127);
    // i2cWriteDevice(fd, mess.data(), 6);
    // // command for set address page
    // mess.push_back(cmdCo_continue);
    // mess.push_back(0x22);
    // mess.push_back(cmdCo_continue);
    // mess.push_back(0);
    // mess.push_back(cmdCo_single);
    // mess.push_back(3);
    // i2cWriteDevice(fd, mess.data(), 6);
    // build2bMes(cmdCo_single, setStar);
    // i2cWriteDevice(fd, Message2b.data(), 2);
    // std::array<char, 4 * 128> image{0};
    int i{};
    while (i < 4 * 128)
    {
        mess.clear();
        mess.push_back(dataCo);
        for (int j{1}; j < 31; ++j)
        {
            mess.push_back(startupImage[i]);
            i++;
            if (i >= 4 * 128)
                break;
        }
        i2cWriteDevice(fd, mess.data(), mess.size());
        // send additional byte to display the data
        // build2bMes(cmdCo_single, 0x00);
        // i2cWriteDevice(fd, Message2b.data(), 2);
    }
    build2bMes(cmdCo_single, noOP);
    i2cWriteDevice(fd, Message2b.data(), 2); // display data?
    return 0;
};

int SSD1306::fillwithbyte(char bt)
{
    std::vector<char> mess;
    int i{};
    while (i < 4 * 128)
    {
        mess.clear();
        mess.push_back(dataCo);
        for (int j{0}; j < 31; ++j)
        {
            mess.push_back(bt);
            i++;
            if (i >= 4 * 128)
                break;
        }
        i2cWriteDevice(fd, mess.data(), mess.size());
        // send additional byte to display the data
        //     build2bMes(cmdCo_single, 0x00);
        //     i2cWriteDevice(fd, Message2b.data(), 2);
    }
    build2bMes(cmdCo_single, noOP);
    i2cWriteDevice(fd, Message2b.data(), 2); // display data?
    return 0;
}

int SSD1306::emptyRAM()
{
    std::vector<char> mess;
    // command for set column address
    // mess.push_back(cmdCo_continue);
    // mess.push_back(0x21);
    // mess.push_back(cmdCo_continue);
    // mess.push_back(0);
    // mess.push_back(cmdCo_single);
    // mess.push_back(127);
    // i2cWriteDevice(fd, mess.data(), 6);
    // // command for set address page
    // mess.push_back(cmdCo_continue);
    // mess.push_back(0x22);
    // mess.push_back(cmdCo_continue);
    // mess.push_back(0);
    // mess.push_back(cmdCo_single);
    // mess.push_back(3);
    // i2cWriteDevice(fd, mess.data(), 6);
    // build2bMes(cmdCo_single, setStar);
    // i2cWriteDevice(fd, Message2b.data(), 2);
    int i{};
    while (i < 4 * 128)
    {
        mess.clear();
        mess.push_back(dataCo);
        for (int j{0}; j < 31; ++j)
        {
            mess.push_back(0);
            i++;
            if (i >= 4 * 128)
                break;
        }
        i2cWriteDevice(fd, mess.data(), mess.size());
        // send additional byte to display the data
        //     build2bMes(cmdCo_single, 0x00);
        //     i2cWriteDevice(fd, Message2b.data(), 2);
    }
    build2bMes(cmdCo_single, noOP);
    i2cWriteDevice(fd, Message2b.data(), 2); // display data?
    return 0;
};