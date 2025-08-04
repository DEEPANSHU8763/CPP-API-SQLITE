#pragma once
#include<string>


class ApiClient{
    public:
        std::string fetchUserData(const std::string& url);
        bool postUserDataToApi(const std::string& url, const std::string& jsonPayload);

};
