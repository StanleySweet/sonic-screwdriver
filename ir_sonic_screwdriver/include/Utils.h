
#include <cstdint>
#include <functional>

namespace Utils
{
    void IntervalBlink(uint8_t pin, unsigned long interval, unsigned long *previousExecutionTime);
    void IntervalAction(std::function<void()> func, unsigned long interval, unsigned long *previousExecutionTime);
}
