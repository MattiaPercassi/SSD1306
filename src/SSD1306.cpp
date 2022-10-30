#include "SSD1306.h"
#include <stdio.h>
#include <fstream>
#include <vector>
#include <bitset>
#include <iostream>
#include <string>

SSD1306::SSD1306(int h, int w) : addr{0x3c}, height{h}, width{w}
{
    fd = i2cOpen(1, addr, 0);
    loadCustomImage("/home/mattia/Documents/i2ctest/resources/rie.bmp");
};

int SSD1306::loadCustomImage(std::string filepath)
{
    std::ifstream ifile{filepath, std::ios::binary};
    if (!ifile)
    {
        std::cerr << "Error opening custom image";
        return -1;
    }
    // we need to ensure it is a 1bpp bitmap
    char dt;
    std::array<char, 3> validationArr;
    ifile.read(validationArr, 2);
    ifile.ignore(26);
    ifile.read(validationArr + 2, 1);
    if (validationArr.at(0) != 0x42 || validationArr.at(1) != 0x4d || validationArr.at(2) != 0x01)
    {
        std::cerr << "It seems that the image is not a 1bpp bitmap";
        return -1;
    }
    ifile.clear();
    ifile.seekg(0);
    ifile.ignore(62); // header length in 1bpp bitmap
    std::array<char, 4 * 128> tempStor{};
    std::array<char, 4 * 128> finalImage{};
    for (auto &byte : tempStor)
    {
        ifile.read(&dt, 1);
        byte = dt;
    }
    for (int i{}; i < 64; ++i) // 64 blocks of 8 bytes
    {
        for (int j{}; j < 8; ++j) // go through each byte of the new image
        {
            for (int z{}; z < 8; ++z) // go through each byte of the original bitmap
            {
                dt = 1 & (tempStor[128 * (4 - i / 16) - 16 + i % 16 - z * 16] >> (7 - j));
                finalImage[i * 8 + j] += (dt << z);
            }
        }
    }
    ifile.close();
    Images.push_back(finalImage);
    return 0;
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
    build4bMes(CObyte.cmdCo_continue, cmd.setMUX, CObyte.cmdCo_single, 63);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("set mux");
        return -1;
    }
    build4bMes(CObyte.cmdCo_continue, cmd.setOffset, CObyte.cmdCo_single, 0);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("set offset");
        return -1;
    }
    build2bMes(CObyte.cmdCo_single, cmd.setStar);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("set start");
        return -1;
    }
    build2bMes(CObyte.cmdCo_single, cmd.setRemap | 1); // remap com0 to com 127
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("set remap");
        return -1;
    }
    build2bMes(CObyte.cmdCo_single, cmd.setCOMdir);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("set COMdir");
        return -1;
    }
    build4bMes(CObyte.cmdCo_continue, cmd.setCOMpins, CObyte.cmdCo_single, 0x02);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("set COM pins");
        return -1;
    }
    build4bMes(CObyte.cmdCo_continue, cmd.setContrast, CObyte.cmdCo_single, 0x8f);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("Set contrast");
        return -1;
    }
    build2bMes(CObyte.cmdCo_single, cmd.displayRAM);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("disable display");
        return -1;
    }
    build2bMes(CObyte.cmdCo_single, cmd.setNormDis);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("set normal display");
        return -1;
    }
    build4bMes(CObyte.cmdCo_continue, cmd.setOsc, CObyte.cmdCo_single, 0x80);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("set oscillator");
        return -1;
    }
    build4bMes(CObyte.cmdCo_continue, cmd.chargePump, CObyte.cmdCo_single, 0x14);
    if (i2cWriteDevice(fd, Message4b.data(), 4) < 0)
    {
        perror("set chargepump");
        return -1;
    }
    // set precharge
    build4bMes(CObyte.cmdCo_continue, cmd.setPrecharge, CObyte.cmdCo_single, 0xf1);
    i2cWriteDevice(fd, Message4b.data(), 4);
    // set vcomdetect
    build4bMes(CObyte.cmdCo_continue, cmd.setVCOMdetect, CObyte.cmdCo_single, 0x20);
    i2cWriteDevice(fd, Message4b.data(), 4);
    std::vector<char> mess;
    // command for set address page
    mess.push_back(CObyte.cmdCo_continue);
    mess.push_back(0x22);
    mess.push_back(CObyte.cmdCo_continue);
    mess.push_back(4);
    mess.push_back(CObyte.cmdCo_single);
    mess.push_back(7);
    i2cWriteDevice(fd, mess.data(), 6);
    // command for set column address
    mess.push_back(CObyte.cmdCo_continue);
    mess.push_back(0x21);
    mess.push_back(CObyte.cmdCo_continue);
    mess.push_back(0);
    mess.push_back(CObyte.cmdCo_single);
    mess.push_back(127);
    i2cWriteDevice(fd, mess.data(), 6);
    // set horizontal mode
    build4bMes(CObyte.cmdCo_continue, cmd.setMode, CObyte.cmdCo_single, 0x00);
    i2cWriteDevice(fd, Message4b.data(), 4);
    //  set no scroll
    build2bMes(CObyte.cmdCo_single, cmd.deactivateScroll);
    i2cWriteDevice(fd, Message2b.data(), 2);
    build2bMes(CObyte.cmdCo_single, cmd.displayON);
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
    build2bMes(CObyte.cmdCo_single, cmd.allON);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("light all");
        return -1;
    }
    return 0;
};

