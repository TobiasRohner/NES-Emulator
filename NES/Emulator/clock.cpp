#include "clock.h"
#include <thread>

Clock::Clock(std::shared_ptr<std::condition_variable> clk, std::chrono::nanoseconds period)
{
    this->clk = clk;
    this->period = period;
}

void Clock::start() {
    clockRunningMutex.lock();
    clockRunning = true;
    clockRunningMutex.unlock();
    run();
}

void Clock::stop() {
    clockRunningMutex.lock();
    clockRunning = false;
    clockRunningMutex.unlock();
}

bool Clock::running() {
    clockRunningMutex.lock();
    bool r = clockRunning;
    clockRunningMutex.unlock();
    return r;
}

void Clock::run() {
    for (;;) {
        if (running()) {
            std::this_thread::sleep_for(period);
            clk.get()->notify_all();
        }
        else {
            break;
        }
    }
}
