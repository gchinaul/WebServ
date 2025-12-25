/*
 * RFC 7230 : Hypertext Transfer Protocol (HTTP 1.1)
 * RFC 7231 : Semantics and Content
 *
 * The Each Hypertext Transfer Protocol (HTTP) message is either a request or a response. 
 * A server listens on a connection for a request, parses each message received, interprets 
 * the message semantics in relation to the identified request target, and responds 
 * to that request with one or more response messages.
 *
 */

#pragma once
#ifndef HTTP_HPP
#define HTTP_HPP

#include "Webserv.hpp"

struct HTTPStatus
{
    int             code;
    const char*     reason;
};

namespace HTTP
{
    const char*     reason(int code);

    std::string     dateNow();
    std::string     guessContentType(const std::string& path);
}

#endif
