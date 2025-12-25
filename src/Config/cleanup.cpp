#include "../include/Webserv.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/Session.hpp"

void Config::cleanup()
{

    for (size_t s = 0; s < servers.size(); ++s)
    {

        ServerConf &serverConf = servers[s];

        for (size_t i = 0; i < serverConf.routes.size(); ++i)
        {

            RouteConf &r = serverConf.routes[i];
            r.allowedMethods.clear();
            std::set<std::string>().swap(r.allowedMethods);
            r.cgiMap.clear();
            std::map<std::string, std::string>().swap(r.cgiMap);
        }

        serverConf.routes.clear();
        std::vector<RouteConf>().swap(serverConf.routes);
    }

    servers.clear();
    std::vector<ServerConf>().swap(servers);
}
