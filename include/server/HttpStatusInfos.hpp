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
    static std::map<std::string, std::string> mMimeType;
    static std::string mWebservRoot;
    static std::vector<std::string> mEnvp;
    static void initHttpStatusReasons();
    static void initHttpErrorPages();
    static void initMimeType();
    static void setWebservRoot(char **envp);
    static void setEnvp(char **envp);
    HttpStatusInfos();

  public:
    static void initHttpStatusInfos(char **envp);
    static const std::string &getHttpReason(const int statusCode);
    static const std::string &getHttpErrorPage(const int statusCode);
    static const std::string &getWebservRoot();
    static const std::string &getMimeType(const std::string &type);
    static const std::string makeAutoIndexPage(const std::string &path);
    static void addEnv(const std::string &env);
    static char **allocateNewEnvp();
};

#endif