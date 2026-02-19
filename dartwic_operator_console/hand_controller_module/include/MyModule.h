//
// Created by kemptonburton on 11/14/2025.
//

#ifndef MYMODULE_H
#define MYMODULE_H

#include <iostream>
#include <BaseModule.h>

class MyModule : public DARTWIC::Modules::BaseModule {
public:
    MyModule(YAML::Node cfg, DARTWIC::API::SDK_API* drtw) : BaseModule(cfg, drtw) {
        std::string device_name = getParameter<std::string>("example_parameter_1");

        std::cout << device_name << std::endl; //should print 0

        dartwic->onLoop("module_example", [&]() {
            double val = dartwic->queryChannelValue("module_example", "test_channel_1", 10.0);

            std::cout << "test_channel_1: " << val << std::endl;
        });
    };

    ~MyModule() override {
        dartwic->removeLoop("module_example");
    };

};

#endif //MYMODULE_H

