#ifndef HTTPSTATUSINFOS_HPP
#define HTTPSTATUSINFOS_HPP

#define CRLF "\r\n"
#include <map>
#include <string>

class HttpStatusInfos
{
  private:
    static std::map<int, std::string> mHttpStatusReasons;
    static std::map<int, std::string> mHttpErrorPages;
    static std::string mWebservRoot;
    HttpStatusInfos();

  public:
    void initHttpStatusInfos(char **envp);
    void initHttpStatusReasons();
    void initHttpErrorPages();
    void setWebservRoot(char **envp);
    const std::string &getHttpReason(const int statusCode);
    const std::string &getHttpErrorPage(const int statusCode);
    const std::string &getWebservRoot();
};

#endif