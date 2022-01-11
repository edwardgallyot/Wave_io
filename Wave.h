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

class Wave_Header {
public:
    Wave_Header() = default;

    // Copy Constructor to move wav header to new file
    Wave_Header(const Wave_Header &other) {
        for (auto i = 0; i < 4; i++)
            RIFF[i] = other.RIFF[i];
        ChunkSize = other.ChunkSize;
        for (auto i = 0; i < 4; i++)
            WAVE[i] = other.WAVE[i];
        for (auto i = 0; i < 4; i++)
            fmt[i] = other.fmt[i];
        Subchunk1Size = other.Subchunk1Size;
        AudioFormat = other.AudioFormat;
        NumOfChan = other.NumOfChan;
        SamplesPerSec = other.SamplesPerSec;
        bytesPerSec = other.bytesPerSec;
        blockAlign = other.blockAlign;
        bitsPerSample = other.bitsPerSample;
        for (auto i = 0; i < 4; i++)
            Subchunk2ID[i] = other.Subchunk2ID[i];
        Subchunk2Size = other.Subchunk2Size;
    };

    void getHeader(std::fstream &fstream) {
        fstream.read(reinterpret_cast<char *>(RIFF), 4);
        fstream.read(reinterpret_cast<char *>(&ChunkSize), 4);
        fstream.read(reinterpret_cast<char *>(WAVE), 4);
        fstream.read(reinterpret_cast<char *>(fmt), 4);
        fstream.read(reinterpret_cast<char *>(&Subchunk1Size), 4);
        fstream.read(reinterpret_cast<char *>(&AudioFormat), 2);
        fstream.read(reinterpret_cast<char *>(&NumOfChan), 2);
        fstream.read(reinterpret_cast<char *>(&SamplesPerSec), 4);
        fstream.read(reinterpret_cast<char *>(&bytesPerSec), 4);
        fstream.read(reinterpret_cast<char *>(&blockAlign), 2);
        fstream.read(reinterpret_cast<char *>(&bitsPerSample), 2);
        fstream.read(reinterpret_cast<char *>(Subchunk2ID), 4);
        fstream.read(reinterpret_cast<char *>(&Subchunk2Size), 4);
    };

    void setSamplesPerSec(uint32_t samplesPerSec) {
        SamplesPerSec = samplesPerSec;
    }

    void setBytesPerSec(uint32_t bytesPerSec) {
        Wave_Header::bytesPerSec = bytesPerSec;
    }

    uint32_t getSubchunk2Size() const {
        return Subchunk2Size;
    }

private:
    /* RIFF Chunk Descriptor */
    uint8_t RIFF[4]{}; // RIFF Header Magic Header
    uint32_t ChunkSize{}; // RIFF Chunk Size
    uint8_t WAVE[4]{}; // WAVE Header

    /* "fmt" sub-chunk */
    uint8_t fmt[4]{}; // FMT m_headerIn
    uint32_t Subchunk1Size{}; // Size of the fmt chunk
    uint16_t AudioFormat{}; // Audio Format 1=PCM, 6=mulaw, 7=a-law
    uint16_t NumOfChan{}; // Number of channels
    uint32_t SamplesPerSec{}; // Sample Rate
    uint32_t bytesPerSec{}; // bytes per second
    uint16_t blockAlign{};// 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample{}; // Number of bits per sample

    /* m_byteData sub-chunk */
    uint8_t Subchunk2ID[4]{}; // "m_byteData" string
    uint32_t Subchunk2Size{}; // Sampled data length
};


template<typename T>
class Wave {
public:
    Wave() : m_Data(nullptr), m_byteData(nullptr), m_headerIn(nullptr), m_headerOut(nullptr) {};

    ~Wave() {
        delete m_fileNameIn;
        delete m_fileNameOut;
        delete m_headerIn;
        delete m_headerOut;
        delete[] m_Data;
        delete[] m_byteData;
    };

    void getHeader() {
        m_fileStream.open(*m_fileNameIn, std::ios::in);
        if (!m_fileStream.is_open()) {
            std::cout << "No File Found..." << std::endl;
            return;
        }
        m_headerIn = new Wave_Header;
        m_headerIn->getHeader(m_fileStream);
        m_fileStream.close();
        m_byteLength = m_headerIn->getSubchunk2Size();
        m_dataLength = m_byteLength / sizeof(u_int16_t);
        copyHeader();
    };

