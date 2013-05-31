#ifndef COMMON_H
#define COMMON_H

#define SV_THRESHOLD 0.00001
#define SV_DISTINCT_MAX 0.4
#define SV_DISTINCT_MIN 0.00001

#define COMMON_WEIGHT_POWER 0.49
#define DISTINCT_WEIGHT_POWER 0.75

#define VERBOSITY_LEVEL 2

#include <iostream>

using namespace std;

enum Domain {LEARNING_DOMAIN = 0, SAMPLE_DOMAIN = 1, TEST_DOMAIN = 2};

struct DistanceTo {
    DistanceTo(int _pid, double _distance) : pid(_pid), distance(_distance) {}
    bool operator< (const DistanceTo& a) const {
        return (distance < a.distance);
    }
    int pid;
    double distance;
};

#endif