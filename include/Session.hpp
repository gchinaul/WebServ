
#pragma once
#ifndef SESSION_HPP
#define SESSION_HPP

#include <string>
#include <map>
#include <time.h>
#ifdef USE_SQLITE
# include <sqlite3.h>
#endif

class SessionManager {

    public:

        std::string createSession();
        std::map<std::string, std::string> *getSession( const std::string &id );

        static SessionManager &instance();
        unsigned int getTTL() const;

        void setValue( const std::string &id, const std::string &k, const std::string &v );
        void removeSession( const std::string &id );
        void setTTL( unsigned int seconds );
        void runGC();
        void enablePersistence( const std::string &path );
        void cleanup();

    private:

        SessionManager();
        SessionManager( const SessionManager& obj );
        SessionManager& operator=( const SessionManager& obj );
        ~SessionManager();  
          
        struct SessionEntry {

            std::map<std::string, std::string> data;
            time_t created;
            time_t lastAccess;
        };

        std::map<std::string, SessionEntry> sessions;
        std::string persistencePath;

        unsigned int ttlSeconds;
        time_t lastGCTime;
        bool persistenceIsSqlite;
};

#endif