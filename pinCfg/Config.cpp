#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>

#include "Config.hpp"
#include "ExtCfgReciever.hpp"
#include "InPin.hpp"
#include "Switch.hpp"
#include "Trigger.hpp"

namespace pinCfg
{
const char *Config::invPinFormatMsg("Warning: pin string has invalid format. Format: \"0.0\" or \"0\" "
                                    "where first variant is <spi device>.<spi "
                                    "device output pin> or second variant is just <output pin>");
const char *Config::invFbPinFormatMsg("Warning: feedbackPin string has invalid format. Format: \"0.0\" or \"0\" "
                                      "where first variant is <spi device>.<spi "
                                      "device input pin> or second variant is just <input pin>");

bool Config::fileExists(const string &filePath)
{
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}

void Config::splitString(const string &str, char delimiter, vector<string> &out)
{
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delimiter, end)) != string::npos)
    {
        end = str.find(delimiter, start);
        out.push_back(str.substr(start, end - start));
    }
}

int Config::getPinFromJson(uint8_t &pin, uint8_t &dev, const json &pinJson)
{
    if (pinJson.is_string())
    {
        vector<string> vDevPin;
        splitString(string(pinJson), '.', vDevPin);
        if (vDevPin.size() == 1)
        {
            try
            {
                pin = stoi(vDevPin[0]);
            }
            catch (const exception &e)
            {
                return -1;
            }
        }
        else if (vDevPin.size() > 1)
        {
            try
            {
                dev = stoi(vDevPin[0]);
                pin = stoi(vDevPin[1]);
            }
            catch (const exception &e)
            {
                return -1;
            }
        }
    }
    else if (pinJson.is_number())
        pin = pinJson;
    else
    {
        return -1;
    }

    return 0;
}

