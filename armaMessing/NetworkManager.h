#include <nlohmann/json.hpp>
#include <vector>



class NetworkManager {
public:

    struct ActivationResult {
        bool ok = false;
        std::string error;
    };

    struct HeartbeatResult {
        bool alive = false;   // should the session keep running?
        std::string code;     // VALID, SUSPENDED, EXPIRED, NO_MACHINE...
        std::string error;    // transport error (network)
    };


    bool endSession(const std::string& processId, std::string& errorOut);


    explicit NetworkManager(std::string baseUrl);

    // Returns true on success; on failure, fills errorOut with a message.

    bool isLoggedIn() const { return !sessionToken_.empty(); }
    const std::string& sessionToken() const { return sessionToken_; }
    const std::string& refreshToken() const { return refreshToken_; }

    HeartbeatResult heartbeat(const std::string& licenseId, const std::string& processId);   // was (licenseId, fingerprint)

    bool refresh(const std::string& refreshToken, std::string& errorOut);

    void setSessionToken(const std::string& t) { sessionToken_ = t; }

    static size_t writeVecCallback(char* ptr, size_t size, size_t nmemb, void* userdata);

private:
    struct Response {
        long status = 0;
        nlohmann::json body;
        std::string error;   // transport-level error (network, TLS, bad JSON)
    };



    Response request(const std::string& method, const std::string& path, const nlohmann::json* body = nullptr);

    static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata);

    std::string baseUrl_;
    std::string sessionToken_;
    std::string refreshToken_;
};
