#include "Wave.h"

int main() {
    try {
        Wave<float> wave;
        wave.setFileNameIn("TrapBeatz_mono.wav");
        wave.readHeader();
        wave.getData();
        wave.setFileNameOut("test.wav");
        wave.writeData();
        wave.setFileNameOut("test_48000.wav");
        wave.changeSampleRate(48000);
        wave.writeData();
        wave.setFileNameOut("test_muted.wav");
        wave.changeSampleRate(44100);
        wave.writeDataWithMute();
    } catch (std::exception &e) {
        std::cout << "Exception Caught: " << e.what() << std::endl;
    }
    return 0;
}
