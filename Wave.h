//
// Created by Edward Gallyot on 03/01/2022.
//

#ifndef WAVE_IO_WAVE_H
#define WAVE_IO_WAVE_H

#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>

#define BIT16_RES 32767.0

struct Wave_Header
{
    /* RIFF Chunk Descriptor */
    uint8_t RIFF[4]; // RIFF Header Magic Header
    uint32_t ChunkSize; // RIFF Chunk Size
    uint8_t WAVE[4]; // WAVE Header

    /* "fmt" sub-chunk */
    uint8_t fmt[4]; // FMT header
    uint32_t Subchunk1Size; // Size of the fmt chunk
    uint16_t AudioFormat; // Audio Format 1=PCM, 6=mulaw, 7=a-law
    uint16_t NumOfChan; // Number of channels
    uint32_t SamplesPerSec; // Sample Rate
    uint32_t bytesPerSec; // bytes per second
    uint16_t blockAlign; // 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample; // Number of bits per sample

    /* byteData sub-chunk */
    uint8_t Subchunk2ID[4]; // "byteData" string
    uint32_t Subchunk2Size; // Sampled byteData length
};


class Wave
{
public:
    Wave() = default;

    void getHeader();

    void getData();

    void setFileName(const std::string &fileName);

private:
    std::string fileName;
    Wave_Header header;
    std::fstream fileStream;
    int8_t *byteData;
    float *floatData;
};


#endif //WAVE_IO_WAVE_H
