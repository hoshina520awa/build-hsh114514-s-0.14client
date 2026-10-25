#ifndef TIMER_H
#define TIMER_H

class Timer {
public:
    float ticksPerSecond; //0
    int elapsedTicks; //4
    float renderPartialTicks; //8
    float timerSpeed; //12
    float elapsedPartialTicks; //16
};

#endif //TIMER_H
