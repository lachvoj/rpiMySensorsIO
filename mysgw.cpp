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
void loop()
{
    // uint8_t spiOut = spiArduino.test(0b01101001);
    // std::bitset<8> bSpiOut(spiOut);
    // cout << "arduino responded: " << bSpiOut << endl;
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
        // cout << "loop ms: " << miliseconds << endl;
    }
}

void receive(const MyMessage &message)
{
#if MY_DEBUG
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
