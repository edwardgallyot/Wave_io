#include "Wave.h"

int main()
{
    Wave<float> wave;
    wave.setFileNameIn("TrapBeatz_mono.wav");
    wave.getHeader();
    wave.getData();
    wave.setFileNameOut("test.wav");
    wave.writeData();
    wave.setFileNameOut("test_48000.wav");
    wave.changeSampleRate(48000);
    wave.writeData();
    wave.setFileNameOut("test_muted.wav");
    wave.changeSampleRate(44100);
    wave.writeDataWithMute();
    return 0;
}
