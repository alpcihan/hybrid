#pragma once

#include "hybrid/hybrid_shared.hpp"

namespace hybrid {

class Timer {
public:
    Timer();

    void reset();
    float elapsed();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

}  // namespace hybrid