#include "Wave.h"

int main() {
    Wave<float> wave;
    try {
        wave.readFile("bombay_pad_mono.wav");
        wave.printFileName();
        wave.printInfo();
        wave.writeFile("test.wav");
        wave.changeSampleRate(48000);
        wave.writeFile("test_48000.wav");
        std::cout << std::endl;
        std::cout << std::endl;
        wave.printFileName();
        wave.printInfo();
        wave.changeSampleRate(44100);
        wave.writeFileWithMute("test_muted.wav");
    } catch (std::exception &e) {
        std::cout << "Exception Caught: " << e.what() << std::endl;
    }
    return 0;
}