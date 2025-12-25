
#pragma once
#ifndef RESPONDER_HPP
#define RESPONDER_HPP

#include <string>
#include <map>

struct ServerConf;
struct RouteConf;
struct Request;

struct Response {

    int                                 status;

    std::map<std::string, std::string>  headers;

    std::string                         body;
    std::string serialize() const;

    Response() : status(200) {}

};

class Responder {

    public:
    
        static Response makeError( const ServerConf &srv, int code );
        static Response sendFile( const ServerConf &srv, const std::string &fsPath );
        static Response listDir( const ServerConf &srv, const std::string &urlPath, const std::string &fsPath );
        static Response handleGet( const ServerConf &srv, const RouteConf &loc, const Request &req );
        static Response handlePost( const ServerConf &srv, const RouteConf &loc, const Request &req );
        static Response handleDelete( const ServerConf &srv, const RouteConf &loc, const Request &req );

        static std::map<std::string, std::string> parseFormData( const std::string &body );
};

#endif