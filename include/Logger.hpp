/*
 * RFC 5424 : The Syslog Protocol
 *
 * The goal of this architecture is to separate message content from message transport
 * while enabling easy extensibility for each layer
 *
 */

#pragma once
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Webserv.hpp"

namespace Logger
{
    void    info(const std::string& );
    void    error(const std::string& );
    void    warning(const std::string& );
    void    debug(const std::string& );
}

#endif
