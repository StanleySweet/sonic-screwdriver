
#include <cstdint>
#include <functional>

namespace Utils
{
    void IntervalAction(std::function<void()> func, unsigned long interval, unsigned long *previousExecutionTime);
}
