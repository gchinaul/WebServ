/*
 * The RFC 7230 if used for message syntex and routing
 * RFC 7231 if used for semantics and content
 *
 * For Parsing if fundamental too understand the Internet Standards.
 *
 */

#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include "Webserv.hpp"

#define MAP_STR_STR std::map<std::string, std::string>
#define MAP_INT_STR std::map<int, std::string>

class ConfigParser
{
    private:

        ConfigParser();
        ConfigParser(const ConfigParser& obj);
        ConfigParser& operator=(const ConfigParser& obj);

        ServerConf      _config;

        RouteConf*      findOrCreateRoute(const std::string& path);

        std::string     _filePath;
        std::string     normalizeValue(std::string& s);

        std::vector<ServerConf> _configs;
        std::vector<std::pair<std::string, int> > _additionalListens;
        bool            _hasError;

        MAP_STR_STR     _configMap;
        MAP_INT_STR     _errorPages;

        void            parseLocations();
        void            trimLeadingSpaces(std::string& str);
        void            trimTrailingSpaces(std::string& str);
        void            trimTrailingComment(std::string& str);
        void            parseServerBlock(std::ifstream& file);
        void            parseLine(std::string& line, MAP_STR_STR& targetMap);
        void            parseBlock(std::ifstream& file, const std::string& firstLine, MAP_STR_STR& targetMap);

    public:

        ConfigParser(const std::string& filePath);
        ~ConfigParser();

        ServerConf      getConfig() const;

        std::string     getHost() const;
        std::string     getServerName() const;
        std::string     getErrorPages() const;
        std::vector<ServerConf> getConfigs() const;
        bool            hasError() const;

        size_t          getClientMaxBodySize() const;
        size_t          parseSize(const std::string& s, size_t def);

        void            setConfig();
        void            parseConfigFile();
        void            setHostAndPorts(MAP_STR_STR::iterator it);
};

#endif
