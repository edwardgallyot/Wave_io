#include <iostream>
#include "Wave.h"



int main()
{
    std::cout << sizeof(float) << std::endl;

    Wave wave;
    wave.setFileName("bombay_pad.wav");
    wave.getHeader();
    wave.getData();
    return 0;
}
