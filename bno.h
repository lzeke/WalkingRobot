#pragma once

#include "../bno055_cpp/ext/serial/include/serial/serial.h"
#include "../bno055_cpp/src/BNO055.h"

class BNO
{
public:
    BNO();
    bool Init();
    bool getEulers(double* pPitch, double* pRoll);
private:
    std::string port_name = "/dev/ttyAMA0";
    serial::Serial port = serial::Serial(port_name, 115200);
    BNO055* pBNO055;
};