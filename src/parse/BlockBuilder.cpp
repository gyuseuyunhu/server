#include "BlockBuilder.hpp"
#include "HttpStatusInfos.hpp"
#include <iostream>
#include <sstream>

BlockBuilder::BlockBuilder()
    : mRoot("html"), mClientMaxBodySize(1000000), mPort(80), mServerName(""), mRedirectionCode(200),
      mIsAutoIndex(false), mIsAllowedGet(true), mIsAllowedPost(true), mIsAllowedDelete(true)
{
}

std::string BlockBuilder::reduceMultipleSpaces(std::string token)
{
    std::string cleanedToken;
    bool previousWasSpace = false;
    char c;
    for (std::size_t i = 0; i < token.size(); ++i)
    {
        c = token[i];
        if (c != ' ' && c != '\t')
        {
            cleanedToken += c;
            previousWasSpace = false;
        }
        else
        {
            if (!previousWasSpace)
            {
                cleanedToken += ' ';
            }
            previousWasSpace = true;
        }
    }
    return cleanedToken;
}

// 함수 만드는 방법
bool BlockBuilder::tryConvertNumber(const std::string &valueString, bool hasUnit, unsigned int &result)
{
    char *checkPtr;
    int value;

    value = std::strtod(valueString.c_str(), &checkPtr);
    if (value < 0)
    {
        return false;
    }
    else if (value == 0 && *checkPtr != '\0')
    {
        return false;
    }

    result = value;
    if (hasUnit == true)
    {
        switch (*checkPtr)
        {
        case 'k':
            /*fall through*/
        case 'K':
            result *= E_KILLO;
            break;
        case 'm':
            /*fall through*/
        case 'M':
            result *= E_MEGA;
            break;
        case 'g':
            /*fall through*/
        case 'G':
            result *= E_GIGA;
            break;
        case '\0':
            break;
        default:
            return false;
            break;
        }
        if (*checkPtr != '\0' && *(checkPtr + 1) != '\0')
        {
            return false;
        }
    }
    else
    {
        if (*checkPtr != '\0')
        {
            return false;
        }
    }
    return true;
}

void BlockBuilder::parseConfig(const enum blockType blockType, const std::string &configString)
{
    std::istringstream iss(configString);
    std::string token;
    bool isFirstIndex = true;

    mServerDirective.initDirectiveMap();
    mLocationDirective.initDirectiveMap();

    while (std::getline(iss, token, ';'))
    {
        std::istringstream tokenStream(reduceMultipleSpaces(token));
        std::string subtoken;
        std::string keyValue;
        bool isKeyValue = true;
        bool isValue = false;
        bool isFirstValue = true;
        bool isMadeErrorPages = false;
        while (tokenStream >> subtoken)
        {
            if (isKeyValue == true)
            {
                if (blockType == E_HTTP)
                {
                    mHttpDirective.checkDirective(subtoken);
                }
                else if (blockType == E_SERVER)
                {
                    mServerDirective.checkDirective(subtoken);
                }
                else if (blockType == E_LOCATION)
                {
                    mLocationDirective.checkDirective(subtoken);
                }
                isKeyValue = false;
                keyValue = subtoken;
                continue;
            }
            updateConfig(keyValue, subtoken, isFirstValue, isFirstIndex, isMadeErrorPages);
            isValue = true;
            isFirstValue = false;
        }
        if (tokenStream.fail())
        {
            if (isValue == false)
            {
                throw std::runtime_error("Error: BlockBuilder::parseConfig() no value\n" + subtoken);
            }
        }
        if (mIndexs.size() == 0)
        {
            mIndexs.push_back("index.html");
        }
        if (keyValue == "error_page" && isMadeErrorPages == false)
        {
            throw std::runtime_error("Error: BlockBuilder::parseConfig() 에러페이지 없음\n" + subtoken);
        }
    }
}

