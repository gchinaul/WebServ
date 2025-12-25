
#pragma once
#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <ctime>

#include "Config.hpp"
#include "CGI.hpp"
#include "ServerSocket.hpp"
#include "Responder.hpp"

#define CLIENT_TIMEOUT 60

struct Request {

    std::string method;
    std::string uri;
    std::string path;
    std::string query;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string bodyFile;
    bool chunked;
    size_t contentLength;

    Request():chunked(false),contentLength(0){}
};

class RequestParser {

    Request _req;

    std::string _buf;
    std::string _streamFilePath;

    std::ofstream *_streamFile;
    
    bool _streamToFile;
    bool _headersDone;
    bool _bodyDone;
    
    size_t _streamedBytes;

    public:

        RequestParser();
        ~RequestParser();

        bool feed( const char* data, size_t len, size_t maxBody, int &errCode );
        bool done() const;
        Request get() const;
        RequestParser( const RequestParser& obj );
        RequestParser& operator=( const RequestParser& obj );
        int ft_mkstemp(std::string &tmpl);

    private:

        void setHeader( const std::string &k, const std::string &v );
        
        bool parseStartLine( const std::string &line, int &errCode );
        bool parseHeaders( std::string &rest, int &errCode );
        bool parseBody( std::string &rest, size_t maxBody, int &errCode );
        bool decodeChunked( std::string &rest, int &errCode );
};

#endif