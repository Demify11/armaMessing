
#include "NetworkManager.h"
#include <curl/curl.h>

NetworkManager::NetworkManager(std::string baseUrl) 
    : baseUrl_(std::move(baseUrl)) {}


size_t NetworkManager::writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* buf = static_cast<std::string*>(userdata);
    buf->append(ptr, size * nmemb);
    return size * nmemb;
}

NetworkManager::Response NetworkManager::request(const std::string& method,
    const std::string& path,
    const nlohmann::json* body) {
    Response result;
    CURL* curl = curl_easy_init();
    if (!curl) { result.error = "failed to init curl"; return result; }

    const std::string url = baseUrl_ + path;
    const std::string requestBody = body ? body->dump() : std::string();
    std::string responseBody;

    curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    if (body) headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string authHeader;
    if (!sessionToken_.empty()) {
        authHeader = "Authorization: Bearer " + sessionToken_;
        headers = curl_slist_append(headers, authHeader.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    // TLS verification stays ON by default — do not disable it.

    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(requestBody.size()));
    }
    else if (method != "GET") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        if (body) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(requestBody.size()));
        }
    }
    // GET needs nothing extra.

    const CURLcode rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
        result.error = curl_easy_strerror(rc);
    }
    else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result.status);
        if (!responseBody.empty()) {
            try {
                result.body = nlohmann::json::parse(responseBody);
            }
            catch (const std::exception& e) {
                result.error = std::string("bad json: ") + e.what();
            }
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return result;
}


NetworkManager::HeartbeatResult
NetworkManager::heartbeat(const std::string& licenseId, const std::string& processId) {
    HeartbeatResult hr;
    const nlohmann::json body = { {"licenseId", licenseId}, {"processId", processId} };
    Response res = request("POST", "/heartbeat", &body);
    if (!res.error.empty()) { hr.error = res.error; return hr; }
    if (res.status == 200) {
        hr.alive = res.body.value("alive", false);
        if (res.body.contains("code") && res.body["code"].is_string())
            hr.code = res.body["code"].get<std::string>();
        return hr;
    }
    hr.error = res.body.contains("error") ? res.body["error"].get<std::string>()
        : "heartbeat failed (status " + std::to_string(res.status) + ")";
    return hr;
}


bool NetworkManager::endSession(const std::string& processId, std::string& errorOut) {
    const nlohmann::json body = { {"processId", processId} };
    Response res = request("POST", "/kill", &body);
    if (!res.error.empty()) { errorOut = res.error; return false; }
    if (res.status == 200 && res.body.value("ok", false)) return true;
    errorOut = res.body.contains("error") ? res.body["error"].get<std::string>()
        : "kill failed (status " + std::to_string(res.status) + ")";
    return false;
}

size_t NetworkManager::writeVecCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* buf = static_cast<std::vector<unsigned char>*>(userdata);
    auto* p = reinterpret_cast<unsigned char*>(ptr);
    buf->insert(buf->end(), p, p + size * nmemb);
    return size * nmemb;
}


bool NetworkManager::refresh(const std::string& refreshToken, std::string& errorOut) {
    const nlohmann::json body = { {"refreshToken", refreshToken} };
    Response res = request("POST", "/refresh", &body);
    if (!res.error.empty()) { errorOut = res.error; return false; }
    if (res.status == 200 && res.body.contains("token")) {
        sessionToken_ = res.body["token"].get<std::string>();  // adopt the fresh access token
        return true;
    }
    errorOut = res.body.contains("error") ? res.body["error"].get<std::string>()
        : "refresh failed (status " + std::to_string(res.status) + ")";
    return false;
}
