#include <cstdint>
#include <thread>
#include <cstdio>

struct Pool
{
    thread_local static uint32_t cnt;
};

thread_local uint32_t Pool::cnt = 0;

int main()
{
    Pool pool;
    for (uint32_t i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        std::thread([=, &pool]()
                    {
            pool.cnt = 0xf;
            printf("--%d: %d--\n", i, pool.cnt); })
            .detach();
    }

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(200ms);
}