int SSD1306::showRAM()
{
    build2bMes(CObyte.cmdCo_single, cmd.displayRAM);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("display ram");
        return -1;
    }
    return 0;
};

int SSD1306::turnOFF()
{
    build2bMes(CObyte.cmdCo_single, cmd.displayOFF);
    if (i2cWriteDevice(fd, Message2b.data(), 2) < 0)
    {
        perror("turn off");
        return -1;
    }
    return 0;
};

int SSD1306::invertDisplay()
{
    build2bMes(CObyte.cmdCo_single, cmd.setInvertDis);
    i2cWriteDevice(fd, Message2b.data(), 2);
    return 0;
};

int SSD1306::resetCursor()
{
    build2bMes(CObyte.cmdCo_single, cmd.setStar);
    i2cWriteDevice(fd, Message2b.data(), 2);
    return 0;
};

int SSD1306::writeImage(int index)
{
    if (index > Images.size() - 1 || index < 0)
    {
        std::cerr << "Index not acceptable";
        return -1;
    }
    std::vector<char> mess;
    int i{};
    while (i < 4 * 128)
    {
        mess.clear();
        mess.push_back(CObyte.dataCo);
        for (int j{1}; j < 31; ++j)
        {
            mess.push_back(Images.at(index).at(i));
            i++;
            if (i >= 4 * 128)
                break;
        }
        i2cWriteDevice(fd, mess.data(), mess.size());
    }
    return 0;
};

int SSD1306::fillwithbyte(char bt)
{
    std::vector<char> mess;
    int i{};
    while (i < 4 * 128)
    {
        mess.clear();
        mess.push_back(CObyte.dataCo);
        for (int j{0}; j < 31; ++j)
        {
            mess.push_back(bt);
            i++;
            if (i >= 4 * 128)
                break;
        }
        i2cWriteDevice(fd, mess.data(), mess.size());
        // send additional byte to display the data
        //     build2bMes(CObyte.cmdCo_single, 0x00);
        //     i2cWriteDevice(fd, Message2b.data(), 2);
    }
    build2bMes(CObyte.cmdCo_single, cmd.noOP);
    i2cWriteDevice(fd, Message2b.data(), 2); // display data?
    return 0;
};

int SSD1306::emptyRAM()
{
    std::vector<char> mess;
    int i{};
    while (i < 4 * 128)
    {
        mess.clear();
        mess.push_back(CObyte.dataCo);
        for (int j{0}; j < 31; ++j)
        {
            mess.push_back(0);
            i++;
            if (i >= 4 * 128)
                break;
        }
        i2cWriteDevice(fd, mess.data(), mess.size());
        // send additional byte to display the data
        //     build2bMes(CObyte.cmdCo_single, 0x00);
        //     i2cWriteDevice(fd, Message2b.data(), 2);
    }
    build2bMes(CObyte.cmdCo_single, cmd.noOP);
    i2cWriteDevice(fd, Message2b.data(), 2); // display data?
    return 0;
};