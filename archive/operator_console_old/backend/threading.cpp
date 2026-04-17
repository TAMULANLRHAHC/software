#include "threading.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include "haos.h"
#include "servervariables.h"

ThreadedLoop::ThreadedLoop(
    const std::string& loop_name
) : loop_name(loop_name)
    {

    // Initialize in-memory database tags
    insertTag(dataSourceName+":servervariables/loops." + loop_name + ".target_loop_frequency.units", "Hz");
    insertTag(dataSourceName+":servervariables/loops." + loop_name + ".actual_loop_frequency.units", "Hz");
    insertTag(dataSourceName+":servervariables/loops." + loop_name + ".actual_loop_frequency.value", 0.0);

    //default to default target frequency
    insertTag(dataSourceName+":servervariables/loops." + loop_name + ".target_loop_frequency.value", DEFAULT_LOOP_TARGET_FREQUENCY);

    //start the thread
    //default to on
    insertTag(dataSourceName+":servervariables/loops." + loop_name + ".running.value", 1.0);

    int loop_status = queryTag<double>(dataSourceName+":servervariables/loops." + loop_name + ".running.value");
    if (loop_status == 1) {
        //start
        start();
    } //if it is 0, then no need to start the thread.
}

ThreadedLoop::ThreadedLoop() {
}

void ThreadedLoop::start() {
    //ensure running tag is set to true
    upsertTag(dataSourceName+":servervariables/loops." + loop_name + ".running.value", 1.0);
    upsertTag(dataSourceName+":servervariables/loops." + loop_name + ".running.units", "True/False");
    //set internal running flag to true
    running = true;

    worker_thread = std::thread(&ThreadedLoop::workerLoop, this);
}

void ThreadedLoop::stop() {
    if (worker_thread.joinable()) {
        //set internal running flag to false and join the thread
        running = false;
        worker_thread.join();

        //after loop is done:
        //ensure running tag is set to false
        upsertTag(dataSourceName+":servervariables/loops." + loop_name + ".running.value", 0.0);
        upsertTag(dataSourceName+":servervariables/loops." + loop_name + ".actual_loop_frequency.value", 0.0);
    }
}

void ThreadedLoop::setOnStart(const std::function<void(ThreadedLoop &)> &function) {
    start_function = function;
}

void ThreadedLoop::setOnLoop(const std::function<void(ThreadedLoop &)> &function) {
    loop_function = function;
}

void ThreadedLoop::setOnEnd(const std::function<void(ThreadedLoop &)> &function) {
    end_function = function;
}

void ThreadedLoop::addToLoop(const std::string added_function_name, const std::function<void(ThreadedLoop &)> &function) {
    added_functions[added_function_name] = function;
}

void ThreadedLoop::removeFromLoop(const std::string added_function_name) {
    added_functions.erase(added_function_name);
}

std::unordered_map<std::string, std::function<void(ThreadedLoop &)>> &ThreadedLoop::getAddedFunctions() {
    return this->added_functions;
}

ThreadedLoop::~ThreadedLoop() {
    if (queryTag<double>(dataSourceName+":servervariables/loops." + loop_name + ".running.value")) {
        stop();
    }
}

//loop that is run on another thread
void ThreadedLoop::workerLoop() {

    //invoke start function
    if (start_function) {
        start_function(*this);
    }

    //last call
    auto lastCall = std::chrono::high_resolution_clock::now();
    //while loop that calls the loop function
    while (running) {

        // Target frequency (in Hz)
        int targetFrequencyHz = queryTag<double>(dataSourceName + ":servervariables/loops." + loop_name + ".target_loop_frequency.value");
        std::chrono::microseconds targetPeriodUS(static_cast<int>(1000000.0 / targetFrequencyHz));

        /// ABOUT TO CALL FUNCTION ///
        // Calculate real-time frequency
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastCall).count();
        if (elapsedTime > 0) {
            upsertTag(dataSourceName + ":servervariables/loops." + loop_name + ".actual_loop_frequency.value", 1'000'000.0 / elapsedTime);
        }

        /// CALL THE FUNCTION ///
        auto callStart = std::chrono::high_resolution_clock::now();
        lastCall = callStart; // for frequency calculation
        if (loop_function) {
            loop_function(*this);

            //call any added loop functions
            if (!added_functions.empty()) {
                for (auto it = added_functions.begin(); it != added_functions.end(); it++) {
                    it->second(*this);
                }
            }

        }
        auto callEnd = std::chrono::high_resolution_clock::now();

        /// CALCULATE TIME REMAINING IN HZ INTERVAL ///
        auto callElapsed = std::chrono::duration_cast<std::chrono::microseconds>(callEnd - callStart); // Time taken by loop function
        auto remainingTime = targetPeriodUS - callElapsed; //chrono duration in microseconds

        /// SLEEP REMAINING TIME ///
        if (remainingTime > std::chrono::microseconds(0)) {
            std::this_thread::sleep_for(remainingTime);
        }
    }

    //once not running, invoke end function
    if (end_function) {
        //run end function
        end_function(*this);
    }
}

