#include "bno.h"

BNO::BNO()
{
    // interface to BNO055 chip.
    // These calls must be in a class since otherwise it doesn't work well
}

bool BNO::Init()
{
    bool ret = false;

    this->pBNO055 = new BNO055(
        [this]() {return port.flush(); },
        [this]() {return port.available(); },
        [this](uint8_t* data, std::size_t length) {return port.write(data, length); },
        [this](uint8_t* data, std::size_t length) {return port.read(data, length); },
        false
    );

    std::cout << "Initialized BNO055 object" << std::endl;

    //Run the selftest
    auto selftest = pBNO055->runSelftest();
    std::cout << "Selftest result: " << std::endl;
    std::cout << "  0x0F is normal: " << std::hex << (int)selftest.selftest << std::dec << std::endl;
    if (selftest.status == 0x01)
    {
        std::cout << "System error: " << "0x" << std::hex << (int)selftest.error << std::dec << std::endl;
        return true;
    }
    //Read some more stuff
    auto revision = pBNO055->readRevision();
    std::cout << "Revisions:" << std::endl;
    std::cout << " Accel: " << revision.accelerometer << std::endl;
    std::cout << " Mag: " << revision.magnetometer << std::endl;
    std::cout << " Bootloader: " << revision.bootloader << std::endl;
    std::cout << " Software: " << revision.software << std::endl;

    std::cout << "System status: " << "0x" << std::hex << (int)pBNO055->readStatus() << std::dec << std::endl;

    auto calib = pBNO055->readCalibrationstatus();
    std::cout << "Calibration status: " << std::endl;
    std::cout << " System: " << (int)calib.system << std::endl;
    std::cout << " Gyro: " << (int)calib.gyro << std::endl;
    std::cout << " Accel: " << (int)calib.accel << std::endl;
    std::cout << " Magneto: " << (int)calib.mag << std::endl;

    return ret;
}

bool BNO::getEulers(double* pPitch, double* pRoll)
{
    bool ret = false;

    const auto result = pBNO055->readEuler();
    if (result)
    {
        const auto euler_angles = result.value();
        *pPitch = euler_angles.pitch;
        *pRoll = euler_angles.roll;
    }
    else
    {
        printf("BNO055 euler angle reading fails\n");
        ret = true;
    }

    return ret;
}
