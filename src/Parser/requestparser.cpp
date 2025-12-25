
#include "../include/Server.hpp"
#include "../include/LibftCpp.hpp"
#include "../include/Logger.hpp"
#include "../include/RequestParser.hpp"
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <cerrno>

RequestParser::RequestParser() : _headersDone(false), _bodyDone(false) {

    _req.chunked = false;
    _req.contentLength = 0;
    _streamToFile = false;
    _streamFile = NULL;
    _streamedBytes = 0;
    _streamFilePath = "";
}

RequestParser::~RequestParser() {}

RequestParser::RequestParser( const RequestParser& obj ) {

    _buf = obj._buf;
    _headersDone = obj._headersDone;
    _bodyDone = obj._bodyDone;
    _req = obj._req;
    _streamToFile = obj._streamToFile;
    _streamFilePath = obj._streamFilePath;
    _streamedBytes = obj._streamedBytes;
    _streamFile = obj._streamFile;
}
RequestParser& RequestParser::operator=( const RequestParser& obj ) {

    _buf = obj._buf;
    _headersDone = obj._headersDone;
    _bodyDone = obj._bodyDone;
    _req = obj._req;
    _streamToFile = obj._streamToFile;
    _streamFilePath = obj._streamFilePath;
    _streamedBytes = obj._streamedBytes;
    _streamFile = obj._streamFile;
    
    return ( *this );
}
