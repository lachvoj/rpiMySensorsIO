#include <MySensors.h>
#include <algorithm>
#include <bitset>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <unistd.h>
#include <vector>

#include "Config.hpp"
#include "ILoopable.hpp"
#include "IMySensorsPresentable.hpp"
#include "SPIArduino.hpp"

#define MY_GATEWAY_MAX_CLIENTS 10
#define MY_DEBUG

vector<shared_ptr<pinCfg::ILoopable>> loopables;
vector<shared_ptr<pinCfg::IMySensorsPresentable>> presentables;
bool remoteConfigEnbled = true;
pinCfg::SPIArduino spiArduino;

void setup()
{
    // Setup locally attached sensors
    pinCfg::Config::readConfigFromFile(remoteConfigEnbled, loopables, presentables);
    loopables.shrink_to_fit();
    presentables.shrink_to_fit();
}

void presentation()
{
    // Present locally attached sensors here
    sendSketchInfo("RPI configurable IOs.", "1.0");
    for (auto &presentable : presentables)
    {
        presentable->Present();
    }
}

bool initialValueSent = false;
#ifdef MY_DEBUG
uint64_t lastLoopMs = (uint64_t)millis();
uint64_t msPerPeriod = 0;
uint32_t loopCount = 0;
#define LOOP_AVG_COUNT 1000
#endif
void loop()
{
    if (!initialValueSent)
    {
        // Send locally attached sensors data here
        for (auto &presentable : presentables)
        {
            presentable->PresentState();
        }
        wait(2000, C_SET, V_STATUS);
    }
    else
    {
        uint64_t miliseconds = (uint64_t)millis();
        for (shared_ptr<pinCfg::ILoopable> &loopable : loopables)
        {
            loopable->loop(miliseconds);
        }
#ifdef MY_DEBUG
        if (loopCount == LOOP_AVG_COUNT)
        {
            uint32_t avg = msPerPeriod / LOOP_AVG_COUNT;
            cout << "Avg loop time per " << LOOP_AVG_COUNT << " loops is: " << avg << "ms" << endl;
            loopCount = 0;
            msPerPeriod = 0;
        }
        msPerPeriod += miliseconds - lastLoopMs;
        lastLoopMs = miliseconds;
        loopCount++;
#endif
    }
}

void receive(const MyMessage &message)
{
#ifdef MY_DEBUG
    if (message.isAck())
        cout << "ACK from gateway.";
#endif

    if (message.type == V_STATUS || message.type == V_TEXT)
    {
        if (!initialValueSent)
            initialValueSent = true;

        if (message.getDestination() != 0)
            return;

        uint8_t id = message.getSensor();
        shared_ptr<pinCfg::IMySensorsPresentable> fsw;
        try
        {
            fsw = presentables.at(id);
        }
        catch (const out_of_range &oor)
        {
            cerr << "Presentable with id:" << id << " is not present." << endl;
            return;
        }
        fsw->rcvMessage(message);
    }
}
