#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ILoopable.hpp"
#include "IMySensorsPresentable.hpp"

#ifndef PINCFGPATH
#define PINCFGPATH "./config.json"
#endif

using namespace std;

namespace pinCfg
{

class Config
{
  private:
    static bool fileExists(const string &filePath);
    static void splitString(const string& str, char delimiter, vector<string> &out);

  public:
    static void readConfigFromFile(
        bool &remoteConfigEnbled,
        vector<shared_ptr<ILoopable>> &loopables,
        vector<shared_ptr<IMySensorsPresentable>> &presentables);
    static bool verifyConfiguration(const string &confStr, stringstream &err);
};
} // namespace pinCfg
