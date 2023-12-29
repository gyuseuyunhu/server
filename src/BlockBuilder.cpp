#include "BlockBuilder.hpp"
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

unsigned int BlockBuilder::convertNumber(const std::string &valueString, bool hasUnit)
{
    char *checkPtr;
    int value;

    value = std::strtod(valueString.c_str(), &checkPtr);
    if (value < 0)
    {
        throw std::runtime_error("Error : BlockBuilder::convertNumber() invalid max_body_size" + valueString);
    }
    else if (value == 0 && *checkPtr != '\0')
    {
        throw std::runtime_error("Error : BlockBuilder::convertNumber() invalid max_body_size" + valueString);
    }

    if (hasUnit == true)
    {
        switch (*checkPtr)
        {
        case 'k':
            /*fall through*/
        case 'K':
            value *= killo;
            break;
        case 'm':
            /*fall through*/
        case 'M':
            value *= mega;
            break;
        case 'g':
            /*fall through*/
        case 'G':
            value *= giga;
            break;
        case '\0':
            break;
        default:
            throw std::runtime_error("Error : BlockBuilder::convertNumber() invalid max_body_size" + valueString);
            break;
        }
        if (*checkPtr != '\0' && *(checkPtr + 1) != '\0')
        {
            throw std::runtime_error("Error : BlockBuilder::convertNumber() invalid max_body_size" + valueString);
        }
    }
    else
    {
        if (*checkPtr != '\0')
        {
            throw std::runtime_error(valueString + ": invalid Value");
        }
    }
    return static_cast<unsigned int>(value);
}

void BlockBuilder::parseConfig(const enum blockType blockType, const std::string &configString)
{
    std::istringstream iss(configString);
    std::string token;
    bool isFirstIndex = true;
    bool isFirstErrorPage = true;

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
            updateConfig(keyValue, subtoken, isFirstValue, isFirstIndex, isFirstErrorPage);
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
    }
}

void BlockBuilder::updateConfig(const std::string &key, const std::string &value, bool isFirstValue, bool &isFirstIndex,
                                bool &isFirstErrorPage)
{
    if (key == "root")
    {
        mRoot = value;
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
        if (isFirstErrorPage == true)
        {
            mErrorPages.clear();
            isFirstErrorPage = false;
        }
        mErrorPages.push_back(value);
    }
    else if (key == "client_max_body_size")
    {
        mClientMaxBodySize = convertNumber(value, true);
    }
    else if (key == "server_name")
    {
        mServerName = value;
    }
    else if (key == "return")
    {
        if (isFirstValue == true)
        {
            mRedirectionCode = convertNumber(value, false);
        }
        else
        {
            mRedirectionPath = value;
        }
    }
    else if (key == "listen")
    {
        mPort = convertNumber(value, false);
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
