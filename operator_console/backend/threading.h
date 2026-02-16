//
// Created by kemptonburton on 8/29/2025.
//

#ifndef THREADING_H
#define THREADING_H
#include <atomic>
#include <functional>
#include <string>
#include <thread>

class ThreadedLoop {
public:
    // Loop name
    std::string loop_name;
    //internal running flag
    std::atomic<bool> running;

    // Worker thread object
    std::thread worker_thread;

    // Constructor
    ThreadedLoop(const std::string& loop_name);

    ThreadedLoop();

    // Starts the worker loop in the worker thread
    void start();

    // Stops the worker thread and loop
    void stop();

    //sets the loop functions
    void setOnStart(const std::function<void(ThreadedLoop&)> &function);
    void setOnLoop(const std::function<void(ThreadedLoop&)> &function);
    void setOnEnd(const std::function<void(ThreadedLoop&)> &function);

    //adds a function call to the loop
    void addToLoop(const std::string added_function_name, const std::function<void(ThreadedLoop&)> &function);
    void removeFromLoop(const std::string added_function_name);

    //gets added functions
    std::unordered_map<std::string, std::function<void(ThreadedLoop &)>> &getAddedFunctions();

    // Destructor
    ~ThreadedLoop();

private:

    // Target frequency for the loop
    double initial_target_loop_frequency;

    //loop functions
    std::function<void(ThreadedLoop& threadedLoopObject)> start_function;
    std::function<void(ThreadedLoop& threadedLoopObject)> loop_function;
    std::function<void(ThreadedLoop& threadedLoopObject)> end_function;

    //added loop functions - to be called in main loop function
    std::unordered_map<std::string, std::function<void(ThreadedLoop &)>> added_functions;

    // The main loop executed on a separate thread
    void workerLoop();

};

#endif //THREADING_H
