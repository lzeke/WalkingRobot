#pragma once

class Servo
{
public:
    Servo(const char* name, int pin, int bias = 0);
    void Move(int position);
    int getPosition();
    const char* getName();
private:
    const char* name;
    int pca9685Pin;
    int bias;
    int lastPosition;
};


