#include "Config.hpp"

Directive Config::mDirective;     // Definition of mDirective
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

HttpBlock Config::createHttpBlock(std::string httpStr)
{
    std::istringstream iss(httpStr);
    std::string token;
    std::string root;
    std::vector<std::string> indexs;
    std::vector<std::string> errorPages;
    unsigned int clientMaxBodySize;
    while (std::getline(iss, token, ';'))
    {
        std::istringstream tokenStream(reduceMultipleSpaces(token));
        std::string subtoken;
        std::string keyValue;
        bool isKeyValue = true;
        bool isValue = false;
        // 빈칸 처리가 되는듯?
        while (tokenStream >> subtoken)
        {
            if (isKeyValue == true && mDirective.checkHttpStr(subtoken))
            {
                isKeyValue = false;
                keyValue = subtoken;
                continue;
            }

            if (keyValue == "root")
            {
                root = subtoken;
                isValue = true;
            }
            else if (keyValue == "index")
            {
                indexs.push_back(subtoken);
                isValue = true;
            }
            else if (keyValue == "error_page")
            {
                errorPages.push_back(subtoken);
                isValue = true;
            }
            else
            {
                // 이거 생각할 필요가 있음
                clientMaxBodySize = atoi(subtoken.c_str());
                isValue = true;
            }
        }
        if (tokenStream.fail())
        {
            if (isValue == false)
            {
                throw std::runtime_error("no value");
            }
        }
    }
    return HttpBlock(root, indexs, errorPages, clientMaxBodySize);
}