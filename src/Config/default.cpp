#include "../include/Webserv.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/Session.hpp"

bool Config::loadDefaultConfig()
{

    servers.clear();

    std::vector<ServerConf>().swap(servers);

    ServerConf defaultServer;
    defaultServer.host = "localhost";
    defaultServer.port = 8080;
    defaultServer.serverName = "webserv";
    defaultServer.clientMaxBody = 1024 * 1024;
    defaultServer.root = "./www";
    defaultServer.errorDir = "./errors";
    defaultServer.defaultFile = "index.html";

    RouteConf cgiRoute;
    cgiRoute.path = "/cgi";
    cgiRoute.root = "./www/cgi";
    cgiRoute.hasCGI = true;
    cgiRoute.autoindex = false;
    cgiRoute.cgiMap[".py"] = "/usr/bin/python3";
    cgiRoute.cgiMap[".pl"] = "/usr/bin/perl";
    cgiRoute.allowedMethods.insert("GET");
    cgiRoute.allowedMethods.insert("POST");
    defaultServer.routes.push_back(cgiRoute);

    RouteConf uploadRoute;
    uploadRoute.path = "/upload";
    uploadRoute.root = "./www/upload";
    uploadRoute.hasUpload = true;
    uploadRoute.uploadStore = "./www/upload";
    uploadRoute.autoindex = true;
    uploadRoute.allowedMethods.insert("GET");
    uploadRoute.allowedMethods.insert("POST");
    defaultServer.routes.push_back(uploadRoute);

    RouteConf deleteRoute;
    deleteRoute.path = "/delete";
    deleteRoute.root = "./www/delete";
    deleteRoute.autoindex = true;
    deleteRoute.allowedMethods.insert("GET");
    deleteRoute.allowedMethods.insert("DELETE");
    defaultServer.routes.push_back(deleteRoute);

    RouteConf rootRoute;
    rootRoute.path = "/";
    rootRoute.root = "./www";
    rootRoute.autoindex = true;
    rootRoute.index = "index.html";
    rootRoute.allowedMethods.insert("GET");
    defaultServer.routes.push_back(rootRoute);

    servers.push_back(defaultServer);

    return (true);
}
