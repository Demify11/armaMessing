#include "AppSession.h"

void AppSession::run() {
    using namespace std::chrono;

    std::string err;
    if (!net_.refresh(refreshToken_, err)) {   // first access token
        onSessionEnd_("auth");                  // can't even start -> stop
        return;
    }

    auto lastRefresh = steady_clock::now();
    const auto refreshEvery = minutes(45);      // well inside the 1h expiry
    int consecutiveErrors = 0;

    running_ = true;
    while (running_) {
        // Refresh the access token before it can expire.
        if (steady_clock::now() - lastRefresh >= refreshEvery) {
            if (!net_.refresh(refreshToken_, err)) {
                onSessionEnd_("auth");          // revoked or refresh expired -> session over
                break;
            }
            lastRefresh = steady_clock::now();
        }

        auto hr = net_.heartbeat(licenseId_, processId_);
        if (!hr.error.empty()) {
            if (++consecutiveErrors >= 3) { onSessionEnd_("network"); break; }
        }
        else {
            consecutiveErrors = 0;
            if (!hr.alive) { onSessionEnd_(hr.code); break; }   // suspended/expired/dead
        }

        const int periodSec = std::max(interval_ / 2, 15);
        for (int i = 0; i < periodSec && running_; ++i) std::this_thread::sleep_for(seconds(1));
    }
}