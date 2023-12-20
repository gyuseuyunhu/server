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

unsigned int Config::convertNumber(std::string &valueString, bool hasUnit)
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
                clientMaxBodySize = convertNumber(subtoken, true);
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

// LocationBlock Config::createLocationBlock(ServerBlock serverBlock, std::string locationStr)
// {
//     LocationDirective locationDirective;
//     assert(locationStr.size() != 0);

//     // 서버 블록 만들기 인데 httpBlock이 필요하겠네?
//     std::istringstream iss(locationStr);
//     std::string mLocationPath;
//     bool mIsAutoIndex = false;
//     bool mIsAllowedGet = true;
//     bool mIsAllowedPost = true;
//     bool mIsAllowedDelete = true;
//     std::string token;

//     // /hi;
//     while (std::getline(iss, token, ';'))
//     {
//         std::istringstream tokenStream(reduceMultipleSpaces(token));
//         std::string subtoken;
//         std::string keyValue;
//         bool isKeyValue = true;
//         bool isValue = false;
//         while (tokenStream >> subtoken)
//         {
//             if (isKeyValue == true)
//             {
//                 locationDirective.checkDirective(subtoken);
//                 isKeyValue = false;
//                 keyValue = subtoken;
//                 continue;
//             }

//             if (keyValue == "server_name")
//             {
//                 serverName = subtoken;
//             }
//             else if (keyValue == "return")
//             {
//                 if (isRedirectionCode == true)
//                 {
//                     redirectionCode = convertNumber(subtoken, false);
//                     isRedirectionCode = false;
//                 }
//                 else
//                 {
//                     redirectionPath = subtoken;
//                 }
//             }
//             else
//             {
//                 port = convertNumber(subtoken, false);
//             }
//             isValue = true;
//         }
//         if (tokenStream.fail())
//         {
//             if (isValue == false)
//             {
//                 throw std::runtime_error("no value");
//             }
//         }
//     }
// }

ServerWithLocations Config::createServerWithLocations(HttpBlock httpBlock,
                                                      std::pair<std::string, LocationVec> serverLocPair)
{
    std::string serverString;
    ServerDirective serverDirective;
    serverString = serverLocPair.first;
    assert(serverString.size() != 0);

    // 서버 블록 만들기 인데 httpBlock이 필요하겠네?
    std::istringstream iss(serverString);
    unsigned int port = 80;
    std::string serverName = "";
    unsigned int redirectionCode;
    std::string redirectionPath;
    std::string token;

    while (std::getline(iss, token, ';'))
    {
        std::istringstream tokenStream(reduceMultipleSpaces(token));
        std::string subtoken;
        std::string keyValue;
        bool isKeyValue = true;
        bool isValue = false;
        bool isRedirectionCode = true;
        // return 301 /hi2
        // subtoken return
        // tokenStream 301 /hi2
        while (tokenStream >> subtoken)
        {
            if (isKeyValue == true)
            {
                serverDirective.checkDirective(subtoken);
                isKeyValue = false;
                keyValue = subtoken;
                continue;
            }

            if (keyValue == "server_name")
            {
                serverName = subtoken;
            }
            else if (keyValue == "return")
            {
                if (isRedirectionCode == true)
                {
                    redirectionCode = convertNumber(subtoken, false);
                    isRedirectionCode = false;
                }
                else
                {
                    redirectionPath = subtoken;
                }
            }
            else
            {
                port = convertNumber(subtoken, false);
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

    ServerBlock serverBlock(httpBlock, port, serverName, redirectionCode, redirectionPath);

    std::vector<LocationBlock> locations;
    std::vector<std::string> locationStrs = serverLocPair.second;
    std::vector<std::string>::iterator it = locationStrs.begin();

    // location 돌면서 다 만들기. 함수로 빼도 가능
    for (; it != locationStrs.end(); ++it)
    {
        // 함수가 돌아버린다
        // 반환값이 locationBlock
        // 이걸 locations에 pushback
    }
    std::pair<ServerBlock, LocationList> tmp = std::make_pair(serverBlock, locations);

    return tmp;
}
