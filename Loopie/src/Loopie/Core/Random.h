#pragma once
#include <random>

namespace Loopie {

    class Random {
    private:
        inline static std::mt19937 rng = std::mt19937(std::random_device{}());

    public:
        static int Get(int min, int max, bool minInclusive = true) {
            if (!minInclusive) {
                min += 1;
            }
            std::uniform_int_distribution<int> dist(min, max);
            return dist(rng);
        }

        static float Get(float min, float max) {
            std::uniform_real_distribution<float> dist(min, max);
            return dist(rng);
        }

        static double Get(double min, double max) {
            std::uniform_real_distribution<double> dist(min, max);
            return dist(rng);
        }
    };
}