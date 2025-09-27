#include <libutils/chrono.hpp>

#include <iostream>
#include <thread>

int main()
{
    auto f = [](int ms_sleep) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms_sleep));
    };

    auto t =
        utils::chrono::bench::perf_timer<std::chrono::microseconds>::duration(
            f, 10);
    auto microseconds = std::chrono::duration<double, std::micro>(t).count();
    std::cout << "Took: " << microseconds << " microseconds" << '\n';

    auto milliseconds = std::chrono::duration<double, std::milli>(t).count();
    std::cout << "Took: " << milliseconds << " milliseconds" << '\n';

    return 0;
}
