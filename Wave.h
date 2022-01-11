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

class WaveHeader {
public:
    WaveHeader() = default;
    ~WaveHeader() = default;

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

    void writeHeader(std::fstream &fstream) {
        fstream.write(reinterpret_cast<char *>(RIFF), 4);
        fstream.write(reinterpret_cast<char *>(&ChunkSize), 4);
        fstream.write(reinterpret_cast<char *>(WAVE), 4);
        fstream.write(reinterpret_cast<char *>(fmt), 4);
        fstream.write(reinterpret_cast<char *>(&Subchunk1Size), 4);
        fstream.write(reinterpret_cast<char *>(&AudioFormat), 2);
        fstream.write(reinterpret_cast<char *>(&NumOfChan), 2);
        fstream.write(reinterpret_cast<char *>(&SamplesPerSec), 4);
        fstream.write(reinterpret_cast<char *>(&bytesPerSec), 4);
        fstream.write(reinterpret_cast<char *>(&blockAlign), 2);
        fstream.write(reinterpret_cast<char *>(&bitsPerSample), 2);
        fstream.write(reinterpret_cast<char *>(Subchunk2ID), 4);
        fstream.write(reinterpret_cast<char *>(&Subchunk2Size), 4);
    };


    void setSamplesPerSec(uint32_t samplesPerSec) {
        SamplesPerSec = samplesPerSec;
    }

    void setBytesPerSec(uint32_t bytesPerSec) {
        WaveHeader::bytesPerSec = bytesPerSec;
    }

    uint32_t getSubchunk2Size() const {
        return Subchunk2Size;
    }

    size_t getHeaderSize() const {
        return size;
    }

    void printInfo() {
        std::cout << "RIFF Head: ";
        printHead(RIFF);
        std::cout << "File Size: " << ChunkSize << std::endl;
        std::cout << "WAVE Head: ";
        printHead(WAVE);
        std::cout << "fmt Head: ";
        printHead(fmt);
        std::cout << "Chunk 1 Size: " << Subchunk1Size << std::endl;
        std::cout << "Channels: " << NumOfChan << std::endl;
        std::cout << "Sample Rate: " << SamplesPerSec << std::endl;
        std::cout << "Byte Rate: " << bytesPerSec << std::endl;
        std::cout << "Bits Per Sample: " << bitsPerSample << std::endl;
        printHead(Subchunk2ID);
        std::cout << "Size Of Data: " << Subchunk2Size << std::endl;
    };
private:
    static void printHead(char *head) {
        for (auto i = 0; i < 4; i++)
            std::cout << head[i];
        std::cout << std::endl;
    }

    /* RIFF Chunk Descriptor */
    char RIFF[4]{}; // RIFF Header Magic Header
    uint32_t ChunkSize{}; // RIFF Chunk Size
    char WAVE[4]{}; // WAVE Header

    /* "fmt" sub-chunk */
    char fmt[4]{}; // FMT m_headerIn
    uint32_t Subchunk1Size{}; // Size of the fmt chunk
    uint16_t AudioFormat{}; // Audio Format 1=PCM, 6=mulaw, 7=a-law
    uint16_t NumOfChan{}; // Number of channels
    uint32_t SamplesPerSec{}; // Sample Rate
    uint32_t bytesPerSec{}; // bytes per second
    uint16_t blockAlign{};// 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample{}; // Number of bits per sample

    /* m_byteData sub-chunk */
    char Subchunk2ID[4]{}; // "m_byteData" string
    uint32_t Subchunk2Size{}; // Sampled data length

    // WAV Header is always 44 bytes
    size_t size{44};
};


template<typename T>
class Wave : public WaveHeader {
public:
    Wave() = default;

    ~Wave() {
        delete m_fileName;
        delete[] m_Data;
        delete[] m_byteData;
    };

    void printFileName() {
        std::cout << *m_fileName << ": " << std::endl;
    }

    void readFile(std::string fileName) {
        setFileName(fileName);
        readData();
    };

    void writeFile(std::string fileName) {
        setFileName(fileName);
        writeData();
    };

    void writeFileWithMute(std::string fileName) {
        setFileName(fileName);
        writeDataWithMute();
    };

    void changeSampleRate(uint32_t rate) {
        setSamplesPerSec(rate);
        setBytesPerSec(2 * rate);
    }

    void clearFileName() { m_fileName = nullptr; };

private:
    void checkFileStream() {
        if (!m_fileStream.is_open()) {
            throw std::runtime_error("File Stream Failed to Open");
        }
    }

    void checkFileName() {
        if (m_fileName == nullptr) {
            throw std::runtime_error("File Out Name Is Not Set");
        }
    }


    void readHeader() {
        checkFileName();
        m_fileStream.open(*m_fileName, std::ios::in);
        checkFileStream();
        getHeader(m_fileStream);
        m_fileStream.close();
        m_byteLength = getSubchunk2Size();
        m_dataLength = m_byteLength / 2;
    };

    void setFileName(const std::string &fileName) {
        m_fileName = new std::string{fileName};
    }

    void readData() {
        readHeader();
        checkFileName();
        m_byteData = new int8_t[m_byteLength];
        m_fileStream.open(*m_fileName, std::ios::in);
        checkFileStream();
        m_fileStream.seekg(getHeaderSize(), std::ios::beg);
        m_fileStream.read(reinterpret_cast<char *>(m_byteData), m_byteLength);
        m_fileStream.close();

        m_Data = new T[m_dataLength];
        for (auto i = 0, j = 0; i < m_byteLength; i = i + 2, j++) {
            // Bitwise or is the same as adding two byte values together
            // In a 16 bit integer if we shift the second byte to be first we have a little endian byte form.
            // If we were on a big endian machine we sould swap the two terms either side of bitwise or around
            int16_t sample_i = ((m_byteData[i] & 0xff) | (m_byteData[i + 1] << 8));
            T sample_f;
            sample_f = sample_i / BIT16_RES;
            m_Data[j] = sample_f;
        }
    };

    void writeData() {
        // Check there is a file name
        checkFileName();
        // Open up and check the file stream
        m_fileStream.open(*m_fileName, std::ios::out);
        checkFileStream();
        // Write the m_headerIn of the file
        writeHeader(m_fileStream);

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

    void writeDataWithMute() {
        // Check there is a file name
        // Open up the File Stream
        checkFileName();
        m_fileStream.open(*m_fileName, std::ios::out);
        checkFileStream();
        // Write the header to the file
        writeHeader(m_fileStream);
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

    std::string *m_fileName{nullptr};
    std::fstream m_fileStream;
    int8_t *m_byteData{nullptr};
    size_t m_byteLength{0};
    T *m_Data{nullptr};
    size_t m_dataLength{0};
};


#endif //WAVE_IO_WAVE_H
