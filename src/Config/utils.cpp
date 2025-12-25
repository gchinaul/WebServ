#include "../include/Webserv.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/Session.hpp"

void Config::resetServers()
{

    servers.clear();
    std::vector<ServerConf>().swap(servers);
}

void Config::configureSession(const std::string &path)
{

    for (size_t i = 0; i < cfgs.size(); ++i)
    {

        servers.push_back(cfgs[i]);

        Logger::info("Configuration loaded successfully from: " + path);
        Logger::info("Server: " + cfgs[i].host + ":" + Libft::itoa(cfgs[i].port));
        Logger::info("Routes configured: " + Libft::itoa(cfgs[i].routes.size()));

        SessionManager::instance().setTTL(cfgs[i].sessionTTL);

        if (!cfgs[i].sessionPersist.empty())
        {

            SessionManager::instance().enablePersistence(cfgs[i].sessionPersist);
            Logger::info("Session persistence enabled: " + cfgs[i].sessionPersist);
        }
        Logger::info("Session TTL=" + Libft::itoa((long)cfgs[i].sessionTTL) + "s, cookieSecure=" + (cfgs[i].cookieSecure ? "on" : "off") + ", sameSite=" + cfgs[i].cookieSameSite);
    }
}
