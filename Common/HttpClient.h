/*
***************************************************************************************************
* 许可声明：
* 本软件（含源码、二进制）为开源软件，遵循MIT开源许可协议（若以下声明与MIT许可协议发送冲突，以本声明为准）。
* 任何个人或组织都可以不受限制的使用、修改该软件。
* 任何个人或组织都可以以源代码和二进制的形式重新分发、使用该软件。
* 任何个人或组织都可以不受限制地将该软件（或修改后的任意版本）用于商业、非商业的用途。
* 
* 免责声明：
* 作者不对使用、修改、分发（以及其他任何形式的使用）该软件（及其他修改后的任意版本）所产生的后果负有任何法律责任。
* 作者也不对该软件的安全性、稳定性做出任何保证。
* 
* 使用、修改、分发该软件时需要保留上述声明，且默认已经同意上述声明。
***************************************************************************************************
*/
#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_
#pragma once

#include <string>
#include <map>
#include <future>

namespace Http {
using Headers = std::multimap<std::string, std::string>;

// HTTP Request Datagram
class RequestDatagram {
   public:
    enum class METHOD {
        GET = 0,
        POST,
        HEAD,
        DEL,  // DELETE conflict with winnt.h DELETE macro.
        PUT,
        PATCH,
        OPTIONS,
        TRACE,
        CONNECT
    };
    RequestDatagram();
    RequestDatagram(const std::string& url, METHOD m);
    RequestDatagram(const RequestDatagram& that);
    RequestDatagram& operator=(const RequestDatagram& that);
    ~RequestDatagram();

    METHOD GetMethod() const;
    void SetMethod(METHOD m);

    Headers GetHeaders() const;
    void SetHeaders(const Headers& headers);

    std::string GetUrl() const;
    void SetUrl(const std::string& url);

    const void* GetBodyData() const;
    size_t GetBodySize() const;
    void SetBody(const void* data, size_t dataSize);

   private:
    METHOD method_;
    Headers headers_;
    void* bodyData_;
    size_t bodySize_;
    std::string url_;
};

// HTTP Response Datagram
class ResponseDatagram {
   public:
    ResponseDatagram();
    ResponseDatagram(const ResponseDatagram& that);
    ResponseDatagram& operator=(const ResponseDatagram& that);
    ~ResponseDatagram();

    int GetCode() const;
    void SetCode(int code);

    Headers GetHeaders() const;
    void AddHeader(const std::string& key, const std::string& value);

    void* GetBody() const;
    size_t GetBodySize() const;
    void SetBody(void* data, size_t dataSize);

    size_t GetBodyCapacity() const;
    void SetBodyCapacity(size_t s);

   private:
    int code_;
    Headers headers_;
    void* bodyData_;
    size_t bodySize_;
    size_t bodyCapacity_;
};

bool IsHttps(const std::string& url);

// Http Client
class Client {
   public:
    using RequestResult =
        std::function<void(int,                     // >=0 is CURLcode, <0 is custom error codes.
                           unsigned long,           // microseconds
                           const ResponseDatagram&  // HTTP response datagram
                           )>;
    // Custom error code
    enum : int { NO_CURL_HANDLE = -1,
                 USER_ABORT = -2 };

    Client();
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    virtual ~Client();

   public:
    static void Initialize();
    static void Uninitialize();

    class Cleanup {
       public:
        Cleanup();
        ~Cleanup();
    };

   public:
    // Issue an asynchronous HTTP request.
    // Return value:
    //   The return value does not indicate that the request is finished.
    //   Return false only when has another request is doing, other case will return true.
    //
    bool Request(std::shared_ptr<RequestDatagram> reqDatagram,
                 RequestResult ret,
                 bool allowRedirect = true,
                 int connectionTimeout = 5000,  // ms
                 int retry = 0);

    // Abort the current HTTP request.
    //
    void Abort();

    // Wait for the current HTTP request to be finished.
    // Return value:
    //   true  - request has finished or request finished with the special time(ms).
    //   false - timeout, the request did not finished with the special time(ms).
    //
    bool Wait(int ms);

    // Set HTTP proxy
    void SetProxy(const std::string& proxy);
    std::string GetProxy() const;

   private:
    int DoRequest(std::shared_ptr<RequestDatagram> reqDg,
                  ResponseDatagram& rspDg,
                  bool allowRedirect,
                  int connectionTimeout,
                  int retry);

   private:
    static bool initialized_;
    void* curl_;
    std::future<void> future_;
    std::atomic_bool abort_;
    std::string proxy_;
};
}  // namespace Http
#endif  // !HTTP_CLIENT_H_