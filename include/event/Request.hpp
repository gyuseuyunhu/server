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
    const std::string mVersion;
    std::map<std::string, std::string> mHeaders;
    std::string mContent; // 자료형 좀 더 고민

    void checkMethod(std::stringstream &ss);
    void checkPath(std::stringstream &ss);
    void checkHttpVersion(std::stringstream &ss);

    void parseStartLine(std::string &buffer, eRequestLine &requestLine);

    void storeHeaderMap(std::string buffer);

    void parseRequestHeader(std::string &buffer, eRequestLine &requestLine);
    void parseRequestContent(std::string &buffer);

  public:
    Request();
    ~Request();
    void parse(std::string &buffer, eRequestLine &requestLine);
};

#endif