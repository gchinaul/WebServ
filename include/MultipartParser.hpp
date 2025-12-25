
#pragma once
#ifndef MULTIPARTPARSER_HPP
#define MULTIPARTPARSER_HPP

#include <string>
#include <vector>
#include <map>

struct MultipartPart {

    std::map<std::string, std::string> headers;
    std::string body;
    std::string name;
    std::string filename;

};

class MultipartParser {

    private:

        MultipartParser( const MultipartParser& obj );
        MultipartParser& operator=( const MultipartParser& obj );
    
    public:

        MultipartParser();
        ~MultipartParser();

        static std::vector<MultipartPart> parse( const std::string& body, const std::string& contentType );
        static std::vector<MultipartPart> parseFile( const std::string& filePath, const std::string& contentType );
        static void parseContentDisposition( const std::string &val, std::map<std::string,std::string> &out );
};

#endif
