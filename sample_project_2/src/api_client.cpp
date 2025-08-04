#include "api_client.hpp"
#include<curl/curl.h>
#include<iostream>
#include<string>
#include<mutex>

static std::mutex cout;

//callback to collect data from api
static size_t writeCallback(void* apicontent,size_t size,size_t nmemb, std::string* output){
    size_t totalsize = size * nmemb;
    //char pointer to array pointing to apicontent and append till totalsize to get whole output string.
    output->append(static_cast<char*>(apicontent), totalsize);
    return totalsize;
}

//fetch user data using curl
std::string ApiClient::fetchUserData(const std::string& url){
    CURL* curl = curl_easy_init();
    std::string readbuffer;

    if(!curl){
        std::lock_guard<std::mutex> lock(cout);
        std::cerr<<"curl init failed.\n";
        return "";
    }
    


    curl_easy_setopt(curl,CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readbuffer);
    curl_easy_setopt(curl,CURLOPT_TIMEOUT,10L);
    curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK){
        std::lock_guard<std::mutex> lock(cout);
        std::cerr<<"curl_easy_perform() failed"<<curl_easy_strerror(res)<<"\n";
        curl_easy_cleanup(curl);
        return "";
    }

    curl_easy_cleanup(curl);
    return readbuffer;
    
    


}

//post data to Api
bool ApiClient::postUserDataToApi(const std::string& url, const std::string& jsonPayload) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::lock_guard<std::mutex> lock(cout);
        std::cerr << "[POST] curl_easy_init() failed.\n";
        return false;
    }

    CURLcode res;
    struct curl_slist* headers = nullptr;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonPayload.length());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::lock_guard<std::mutex> lock(cout);
        std::cerr << "[POST] Failed: " << curl_easy_strerror(res) << '\n';
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return true;
}


