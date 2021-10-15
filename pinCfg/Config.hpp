#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ILoopable.hpp"
#include "IMySensorsPresentable.hpp"
#include "json.hpp"

#ifndef PINCFGPATH
#define PINCFGPATH "./config.json"
#endif

using namespace std;
using json = nlohmann::json;

namespace pinCfg
{

class Config
{
  private:
    static const char *invPinFormatMsg;
    static const char *invFbPinFormatMsg;
    static bool fileExists(const string &filePath);
    static void splitString(const string &str, char delimiter, vector<string> &out);
    static int getPinFromJson(uint8_t &pin, uint8_t &dev, const json &pinJson);

  public:
    static void readConfigFromFile(
        bool &remoteConfigEnbled,
        vector<shared_ptr<ILoopable>> &loopables,
        vector<shared_ptr<IMySensorsPresentable>> &presentables);
    static bool verifyConfiguration(const string &confStr, stringstream &err);
};
} // namespace pinCfg
