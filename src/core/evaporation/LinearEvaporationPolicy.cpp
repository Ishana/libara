/*
 * $FU-Copyright$
 */

#include "LinearEvaporationPolicy.h"

#include <cmath>
#include <iostream>

using namespace ARA;

LinearEvaporationPolicy::LinearEvaporationPolicy(float evaporationFactor, float threshold, unsigned int timeIntervalInMilliSeconds) : EvaporationPolicy(timeIntervalInMilliSeconds) {
    this->evaporationFactor = evaporationFactor;
    this->threshold = threshold;
}

float LinearEvaporationPolicy::evaporate(float oldPheromoneValue, int millisecondsSinceLastEvaporation){
    if(millisecondsSinceLastEvaporation == 0) {
        return oldPheromoneValue;
    } else {
        float multiplicator = millisecondsSinceLastEvaporation / timeInterval;
        float newPheromoneValue = oldPheromoneValue * pow(evaporationFactor, multiplicator);

        if (newPheromoneValue < threshold) {
            newPheromoneValue = 0;
        }

        return newPheromoneValue;
    }
}
