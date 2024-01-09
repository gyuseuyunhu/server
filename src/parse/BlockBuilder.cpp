#include "BlockBuilder.hpp"
#include "HttpStatusInfos.hpp"
#include <climits>
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
    unsigned long value;

    // 음수인 경우
    if (valueString.c_str()[0] == '-')
    {
        return false;
    }

    value = std::strtoul(valueString.c_str(), &checkPtr, 10);
    if (hasUnit == false && *checkPtr != '\0')
    {
        return false;
    }
    if (value == ULONG_MAX)
    {
        return false;
    }

    if (hasUnit)
    {
        switch (*checkPtr)
        {
        case 'k':
            /*fall through*/
        case 'K':
            if (value > E_CLIENT_MAX_BODY_SIZE / E_KILLO)
            {
                return false;
            }
            value *= E_KILLO;
            break;
        case 'm':
            /*fall through*/
        case 'M':
            if (value > E_CLIENT_MAX_BODY_SIZE / E_MEGA)
            {
                return false;
            }
            value *= E_MEGA;
            break;
        case 'g':
            /*fall through*/
        case 'G':
            if (value > E_CLIENT_MAX_BODY_SIZE / E_GIGA)
            {
                return false;
            }
            value *= E_GIGA;
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

    if (value > E_CLIENT_MAX_BODY_SIZE)
    {
        return false;
    }

    result = static_cast<unsigned int>(value);
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
        mRoot = value;
        if (mRoot[0] != '/')
        {
            mRoot = "/" + mRoot;
        }
        if (mRoot[mRoot.size() - 1] != '/')
        {
            mRoot = mRoot + "/";
        }
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
                    mErrorPages[mErrorCodes[i]] = value;
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
            throw std::runtime_error(
                "Error: BlockBuilder::tryConvertNumber() 적합하지 않은 clinet_max_body_size 숫자\n " + value);
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
