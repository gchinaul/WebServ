
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <algorithm>
#include <iostream>
#include <sys/stat.h>
#include <fstream>

#include "../include/Session.hpp"
#include "../include/LibftCpp.hpp"
#include "../include/Time.hpp"

#ifdef USE_SQLITE
#include <sqlite3.h>
#endif

void SessionManager::runGC()
{

    time_t now = ft_time();

    time_t halfTTL = (time_t)(ttlSeconds / 2);
    time_t cooldown = (halfTTL < (time_t)60) ? halfTTL : (time_t)60;

    if (cooldown < 1)
        cooldown = 1;

    if (now - lastGCTime < cooldown)
        return;

    std::vector<std::string> toRemove;

    for (std::map<std::string, SessionEntry>::iterator it = sessions.begin(); it != sessions.end(); ++it)
    {

        if ((now - it->second.lastAccess) > (time_t)ttlSeconds)
            toRemove.push_back(it->first);
    }
    for (size_t i = 0; i < toRemove.size(); ++i)
        sessions.erase(toRemove[i]);
    lastGCTime = now;

    if (!toRemove.empty())
        std::cerr << "runGC removed " << toRemove.size() << " sessions\n";
    else
        std::cerr << "runGC removed none (now=" << now << " lastGCTime=" << lastGCTime << " ttl=" << ttlSeconds << ")\n";

    if (!persistencePath.empty())
    {

        std::ofstream ofs(persistencePath.c_str());
        if (ofs)
        {

            for (std::map<std::string, SessionEntry>::iterator it = sessions.begin(); it != sessions.end(); ++it)
            {

                ofs << it->first << "\n";
                ofs << it->second.created << " " << it->second.lastAccess << "\n";

                for (std::map<std::string, std::string>::iterator kv = it->second.data.begin(); kv != it->second.data.end(); ++kv)
                    ofs << kv->first << "=" << kv->second << "\n";

                ofs << "--\n";
            }

            ofs.close();
        }
    }
}
