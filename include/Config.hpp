/*
 * RFC 6241 Network Configuration Protocole
 *
 * The NETCONF protocol defines a simple mechanism through wich a network device can be managed
 * configuration data information can be retrieved and new configuration data can be uploaded
 * and manipuled
 *
 */

#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <set>
#include <map>

struct RouteConf
{
    int                     redirectCode;
    bool                    autoindex;
    bool                    directoryListing;
    bool                    hasRedirect;
    bool                    hasUpload;
    bool                    hasCGI;
    std::string             path;
    std::string             root;
    std::string             index;
    std::string             defaultFile;
    std::string             redirect;
    std::string             redirectUrl;
    std::string             uploadDir;
    std::string             uploadStore;
    std::string             cgiExtension;
    std::string             cgiExt;
    std::string             cgiBin;
    std::set<std::string>   allowedMethods;
    std::map<std::string, std::string> cgiMap;

    RouteConf() : redirectCode(301), autoindex(false), directoryListing(false), hasRedirect(false), hasUpload(false), hasCGI(false) {}

};

struct ServerConf
{
    int                     port;
    int                     cgiTimeout;
    bool                    cookieSecure;
    size_t                  clientMaxBody;
    size_t                  cgiMaxInput;
    unsigned int            sessionTTL;
    std::string             host;
    std::string             serverName;
    std::string             root;
    std::string             errorDir;
    std::string             defaultFile;
    std::string             cookieSameSite;
    std::string             sessionPersist;
    std::vector<RouteConf>  routes;

    ServerConf() : port(8080), cgiTimeout(10), cookieSecure(false), 
                   clientMaxBody(10485760), cgiMaxInput(10485760), sessionTTL(3600) {}
};

class Config
{
    public:

        void                        cleanup();
        void                        logConfig();
        void                        resetServers();
        bool                        loadDefaultConfig();
        bool                        load(const std::string &path);
        bool                        parseConfig(const std::string& path);
        void                        configureSession(const std::string& path);
        std::vector<ServerConf>     servers;
        std::vector<ServerConf>     cfgs;
};

#endif
