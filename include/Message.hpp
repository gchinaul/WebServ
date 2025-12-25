
#pragma once
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#define OK 0
#define ERROR 1

const bool GOOD_BOOL = true;
const bool ERROR_BOOL = false;

#define ERROR_ARG 1
#define ERROR_INIT 1
#define ERROR_SOCKET "can not create socket"
#define ERROR_BIND "error with the bind"
#define ERROR_LISTEN "in listen..."
#define ERROR_ACCEPT "in accept..."
#define ERROR_CONFIG "it's error from _cfg"
#define ERROR_SELECT "the select() failed"
#define ERROR_CGI_READ "read() error form the CGI pipe"
#define ERROR_ON_OPEN_FILE "could not open file: "
#define ERROR_MISSING_PATH "missing path"

#define INIT_FAIL "init are failded..."

#define READY "the webserv is.... ready !"
#define LISTENING_ON "it's actually listening "

#endif