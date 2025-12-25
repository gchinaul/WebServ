
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
# include <sqlite3.h>
#endif

void SessionManager::enablePersistence( const std::string &path ) {

    persistencePath = path;
    persistenceIsSqlite = false;
    
    if ( persistencePath.empty() )
        return;
    
    std::ifstream ifs( persistencePath.c_str() );

    if ( !ifs )
        return;
    
    std::string line;
    while ( std::getline( ifs, line ) ) {

        std::string sid = line;
        
        if ( sid.empty() )
            continue;
        
        time_t created = 0, lastAccess = 0;
        
        if ( !std::getline( ifs, line ) )
            break;
        
        std::istringstream iss( line );
        iss >> created >> lastAccess;
        SessionEntry e;
        e.created = created;
        e.lastAccess = lastAccess;

        while ( std::getline( ifs, line ) ) {

            if ( line == "--" )
                break;
            
            size_t eq = line.find( '=' );

            if ( eq == std::string::npos )
                continue;
            
            std::string k = line.substr( 0, eq );
            std::string v = line.substr( eq+1 );
            e.data[k] = v;
        }
        sessions[sid] = e;
    }
    ifs.close();
}
