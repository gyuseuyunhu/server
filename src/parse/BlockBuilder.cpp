#include "BlockBuilder.hpp"
#include "HttpStatusInfos.hpp"
#include <climits>
#include <iostream>
#include <sstream>

const std::string BlockBuilder::DEFAULT_ROOT = "html";
const std::string BlockBuilder::DEFAULT_SERVER_NAME = "";

BlockBuilder::BlockBuilder()
    : mRoot(DEFAULT_ROOT), mClientMaxBodySize(E_DEFAULT_CLIENT_BODY_SIZE), mPort(E_DEFAULT_LISTEN_PORT),
      mServerName(DEFAULT_SERVER_NAME), mIsAutoIndex(false), mIsAllowedGet(true), mIsAllowedPost(true),
      mIsAllowedDelete(true), mCgiUploadDir("/")
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

bool BlockBuilder::tryConvertNumber(const std::string &valueString, bool hasUnit, unsigned int &result)
{
    char *checkPtr;
    unsigned long value;

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
        handleRoot(value);
    }
    else if (key == "index")
    {
        handleIndex(value, isFirstIndex);
    }
    else if (key == "error_page")
    {
        handleErrorPage(value, isMadeErrorPages);
    }
    else if (key == "client_max_body_size")
    {
        handleClientMaxBodySize(value);
    }
    else if (key == "server_name")
    {
        mServerName = value;
    }
    else if (key == "return")
    {
        mRedirectionPath = value;
    }
    else if (key == "listen")
    {
        handleListen(value);
    }
    else if (key == "path")
    {
        mLocationPath = value;
        if (mLocationPath[0] != '/')
        {
            mLocationPath = '/' + mLocationPath;
        }
    }
    else if (key == "autoindex")
    {
        handleAutoindex(value);
    }
    else if (key == "limit_except")
    {
        handleLimitExcept(value, isFirstValue, mIsMethodAllowed);
    }
    else if (key == "cgi_path")
    {
        mCgiPath = value;
    }
    else if (key == "cgi_extension")
    {
        mCgiExtension = value;
    }
    else if (key == "cgi_upload_dir")
    {
        mCgiUploadDir = value;
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
    mPort = E_DEFAULT_LISTEN_PORT;
    mServerName = DEFAULT_SERVER_NAME;
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
    mRedirectionPath = serverBlock.getRedirectionPath();
    mIsAutoIndex = false;
    mIsAllowedGet = true;
    mIsAllowedPost = true;
    mIsAllowedDelete = true;
    mCgiUploadDir = "/";
    mErrorCodes.clear();
}

void BlockBuilder::handleRoot(const std::string &value)
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

void BlockBuilder::handleIndex(const std::string &value, bool &isFirstIndex)
{
    if (isFirstIndex)
    {
        mIndexs.clear();
        isFirstIndex = false;
    }
    mIndexs.push_back(value);
}

void BlockBuilder::handleErrorPage(const std::string &value, bool &isMadeErrorPages)
{
    unsigned int errorCode;

    if (tryConvertNumber(value, false, errorCode) == true)
    {
        if (isMadeErrorPages == true)
        {
            throw std::runtime_error("Error: BlockBuilder::handleErrorPage() 에러페이지 뒤에 에러코드가 존재\n " +
                                     value);
        }
        mErrorCodes.push_back(errorCode);
    }
    else
    {
        if (mErrorCodes.empty())
        {
            throw std::runtime_error("Error: BlockBuilder::handleErrorPage() 에러코드 없음\n " + value);
        }

        if (isMadeErrorPages == false)
        {
            for (size_t i = 0; i < mErrorCodes.size(); ++i)
            {
                mErrorPages[mErrorCodes[i]] = value;
            }
            isMadeErrorPages = true;
            mErrorCodes.clear();
        }
        else
        {
            throw std::runtime_error("Error: BlockBuilder::handleErrorPage() 에러페이지 에러\n " + value);
        }
    }
}

void BlockBuilder::handleClientMaxBodySize(const std::string &value)
{
    if (tryConvertNumber(value, true, mClientMaxBodySize) == false)
    {
        throw std::runtime_error(
            "Error: BlockBuilder::handleClientMaxBodySize()) 적합하지 않은 clinet_max_body_size 숫자\n " + value);
    }
}

void BlockBuilder::handleListen(const std::string &value)
{
    if (tryConvertNumber(value, false, mPort) == false)
    {
        throw std::runtime_error("Error: BlockBuilder::handleListen() 적합하지 않은 숫자\n " + value);
    }
}

void BlockBuilder::handleAutoindex(const std::string &value)
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
        throw std::runtime_error("Error  BlockBuilder::handleAutoindex() invalid value in \"autoindex\"\n" + value);
    }
}

void BlockBuilder::handleLimitExcept(const std::string &value, bool isFirstValue, bool &isMethodAllowed)
{
    if (isFirstValue)
    {
        if (value == "allow")
        {
            isMethodAllowed = true;
        }
        else if (value == "deny")
        {
            isMethodAllowed = false;
        }
        else
        {
            throw std::runtime_error("Error: BlockBuilder::handleLimitExcept() invalid access in \"limit_except\"\n" +
                                     value);
        }
    }
    else
    {
        if (value == "GET")
        {
            mIsAllowedGet = isMethodAllowed;
        }
        else if (value == "POST")
        {
            mIsAllowedPost = isMethodAllowed;
        }
        else if (value == "DELETE")
        {
            mIsAllowedDelete = isMethodAllowed;
        }
        else
        {
            throw std::runtime_error("Error: BlockBuilder::handleLimitExcept() invalid method in \"limit_except\"\n" +
                                     value);
        }
    }
}

HttpBlock BlockBuilder::buildHttpBlock() const
{
    return HttpBlock(mRoot, mIndexs, mErrorPages, mClientMaxBodySize);
}

ServerBlock BlockBuilder::buildServerBlock() const
{
    return ServerBlock(buildHttpBlock(), mPort, mServerName, mRedirectionPath);
}

LocationBlock BlockBuilder::buildLocationBlock() const
{
    return LocationBlock(buildServerBlock(), mLocationPath, mIsAutoIndex, mIsAllowedGet, mIsAllowedPost,
                         mIsAllowedDelete, mCgiPath, mCgiExtension, mCgiUploadDir);
}
