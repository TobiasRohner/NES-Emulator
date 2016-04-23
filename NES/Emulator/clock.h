#ifndef CLOCK_H
#define CLOCK_H

#include <memory>
#include <condition_variable>
#include <mutex>


class Clock
{
public:
    Clock(std::shared_ptr<std::condition_variable> clk, std::chrono::nanoseconds period);

    void start();
    void stop();

private:
    std::shared_ptr<std::condition_variable> clk;
    std::chrono::nanoseconds period;

    std::mutex clockRunningMutex;
    bool clockRunning = false;
    bool running();

    void run();
};

#endif // CLOCK_H
