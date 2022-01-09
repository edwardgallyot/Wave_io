//
// Created by Edward Gallyot on 03/01/2022.
//

#include "Wave.h"

void Wave::getHeader()
{
    int headerSize = sizeof(Wave_Header);
    fileStream.open(fileName, std::ios::binary);
    fileStream.read(reinterpret_cast<char *>(&header), headerSize);
    fileStream.close();
}

void Wave::setFileName(const std::string &fileName)
{
    Wave::fileName = fileName;
}

void Wave::getData()
{
    int headerSize = sizeof(Wave_Header);
    auto length = header.Subchunk2Size;
    byteData = new int8_t[length];
    std::cout << length << std::endl;
    fileStream.open(fileName, std::ios::in);
    if (fileStream.is_open())
    {
        std::cout << "Open!" << std::endl;
    }
    fileStream.seekg(headerSize, std::ios::beg);
    fileStream.read(reinterpret_cast<char *>(byteData), length);
    fileStream.close();

    floatData = new float[length];

    for (auto i = 0; i < length; i = i + 2)
    {
        int16_t sample_i = ((byteData[i] & 0xff) | (byteData[i + 1] << 8));
        //std::cout << sample_i << std::endl;
        float sample_f;

        sample_f = sample_i / BIT16_RES;


        floatData[i / 2] = sample_f;
        std::cout << sample_f << std::endl;
    }
}