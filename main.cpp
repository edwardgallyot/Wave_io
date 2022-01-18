#include "Wave.h"

int main() {
    Wave<float> wave;
    try {
        wave.readFile("bombay_pad_mono.wav");
        wave.printFileName();
        wave.printInfo();
        wave.writeFile("test.wav");
        wave.writeFile("test_48000.wav");\
        wave.writeFileWithMute("test_muted.wav");
        wave.writeFileWithNewSampleRate("test_22050.wav",22050);
        wave.writeFileWithNewSampleRate("test_48000.wav",48000);
        wave.writeFileWithNewSampleRate("test_88200.wav",88200);
    } catch (std::exception &e) {
        std::cout << "Exception Caught: " << e.what() << std::endl;
    }
    return 0;
}