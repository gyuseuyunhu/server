#include "Config.hpp"

Config *Config::mInstance = NULL; // Definition of mInstance

// clang-format off
//Config::Config(HttpBlock httpBlock, std::vector<std::pair<ServerBlock, std::vector<LocationBlock> > > mServerBlockGroups)
//{
//	
//}
// clang-format on

// clang-format off
void Config::createInstance(std::string httpString)
                                 //std::vector<std::pair<std::string, std::vector<std::string> > > mServerStr)
// clang-format on
{
    HttpBlock httpBlock = createHttpBlock(httpString);
    std::cout << httpBlock;
    // mInstance = new Config(httpBlock);
}

Config &Config::getInstance()
{
    if (mInstance == NULL)
    {
        throw std::runtime_error("Before calling getInstance, it is necessary to createInstance first.");
    }
    return *mInstance;
}

void Config::deleteInstance()
{
    if (mInstance == NULL)
    {
        throw std::runtime_error("Before calling deleteInstance, it is necessary to createInstance first.");
    }
    delete mInstance;
    mInstance = NULL;
}

std::string Config::reduceMultipleSpaces(std::string token)
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

unsigned int Config::convertBodySize(std::string &valueString)
{
    char *checkPtr;
    int value;

    value = std::strtod(valueString.c_str(), &checkPtr);
    if (value < 0)
    {
        throw std::runtime_error(std::to_string(value) + ": invalid max_body_size1");
    }
    else if (value == 0 && checkPtr != '\0')
    {
        throw std::runtime_error(valueString + ": invalid max_body_size2");
    }

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
    return static_cast<unsigned int>(value);
}

HttpBlock Config::createHttpBlock(std::string httpStr)
{
    // root 기본값 html, index index.html, errorpage ""(없음), client_max_body_size = 1m;
    std::istringstream iss(httpStr);
    std::string token;
    std::string root = "html";
    std::vector<std::string> indexs;
    std::vector<std::string> errorPages;
    unsigned int clientMaxBodySize = 1000000;
    HttpDirective h;

    while (std::getline(iss, token, ';'))
    {
        std::istringstream tokenStream(reduceMultipleSpaces(token));
        std::string subtoken;
        std::string keyValue;
        bool isKeyValue = true;
        bool isValue = false;
        while (tokenStream >> subtoken)
        {
            if (isKeyValue == true)
            {
                h.checkDirective(subtoken);
                isKeyValue = false;
                keyValue = subtoken;
                continue;
            }

            if (keyValue == "root")
            {
                root = subtoken;
            }
            else if (keyValue == "index")
            {
                indexs.push_back(subtoken);
            }
            else if (keyValue == "error_page")
            {
                errorPages.push_back(subtoken);
            }
            else
            {
                clientMaxBodySize = convertBodySize(subtoken);
            }
            isValue = true;
        }
        if (tokenStream.fail())
        {
            if (isValue == false)
            {
                throw std::runtime_error("no value");
            }
        }
    }
    // indexs 값이 안 들어왔을때 기본값
    if (indexs.size() == 0)
    {
        indexs.push_back("index.html");
    }

    return HttpBlock(root, indexs, errorPages, clientMaxBodySize);
}