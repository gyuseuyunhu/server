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
    MOVED_PERMANENTLY = 301,
    TEMPORARY_REDIRECT = 307,
    BAD_REQUEST = 400,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    NOT_ALLOWED = 405,
    CONTENT_TOO_LARGE = 413,
    NOT_IMPLEMENT = 501,
};

#endif