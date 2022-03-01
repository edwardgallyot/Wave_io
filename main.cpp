#include "Wave.h"

int main() {
    Wave<float> wave;
    try {
        wave.readFile("3_Channels.wav");
        wave.printFileName();
        wave.printInfo();
//        wave.writeFile("test.wav");
//        wave.writeFile("test_48000.wav");\
//        wave.writeFileWithMute("test_muted.wav");
//        wave.writeFileWithNewSampleRate("test_22050.wav",22050);
//        wave.writeFileWithNewSampleRate("test_48000.wav",48000);
//        wave.writeFileWithNewSampleRate("test_88200.wav",88200);
        wave.readMultiChannelFile("3_Channels.wav");
        wave.printInfo();
//        wave.writeMultiChannelData();
//        wave.writeFileWithBitRate("test_8bit.wav", 8);

    } catch (std::exception &e) {
        std::cout << "Exception Caught: " << e.what() << std::endl;
    }
    return 0;
}