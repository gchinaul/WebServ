
#include "Webserv.hpp"
#include "Server.hpp"
#include "Session.hpp"
#include "Logger.hpp"
#include <iostream>
#include <signal.h>
#include <sys/wait.h>

#ifndef lint
/* strings webserv | grep "@(#)" */
static const char webserv_ident[] __attribute__((used)) = "@(#)$Id: main.cpp, v 2.0 2025/12/04 16:16:17 bif Exp $";
#endif

using namespace std;

static volatile sig_atomic_t running = 1;

static Server* g_server_ptr = NULL;

static void sigintHandler(int)
{
    running = 0;
    if (g_server_ptr)
        g_server_ptr->cleanup();
    SessionManager::instance().cleanup();
}

static void sigchldHandler(int)
{
    int status = 0;
    while (waitpid(-1, &status, WNOHANG) > 0) {}
}

int main(int ac, char **av)
{
    string configPath = "config/one.conf";

    if (ac == 2)
    {
        configPath = av[1];
        Logger::info("Conf file: " + configPath);
    }
    else if (ac > 2)
    {
        cerr << "Use: " << av[0] << " [config file]" << endl;
        return 1;
    }
    else
    {
        Logger::info("Use default.conf: " + configPath);
    }

    Server server;
    g_server_ptr = &server;

    if (!server.init(configPath))
    {
        Logger::error("Failed with config: " + configPath);
        return 1;
    }

    Logger::info("Good job! now with: " + configPath);
    Logger::info("Ctrl+C to exit");

    try
    {
        if (signal(SIGINT, sigintHandler) == SIG_ERR)
            Logger::error("Failed SIGINT");
        if (signal(SIGCHLD, sigchldHandler) == SIG_ERR)
            Logger::error("Failed SIGCHLD");
        
        while (running)
        {
            server.loopStep();
        }
    }
    catch (const exception& e)
    {
        Logger::error("Server exception: " + string(e.what()));
        running = 0;
    }
    catch (...)
    {
        Logger::error("Unknow exception");
        running = 0;
    }

    server.cleanup();
    SessionManager::instance().cleanup();
    server.getConfig().cleanup();
    return 0;
}
