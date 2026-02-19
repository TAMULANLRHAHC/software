#ifndef SDK_API_H
#define SDK_API_H

#include <functional>
#include <string>
#include <optional>
#include <vector>

/*
    API header for the Software Development Kit
    Used in BaseModule.h header to access DARTWIC objects and functions.
*/

namespace DARTWIC::API {

    class SDK_API {
    public:
        SDK_API() = default;
        virtual ~SDK_API() = default;

        /// RAPID ///
        virtual double queryChannelValue(const std::string& portal, const std::string& channel,
            std::optional<double> default_value) = 0;

        virtual void insertChannelValue(const std::string& portal, const std::string& channel,
            double value) = 0;

        virtual void upsertChannelValue(const std::string& portal, const std::string& channel,
            double value) = 0;

        virtual void upsertChannelValueBulk(const std::string& portal, const std::string& channel,
                                     const std::vector<std::pair<double, uint64_t>>& data) = 0;

        virtual bool removeChannel(const std::string& portal, const std::string& channel) = 0;

        /// DARTWIC LOOPS ///
        virtual void onStart(const std::string &loop_name, const std::function<void()> &function) = 0;
        virtual void onLoop(const std::string &loop_name, const std::function<void()> &function) = 0;
        virtual void onEnd(const std::string &loop_name, const std::function<void()> &function) = 0;

        virtual void removeLoop(const std::string &loop_name) = 0;

        /// CONSOLE ///
        virtual int consoleError(
            // message type that the subcategory supports | error, warning, message, prompts, etc
            std::string message_title,  // title of the message
            std::string message_description, // description of the message
            std::vector<std::string> tags, // tags the message will show
            std::string resolution, // recommended resolution
            int auto_acknowledge = 0
        ) = 0;
    };

}

#endif //SDK_API_H