void Config::readConfigFromFile(
    bool &remoteConfigEnbled,
    vector<shared_ptr<ILoopable>> &loopables,
    vector<shared_ptr<IMySensorsPresentable>> &presentables)
{
    if (!fileExists(PINCFGPATH))
    {
        cerr << "No config file!" << endl;
        exit(1);
    }

    ifstream cfgFile(PINCFGPATH);
    json config;
    try
    {
        config = json::parse(cfgFile);
    }
    catch (json::parse_error &e)
    {
        cerr << "message: " << e.what() << endl
             << "exception id: " << e.id << endl
             << "byte position of error: " << e.byte << endl;
        exit(2);
    }

    // remote config enabled
    if (config["remoteConfigEnabled"].is_boolean())
        remoteConfigEnbled = static_cast<bool>(config["remoteConfigEnabled"]);
    if (remoteConfigEnbled)
        presentables.push_back(make_shared<ExtCfgReciever>(presentables.size(), "configurationReceiver"));

    // switches
    map<string, shared_ptr<Switch>> mSwitches;
    for (json::iterator itSw = config["switches"].begin(); itSw != config["switches"].end(); ++itSw)
    {
        json pin;
        try
        {
            pin = itSw->at("pin");
        }
        catch (const json::out_of_range &e)
        {
            cerr << "Warning: pin not present in: " << *itSw << endl;
            continue;
        }

        uint8_t nPin = 0;
        uint8_t nDev = 0xFF;
        if (getPinFromJson(nPin, nDev, pin) < 0)
        {
            cerr << invPinFormatMsg << endl;
            continue;
        }

        json feedbackPin;
        try
        {
            feedbackPin = itSw->at("feedbackPin");
        }
        catch (const json::out_of_range &e)
        {
        }
        catch (...)
        {
        }

        uint8_t nFbPin = 0;
        uint8_t nFbDev = 0xFF;
        if (!feedbackPin.is_null())
        {
            if (getPinFromJson(nFbPin, nFbDev, feedbackPin) < 0)
            {
                cerr << invFbPinFormatMsg << endl;
                continue;
            }
        }

        Switch::Mode md = static_cast<Switch::Mode>(itSw->value("mode", Switch::Mode::CLASSIC));
        int impulseDuration = itSw->value("impulseDuration", 300);
        bool present = itSw->value("present", false);
        uint8_t id = present ? presentables.size() : -1;
        shared_ptr<Switch> spSw =
            make_shared<Switch>(id, itSw.key(), present, md, nPin, nDev, nFbPin, nFbDev, impulseDuration);
        mSwitches[itSw.key()] = spSw;
        loopables.push_back(spSw);
        if (present)
            presentables.push_back(spSw);
    }

    // inputs common
    json inputsConfig;
    try
    {
        config.at("inputs").at("config").get_to(inputsConfig);

        if (inputsConfig.find("debounce") != inputsConfig.end())
            InPin::setDebounceMs((uint64_t)inputsConfig["debounce"]);
        if (inputsConfig.find("multiMaxDelay") != inputsConfig.end())
            InPin::setMulticlickMaxDelayMs((uint64_t)inputsConfig["multiMaxDelay"]);
    }
    catch (const exception &e)
    {
    }

    // inputPins
    map<string, shared_ptr<InPin>> inputPins;
    for (json::iterator itInputPin = config["inputPins"].begin(); itInputPin != config["inputPins"].end(); ++itInputPin)
    {
        json inPin;
        try
        {
            inPin = itInputPin->at("pin");
        }
        catch (const json::out_of_range &e)
        {
            cerr << "WARNING: pin not present in: " << *itInputPin << endl;
            continue;
        }

        uint8_t pin = 0;
        uint8_t dev = 0xFF;
        if (getPinFromJson(pin, dev, inPin) < 0)
        {
            cerr << invPinFormatMsg << endl;
            continue;
        }

        bool present = itInputPin->value("present", false);
        uint8_t id = present ? presentables.size() : -1;
        shared_ptr<InPin> spInPin = make_shared<InPin>(id, itInputPin.key(), present, pin, dev);
        inputPins[itInputPin.key()] = spInPin;
        loopables.push_back(spInPin);
        if (present)
            presentables.push_back(spInPin);
    }

    // triggers
    for (auto &trigger : config["triggers"])
    {
        string inPin;
        try
        {
            inPin = trigger.at("inPin");
        }
        catch (const json::out_of_range &e)
        {
            cerr << "WARNING: inPin not present in: " << trigger << endl << "Trigger won't be created." << endl;
            continue;
        }

        map<string, shared_ptr<InPin>>::iterator itr = inputPins.find(inPin);
        if (itr == inputPins.end())
        {
            cerr << "WARNING: inPin with name: " << inPin << " couldn't be found in inputPins." << endl
                 << "Trigger won't be created." << endl;
            continue;
        }

        vector<Trigger::SwitchAction> vSwitches;
        json drives;
        try
        {
            drives = trigger.at("drives");
        }
        catch (const exception &e)
        {
            cout << "WARNING: Trigger: " << endl
                 << trigger << endl
                 << "Doesn't contain \"drives\" element." << endl
                 << "Trigger won't be created." << endl;
            continue;
        }

        if (drives.is_array())
        {
            for (auto &sw : drives)
            {
                string sSw;
                Trigger::Action eAction = Trigger::Action::A_TOGGLE;
                if (sw.is_string())
                {
                    sSw = sw;
                }
                else if (sw.is_object())
                {
                    try
                    {
                        sSw = sw.at("switch");
                    }
                    catch (const exception &e)
                    {
                        cout << "WARNING: drives element : " << endl
                             << sw << endl
                             << "Doesn't contain switch ... skipping." << endl;
                    }
                    eAction = static_cast<Trigger::Action>(sw.value("action", 0));
                }
                else
                {
                    cout << "WARNING: drives element: " << endl
                         << sw << endl
                         << "Contains unsupported element type ... skipping." << endl;
                }
                map<string, shared_ptr<Switch>>::iterator ite = mSwitches.find(sSw);
                if (ite == mSwitches.end())
                {
                    cerr << "WARNING: drives contains undefinded switch in: " << sw << endl;
                    continue;
                }
                vSwitches.emplace_back(ite->second, eAction);
            }
        }
        else if (drives.is_string())
        {
            map<string, shared_ptr<Switch>>::iterator ite = mSwitches.find(drives);
            if (ite == mSwitches.end())
            {
                cerr << "WARNING: drives contains not definded switch in: " << trigger << endl
                     << "Trigger won't be created." << endl;
                continue;
            }
            vSwitches.emplace_back(ite->second);
        }
        else
        {
            cerr << "WARNING: drives contains not supported value in: " << trigger << endl
                 << "Trigger won't be created." << endl;
            continue;
        }

        uint8_t eventType = trigger.value("event", 1);
        uint8_t count = trigger.value("count", 1);

        shared_ptr<IPinSubscriber> subscriber =
            static_pointer_cast<IPinSubscriber>(make_shared<Trigger>(vSwitches, eventType, count));
        itr->second->addSubscriber(subscriber);
    }
}

bool Config::verifyConfiguration(const string &confStr, stringstream &err)
{
    json cfg;
    try
    {
        cfg = json::parse(confStr);
    }
    catch (json::parse_error &e)
    {
        err << "message: " << e.what() << endl
            << "exception id: " << e.id << endl
            << "byte position of error: " << e.byte << endl;
        return false;
    }

    return true;
}
} // namespace pinCfg
