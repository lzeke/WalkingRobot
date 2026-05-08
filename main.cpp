#include <gpiod.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <PCA9685.h>
#include <chrono>
#include <thread>
#include <mutex>

#include "servo.h"
#include "bno.h"

using namespace std;

#include <sys/ioctl.h>
extern "C"
{
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
}

const int rightFootServoPin = 2;
const int rightKneeServoPin = 1;
const int rightHipServoPin = 0;

const int leftFootServoPin = 13;
const int leftKneeServoPin = 14;
const int leftHipServoPin = 15;

bool debug = true; // global debug flag to turn on debug logging
bool stopProgram; // if this is set to true, the program execution loop stops

struct gpiod_chip* pChip;
PiPCA9685::PCA9685* pPCA;

Servo* pRightFootServo = NULL;
Servo* pRightKneeServo = NULL;
Servo* pRightHipServo = NULL;
Servo* pRightHipRollServo = NULL;
Servo* pRightTurnServo = NULL;

Servo* pLeftFootServo = NULL;
Servo* pLeftKneeServo = NULL;
Servo* pLeftHipServo = NULL;
Servo* pLeftHipRollServo = NULL;
Servo* pLeftTurnServo = NULL;

BNO* pBNO;

// balanceWalk(): 100% Claude AI generated 6-phase walking algorithm. Pretty unstable.
void balanceWalk();

// testSingleLegSmooth(): Claude AI generated center-of-mass movement test based on my suggestions 
void testSingleLegSmooth();

// walkSmooth(): Claude AI generated walking algorithm based on my suggestions. Pretty unstable.
void walkSmooth();

//  myWalkSmoothTurn(): My 4=phase walking algorithm, refined by Claude AI. The most stable of the walking algorithms
void myWalkSmoothTurn(double turnBias, int maxSteps = 0);

// initializing the system
bool Init()
{ // returns false if success, true otherwise
    bool ret = false;

    const char* pChipName = "gpiochip0";
    if (::debug)
        printf("Init starting.\n");

    // Open GPIO chip
    if ((pChip = gpiod_chip_open_by_name(pChipName)) == NULL)
    {
        printf("ERROR: can't open gpio chip\n");
        ret = true;
    }
    else
    {
        if (::debug)
            printf("pChip is initialized.\n");
    }

    if (!ret)
    {
        // Open PCA9685 board and set its frequency to 400Hz
        try
        {
            pPCA = new PiPCA9685::PCA9685();
            pPCA->set_pwm_freq(400.0);
            if (::debug)
                printf("PCA9685 is initialized.\n");
        }
        catch (std::exception e)
        {
            printf("ERROR: Exception %s \n", e.what());
        }
    }

    if (!ret)
    {
        pBNO = new BNO();
        if (!pBNO->Init())
        {
            printf("BNO055 is initialized\n");
        }
        else
        {
            printf("BNO055 initialized fails\n");
            ret = true;
        }
    }

    stopProgram = false;

    if (!ret)
        printf("Init finished with SUCCESS.\n");

    return ret;
}

void StandupStraight()
{
    if (pRightHipServo != NULL) pRightHipServo->Move(90);
    if (pRightKneeServo != NULL) pRightKneeServo->Move(90);
    if (pRightFootServo != NULL) pRightFootServo->Move(90);

    if (pLeftHipServo != NULL) pLeftHipServo->Move(90);
    if (pLeftKneeServo != NULL) pLeftKneeServo->Move(90);
    if (pLeftFootServo != NULL) pLeftFootServo->Move(90);
}

bool Setup()
{
    bool ret = false;

    pRightFootServo = new Servo("rightFoot", rightFootServoPin, 6);//2);
    pRightKneeServo = new Servo("rightKnee", rightKneeServoPin, 16);
    pRightHipServo = new Servo("rightHip", rightHipServoPin);

    pLeftFootServo = new Servo("leftFoot", leftFootServoPin, 3);
    pLeftKneeServo = new Servo("leftKnee", leftKneeServoPin, -3);
    pLeftHipServo = new Servo("leftHip", leftHipServoPin, 7);

    StandupStraight();

    printf("Setup done.\nREADY!\n");

    ::debug = false;

    return ret;
}

bool Finish()
{
    bool ret = false;
    printf("Finish\n");
    StandupStraight();

    gpiod_chip_close(pChip);
    sleep(1);
    return ret;
}

void TestServo(Servo* pServo)
{
    pServo->Move(90);
    sleep(1);

    for (int i = 60; i < 140; i += 10)
    {
        pServo->Move(i);
        sleep(1);
    }

    pServo->Move(90);
}

void TestBNO()
{ // move the robot by hand to see if the BNO shows the proper pitch and roll
    for (int i = 0; i < 10; i++)
    {
        double pitch, roll;
        pBNO->getEulers(&pitch, &roll);
        printf("BNO055:  pich=%6.2f roll=%6.2f\n", pitch, roll);
        sleep(1);
    }
}

void TestRobot()
{
    //TestServo(pLeftFootServo);
    //TestBNO();

    //balanceWalk();

    //testSingleLegSmooth();
    //walkSmooth();

    myWalkSmoothTurn(0, 16);
    //myWalkSmoothTurn(8,  8);   
    //myWalkSmoothTurn(-8, 8);
}

int main(int argc, char** argv)
{
    bool ret = false;

    if (::debug)
        printf("Start\n");

    if (!ret)
        ret = Init();

    if (!ret)
        ret = Setup();

    if (!ret)
    {
        TestRobot();
    }

    Finish();
}

