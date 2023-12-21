#include "BlockBuilder.hpp"
#include <iostream>
#include <sstream>

BlockBuilder::BlockBuilder()
    : mRoot("html"), mClientMaxBodySize(1000000), mPort(80), mServerName(""), mRedirectionCode(42), mIsAutoIndex(false),
      mIsAllowedGet(true), mIsAllowedPost(true), mIsAllowedDelete(true)
{
    mIndexs.push_back("index.html");
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
        throw std::runtime_error(std::to_string(value) + ": invalid max_body_size1");
    }
    else if (value == 0 && *checkPtr != '\0')
    {
        throw std::runtime_error(valueString + ": invalid max_body_size2");
    }

    if (hasUnit == true)
    {
        switch (*checkPtr)
        {
        case 'k':
            // 의도적인 fallthrough
        case 'K':
            value *= killo;
            break;
        case 'm':
            // 의도적인 fallthrough
        case 'M':
            value *= mega;
            break;
        case 'g':
            // 의도적인 fallthrough
        case 'G':
            value *= giga;
            break;
        case '\0':
            break;
        default:
            throw std::runtime_error(valueString + ": invalid max_body_size3");
            break;
        }
        if (*checkPtr != '\0' && *(checkPtr + 1) != '\0')
        {
            throw std::runtime_error(valueString + ": invalid max_body_size4");
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
                if (blockType == Http)
                {
                    mHttpDirective.checkDirective(subtoken);
                }
                else if (blockType == Server)
                {
                    mServerDirective.checkDirective(subtoken);
                }
                else if (blockType == Location)
                {
                    mLocationDirective.checkDirective(subtoken);
                }
                isKeyValue = false;
                keyValue = subtoken;
                continue;
            }
            updateConfig(keyValue, subtoken, isFirstValue);
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
    }
}

void BlockBuilder::updateConfig(const std::string &key, const std::string &value, bool isFirstValue)
{
    if (key == "root")
    {
        mRoot = value;
    }
    else if (key == "index")
    {
        if (isFirstValue == true)
        {
            mIndexs.clear();
        }
        mIndexs.push_back(value);
    }
    else if (key == "error_page")
    {
        if (isFirstValue == true)
        {
            mErrorPages.clear();
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

void BlockBuilder::resetServerBlockConfig()
{
    mPort = 80;
    mServerName = "";
    mRedirectionCode = 42;
    mRedirectionPath.clear();
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
