#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>

enum eHttpMethod
{
    E_GET,
    E_POST,
    E_DELETE,
};

enum eRequestLine
{
    E_START_LINE,
    E_REQUEST_HEADER,
    E_REQUEST_CONTENTS
};

class Request
{
  private:
    eHttpMethod mMethod;
    std::string mPath;
    std::string mVersion;
    std::map<std::string, std::string> mHeaders;
    std::string mContent; // 자료형 좀 더 고민

    eRequestLine mRequestLine;
    int mStatus;

    void checkMethod(std::stringstream &ss);
    void checkPath(std::stringstream &ss);
    void checkHttpVersion(std::stringstream &ss);

    void parseStartLine(std::string &buffer);

    void storeHeaderMap(std::string buffer);

    void parseRequestHeader(std::string &buffer);
    void parseRequestContent(std::string &buffer);

  public:
    Request();
    ~Request();
    void parse(std::string &buffer);

    int getStatus() const;
    const std::map<std::string, std::string> &getHeaders() const;
    const std::string &getPath() const;
    void clear();
};

#endif