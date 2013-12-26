#include <chrono>

#include "timer.hpp"

struct timer::timer_private {
    static const double r;
    std::chrono::steady_clock::time_point start;
};

const double timer::timer_private::r = (double) std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den;

timer::timer() {
    p = std::unique_ptr<timer::timer_private>(new timer::timer_private());
    reset();
}

timer::~timer() {}
    
void timer::reset() {
    p->start = std::chrono::steady_clock::now();
}

double timer::elapsed() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration duration = now - p->start;
    return duration.count() * p->r;
}
