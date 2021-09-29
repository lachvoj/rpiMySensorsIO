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
#include "json.hpp"

using json = nlohmann::json;

namespace pinCfg
{

bool Config::fileExists(const string &filePath)
{
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
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
        uint8_t outPin;
        try
        {
            outPin = itSw->at("outPin");
        }
        catch (const json::out_of_range &e)
        {
            cerr << "Error: outPin not present in: " << *itSw << endl;
            exit(3);
        }
        Switch::Mode md = static_cast<Switch::Mode>(itSw->value("mode", Switch::Mode::CLASSIC));
        int impulseDuration = itSw->value("impulseDuration", 300);
        uint8_t feedbackPin = itSw->value("feedbackPin", 0);
        bool present = itSw->value("present", true);
        uint8_t id = present ? presentables.size() : -1;
        shared_ptr<Switch> spSw =
            make_shared<Switch>(id, itSw.key(), present, outPin, md, impulseDuration, feedbackPin);
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
    catch (const std::exception &e)
    {
    }

    // inputPins
    map<uint8_t, shared_ptr<InPin>> inputPins;
    for (json::iterator itInputPin = config["inputs"]["inputPins"].begin();
         itInputPin != config["inputs"]["inputPins"].end();
         ++itInputPin)
    {
        uint8_t inPin;
        try
        {
            inPin = itInputPin->at("pin");
        }
        catch (const json::out_of_range &e)
        {
            cerr << "WARNING: pin not present in: " << *itInputPin << endl;
            continue;
        }

        bool present = itInputPin->value("present", true);
        uint8_t id = present ? presentables.size() : -1;
        shared_ptr<InPin> spInPin = make_shared<InPin>(id, itInputPin.key(), present, inPin);
        inputPins[inPin] = spInPin;
        loopables.push_back(spInPin);
        if (present)
            presentables.push_back(spInPin);
    }

    // triggers
    for (auto &trigger : config["inputs"]["triggers"])
    {
        // cout << trigger << endl;
        uint8_t inPin;
        try
        {
            inPin = trigger.at("inPin");
        }
        catch (const json::out_of_range &e)
        {
            cerr << "WARNING: inPin not present in: " << trigger << endl << "Trigger won't be created." << endl;
            continue;
        }

        map<uint8_t, shared_ptr<InPin>>::iterator itr = inputPins.find(inPin);
        if (itr == inputPins.end())
        {
            shared_ptr<InPin> clInPin = make_shared<InPin>(-1, "nd", false, inPin);
            itr = inputPins.insert(itr, pair<uint8_t, shared_ptr<InPin>>(inPin, clInPin));
            loopables.push_back(clInPin);
        }

        vector<Trigger::SwitchAction> vSwitches;
        json drives;
        try
        {
            drives = trigger.at("drives");
        }
        catch (const std::exception &e)
        {
            cout << "WARNING: Trigger: " << endl << trigger << endl << "Doesn't contain \"drives\" element." << endl;
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
                    catch (const std::exception &e)
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
    }
}
} // namespace pinCfg