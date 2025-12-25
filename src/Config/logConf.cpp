#include "../include/Webserv.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/Session.hpp"

void Config::logConfig()
{

    for (size_t s = 0; s < servers.size(); ++s)
    {

        const ServerConf &serverConf = servers[s];

        for (size_t i = 0; i < serverConf.routes.size(); ++i)
        {

            const RouteConf &r = serverConf.routes[i];
            std::string methods = "";

            for (std::set<std::string>::const_iterator it = r.allowedMethods.begin(); it != r.allowedMethods.end(); ++it)
            {

                if (!methods.empty())
                    methods += ",";
                methods += *it;
            }

            Logger::info("Route[" + Libft::itoa((long)i) + "] path='" + r.path + "' root='" + r.root + "' index='" + r.index +
                         "' autoindex=" + (r.autoindex ? "on" : "off") + " methods=" + methods + " hasUpload=" + (r.hasUpload ? "yes" : "no") +
                         " uploadStore='" + r.uploadStore + "' hasCGI=" + (r.hasCGI ? "yes" : "no"));

            if (r.hasCGI)
            {

                for (std::map<std::string, std::string>::const_iterator mit = r.cgiMap.begin(); mit != r.cgiMap.end(); ++mit)
                    Logger::info("  CGI map: " + mit->first + " -> " + mit->second);
            }
        }
    }
}
