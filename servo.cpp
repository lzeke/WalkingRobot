#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include <math.h>
#include <cstring>
#include <chrono>
#include "servo.h"
#include <PCA9685.h>


extern PiPCA9685::PCA9685* pPCA;
extern Servo* pRightFootServo;
extern Servo* pLeftFootServo;
extern bool debug;

Servo::Servo(const char* name, int pin, int bias)
{
    this->name = (char*)malloc(strlen(name) + 1);
    strcpy((char*)this->name, name);
    this->pca9685Pin = pin;
    this->bias = bias;
    this->lastPosition = -1;
}

int Servo::getPosition()
{
    return this->lastPosition;
}

const char* Servo::getName()
{
    return this->name;
}

void Servo::Move(int position) // position: 0-180
{
    //if(this == pRightFootServo || this == pLeftFootServo) position = 180 - position;

    if (position != this->lastPosition)
    {
        if (debug) printf("%s->Move(90 %d)\n", this->name, position - 90);
        position = fmax(0, fmin(180, position));
        // 0.5: 0degree, 1.5ms:90degree, 2.5ms:180 degree
        double value = 2.0 * (((double)(position + this->bias)) / 180.0) + 0.5;
        //printf("%s: pin=%d, bias=%d, angle=%d, value=%f\n", this->name, this->pca9685Pin, this->bias, position, value);
        pPCA->set_pwm_ms(this->pca9685Pin, value);
        this->lastPosition = position;
        usleep(1000); // wait 1ms
    }
    else
    {
        //printf("pin=%d, bias=%d, angle=%d did not change\n", this->pca9685Pin, this->bias, position);
    }
}