    void copyHeader() {
        m_headerOut = new Wave_Header;
        *m_headerOut = *m_headerIn;
    };

    void setFileNameIn(const std::string &fileNameIn) {
        this->m_fileNameIn = new std::string{fileNameIn};
    }

    void setFileNameOut(const std::string &fileNameOut) {
        this->m_fileNameOut = new std::string{fileNameOut};
    }

    void getData() {
        if (m_headerIn == nullptr) {
            std::cout << "No name set" << std::endl;
            return;
        }
        m_byteData = new int8_t[m_byteLength];

        m_fileStream.open(*m_fileNameIn, std::ios::in);
        m_fileStream.seekg(sizeof(Wave_Header), std::ios::beg);
        m_fileStream.read(reinterpret_cast<char *>(m_byteData), m_byteLength);
        m_fileStream.close();

        m_Data = new T[m_dataLength];
        for (auto i = 0; i < m_byteLength; i = i + 2) {
            // Bitwise or is the same as adding two byte values together
            // In a 16 bit integer if we shift the second byte to be first we have a little endian byte form.
            // If we were on a big endian machine we sould swap the two terms either side of bitwise or around
            int16_t sample_i = ((m_byteData[i] & 0xff) | (m_byteData[i + 1] << 8));
            T sample_f;
            sample_f = sample_i / BIT16_RES;
            m_Data[i / 2] = sample_f;
        }
    };

    void writeData() {
        if (m_fileNameOut == nullptr || m_headerOut == nullptr) {
            std::cout << "Cannot write out... Is File Name Set? Does object have header?" << std::endl;
            return;
        }
        m_fileStream.open(*m_fileNameOut, std::ios::out);
        // Write the m_headerIn of the file
        m_fileStream.write(reinterpret_cast<char *>(m_headerOut), sizeof(Wave_Header));

        for (int i = 0; i < m_dataLength; i++) {
            // Get the value from out data array
            u_int16_t littleEndianValue = BIT16_RES * m_Data[i];
            // Create a byte Array to write to the file
            uint8_t byteArray[sizeof(u_int16_t)];
            byteArray[0] = (uint8_t) (littleEndianValue >> 0); // Most significant byte
            byteArray[1] = (uint8_t) (littleEndianValue >> 8); // Least significant byte
            m_fileStream.write(reinterpret_cast<char *>(byteArray), 2); // Write to File
        }
        m_fileStream.close(); // Close the file stream
    };

    void changeSampleRate(uint32_t rate) {
        if (m_headerOut != nullptr) {
            m_headerOut->setSamplesPerSec(rate);
            m_headerOut->setBytesPerSec(2 * rate);
        }
    }

    void writeDataWithMute() {
        if (m_fileNameOut == nullptr || m_headerOut == nullptr) {
            std::cout << "Cannot write out... Is File Name Set? Does object have header?" << std::endl;
            return;
        }
        // Write the m_headerIn of the file
        m_fileStream.open(*m_fileNameOut, std::ios::out);
        m_fileStream.write(reinterpret_cast<char *>(m_headerOut), sizeof(Wave_Header));

        for (int i = 0; i < m_dataLength; i++) {
            u_int16_t littleEndianValue;
            // Get the value from out data array
            // If inside d_T
            if (i > ((m_dataLength / 2) + (m_dataLength / 100)) && i < ((m_dataLength / 2) + (m_dataLength / 30)))
                littleEndianValue = 0;
            else
                littleEndianValue = BIT16_RES * m_Data[i];
            // Create a byte Array to write to the file
            uint8_t byteArray[sizeof(u_int16_t)];
            byteArray[0] = (uint8_t) (littleEndianValue >> 0); // Most significant byte
            byteArray[1] = (uint8_t) (littleEndianValue >> 8); // Least significant byte
            m_fileStream.write(reinterpret_cast<char *>(byteArray), 2);
        }
        m_fileStream.close();
    };

private:
    std::string *m_fileNameIn{nullptr};
    std::string *m_fileNameOut{nullptr};
    Wave_Header *m_headerIn;
    Wave_Header *m_headerOut;
    std::fstream m_fileStream;
    int8_t *m_byteData;
    size_t m_byteLength{0};
    T *m_Data;
    size_t m_dataLength{0};
};


#endif //WAVE_IO_WAVE_H
