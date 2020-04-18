#pragma once

#include "Task.h"

class RLP {
public:
    std::vector<Task *> &waiting;
private:
    std::vector<Task *> &red_ready;
    std::vector<Task *> &blue_ready

};