#ifndef HTTPENUM_HPP
#define HTTPENUM_HPP

enum eConnectionStatus
{
    KEEP_ALIVE,
    CONNECTION_CLOSE,
};

enum eHttpStatus
{
    OK = 200,
    BAD_REQUEST = 400,
    NOT_ALLOWED = 405,
    NOT_IMPLEMENT = 501,
    TEMPORARY_REDIRECT = 307,
    CONTENT_TOO_LARGE = 413,
};

#endif