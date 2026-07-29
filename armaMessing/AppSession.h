#pragma once
#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include "NetworkManager.h"

class AppSession {
public:
    AppSession(NetworkManager& net, std::string refreshToken, std::string licenseId, std::string processId, int interval)
        : net_(net), refreshToken_(std::move(refreshToken)),
        licenseId_(std::move(licenseId)), processId_(std::move(processId)),
        interval_(interval) {}
    ~AppSession() { stop(); }

    void setOnSessionEnd(std::function<void(const std::string&)> cb) {

        onSessionEnd_ = std::move(cb);
    }
    void start() { running_ = true; thread_ = std::thread(&AppSession::run, this); }
    void stop() {

        running_ = false;

        if (thread_.joinable()) thread_.join();

        if (!processId_.empty()) { std::string e; net_.endSession(processId_, e); }
    }

private:
    void run();

    NetworkManager& net_;
    std::string refreshToken_, licenseId_, processId_;
    int interval_ = 60;
    std::atomic<bool> running_{ false };
    std::thread thread_;
    std::function<void(const std::string&)> onSessionEnd_;
};
