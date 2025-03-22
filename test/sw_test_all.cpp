#include "sw/sw.hpp"

#include <atomic>
#include <future>
#include <iostream>
#include <memory>
#include <string>

using sw::threadpool_settings;
using sw::threadpool;
using sw::logger;
using sw::counter;

std::atomic<bool> simple_flag = true;

auto lp = logger::get_logger("root");

void hp_logging() {
    while (simple_flag.load()) {
        lp->info(EVENT("Hello World."));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

bool time_consuming_task() {
    counter<::std::chrono::high_resolution_clock> c("flag", [](const ::std::string& key, ::std::uint64_t elapsed) { 
        lp->info(EVENT(key + " finished within " + ::std::to_string(elapsed) + "ms")); });
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return false;
}

int main() {
    threadpool_settings ts;
    auto tp = threadpool::create(ts);
    tp->execute(hp_logging);
    auto flag = tp->submit(time_consuming_task);
    simple_flag.store(flag.get());
    tp->shutdown();
    return 0;
}
