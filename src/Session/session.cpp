
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

SessionManager &SessionManager::instance()
{

    static SessionManager mgr;
    return (mgr);
}

SessionManager::SessionManager()
{

    Libft::ft_srand(ft_time());
    ttlSeconds = 3600;
    lastGCTime = ft_time();
    persistencePath = "";
    persistenceIsSqlite = false;
}

SessionManager::SessionManager(const SessionManager &obj)
{

    (void)obj;
}

SessionManager &SessionManager::operator=(const SessionManager &obj)
{

    (void)obj;
    return (*this);
}

SessionManager::~SessionManager() {}

std::map<std::string, std::string> *SessionManager::getSession( const std::string &id ) {

    runGC();
    std::map<std::string, SessionEntry>::iterator it = sessions.find( id );
    if ( it == sessions.end() )
        return ( NULL );
    it->second.lastAccess = ft_time();

    return ( &it->second.data );
}

void SessionManager::setValue( const std::string &id, const std::string &k, const std::string &v ) {

    std::map<std::string, SessionEntry>::iterator it = sessions.find(id);

    if ( it == sessions.end() )
        return;

    it->second.data[k] = v;
}

void SessionManager::removeSession( const std::string &id ) {

    sessions.erase( id );
}

void SessionManager::setTTL( unsigned int seconds ) {

    ttlSeconds = seconds;
}

unsigned int SessionManager::getTTL() const {

    return ( ttlSeconds );
}

void SessionManager::cleanup() {

    sessions.clear();
}
