//
// Created by kemptonburton on 11/14/2025.
//

#ifndef BASEMODULE_H
#define BASEMODULE_H

#include <yaml-cpp/yaml.h>
#include "sdk_api.h"

/*

For DARTWIC to find the module export, the .main file should export a function called <createModule> that makes a class
that inherits this class.

The cmake project should then be exported as a shared library.

#ifdef _WIN32
    #define EXPORT_API __declspec(dllexport)
#else
    #define EXPORT_API __attribute__((visibility("default")))
#endif

extern "C" EXPORT_API DARTWIC::Modules::BaseModule* createModule(YAML::Node cfg, DARTWIC::API::SDK_API* drtw) {
    return new MyModule(cfg, drtw);
}

*/

namespace DARTWIC::Modules {

    class BaseModule {
    public:
        virtual ~BaseModule() = default;
        BaseModule(YAML::Node cfg, API::SDK_API* drtw) : config(cfg), dartwic(drtw) {}

        template <typename T>
        T getConfig(const std::string& key, const T& default_value = T()) const {
            if (config && config[key])
                return config[key].as<T>();
            return default_value;
        }

        template <typename T>
        T getParameter(const std::string& key, const T& default_value = T()) const {
            if (config && config["parameters"][key])
                return config["parameters"][key].as<T>();
            return default_value;
        }

        YAML::Node config;
        API::SDK_API* dartwic = nullptr;
    };

}

#endif //BASEMODULE_H
