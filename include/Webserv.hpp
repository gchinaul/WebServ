#pragma once
#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <poll.h>
#include <signal.h>
#include <cstring>
#include <string>
#include <ctime>
#include <map>
#include <set>
#include <cstddef>
#include <cerrno>
#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Message.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include "Time.hpp"
#include "ASocket.hpp"
#include "Config.hpp"
#include "LibftCpp.hpp"
#include "CGI.hpp"
#include "Responder.hpp"
#include "HTTP.hpp"
#include "ServerSocket.hpp"
#include "TCPConnection.hpp"

#endif