void BlockBuilder::updateConfig(const std::string &key, const std::string &value, bool isFirstValue, bool &isFirstIndex,
                                bool &isMadeErrorPages)
{
    if (key == "root")
    {
        mRoot = "/" + value + "/";
    }
    else if (key == "index")
    {
        if (isFirstIndex == true)
        {
            mIndexs.clear();
            isFirstIndex = false;
        }
        mIndexs.push_back(value);
    }
    else if (key == "error_page")
    {
        unsigned int errorCode;

        if (tryConvertNumber(value, false, errorCode) == true)
        {
            mErrorCodes.push_back(errorCode);
        }
        else
        {
            if (mErrorCodes.size() == 0)
            {
                throw std::runtime_error("Error: BlockBuilder::updateConfig() 에러코드 없음\n " + value);
            }
            if (isMadeErrorPages == false)
            {
                for (size_t i = 0; i < mErrorCodes.size(); ++i)
                {
                    mErrorPages[mErrorCodes[i]] = HttpStatusInfos::getWebservRoot() + value;
                }
                isMadeErrorPages = true;
                mErrorCodes.clear();
                return;
            }
            throw std::runtime_error("Error: BlockBuilder::updateConfig() 에러페이지 에러\n " + value);
        }
    }
    else if (key == "client_max_body_size")
    {
        if (tryConvertNumber(value, true, mClientMaxBodySize) == false)
        {
            throw std::runtime_error("Error: BlockBuilder::tryConvertNumber() 적합하지 않은 숫자\n " + value);
        }
    }
    else if (key == "server_name")
    {
        mServerName = value;
    }
    else if (key == "return")
    {
        if (isFirstValue == true)
        {
            if (tryConvertNumber(value, false, mRedirectionCode) == false)
            {
                throw std::runtime_error("Error: BlockBuilder::tryConvertNumber() 적합하지 않은 숫자\n " + value);
            }
        }
        else
        {
            mRedirectionPath = value;
        }
    }
    else if (key == "listen")
    {
        if (tryConvertNumber(value, false, mPort) == false)
        {
            throw std::runtime_error("Error: BlockBuilder::tryConvertNumber() 적합하지 않은 숫자\n " + value);
        }
    }
    else if (key == "path")
    {
        mLocationPath = value;
    }
    else if (key == "autoindex")
    {
        if (value == "on")
        {
            mIsAutoIndex = true;
        }
        else if (value == "off")
        {
            mIsAutoIndex = false;
        }
        else
        {
            throw std::runtime_error("Error BlockBuilder::updateConfig() : invalid value in \"autoindex\"\n" + value);
        }
    }
    else if (key == "limit_except")
    {
        if (isFirstValue == true)
        {
            if (value == "allow")
            {
                mIsMethodAllowed = true;
            }
            else if (value == "deny")
            {
                mIsMethodAllowed = false;
            }
            else
            {
                throw std::runtime_error("Error BlockBuilder::updateConfig() : invalid access in \"limit_except\"\n" +
                                         value);
            }
        }
        else
        {
            if (value == "GET")
            {
                if (mIsMethodAllowed == true)
                {
                    mIsAllowedGet = true;
                }
                else
                {
                    mIsAllowedGet = false;
                }
            }
            else if (value == "POST")
            {
                if (mIsMethodAllowed == true)
                {
                    mIsAllowedPost = true;
                }
                else
                {
                    mIsAllowedPost = false;
                }
            }
            else if (value == "DELETE")
            {
                if (mIsMethodAllowed == true)
                {
                    mIsAllowedDelete = true;
                }
                else
                {
                    mIsAllowedDelete = false;
                }
            }
            else
            {
                throw std::runtime_error("Error BlockBuilder::updateConfig() : invalid method in \"limit_except\"\n" +
                                         value);
            }
        }
    }
    else
    {
        assert(false);
    }
}

void BlockBuilder::resetServerBlockConfig(const HttpBlock &httpBlock)
{
    mRoot = httpBlock.getRoot();
    mIndexs = httpBlock.getIndexs();
    mErrorPages = httpBlock.getErrorPages();
    mClientMaxBodySize = httpBlock.getClientMaxBodySize();
    mPort = 80;
    mServerName = "";
    mRedirectionCode = 200;
    mRedirectionPath.clear();
    mErrorCodes.clear();
}

void BlockBuilder::resetLocationBlockConfig(const ServerBlock &serverBlock)
{
    mRoot = serverBlock.getRoot();
    mIndexs = serverBlock.getIndexs();
    mErrorPages = serverBlock.getErrorPages();
    mClientMaxBodySize = serverBlock.getClientMaxBodySize();
    mPort = serverBlock.getPort();
    mServerName = serverBlock.getServerName();
    mRedirectionCode = serverBlock.getRedirectionCode();
    mRedirectionPath = serverBlock.getRedirectionPath();
    mIsAutoIndex = false;
    mIsAllowedGet = true;
    mIsAllowedPost = true;
    mIsAllowedDelete = true;
    mErrorCodes.clear();
}

HttpBlock BlockBuilder::buildHttpBlock() const
{
    return HttpBlock(mRoot, mIndexs, mErrorPages, mClientMaxBodySize);
}

ServerBlock BlockBuilder::buildServerBlock() const
{
    return ServerBlock(buildHttpBlock(), mPort, mServerName, mRedirectionCode, mRedirectionPath);
}

LocationBlock BlockBuilder::buildLocationBlock() const
{
    return LocationBlock(buildServerBlock(), mLocationPath, mIsAutoIndex, mIsAllowedGet, mIsAllowedPost,
                         mIsAllowedDelete);
}
