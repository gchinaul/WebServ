#include <sys/resource.h>
#include <fcntl.h>
#include <fstream>
#include <limits.h>

#include "../include/Webserv.hpp"

static int g_err = 0;

bool CGI::handleCFile(const std::string &scriptPath, std::string &executablePath)
{

    std::string baseName = scriptPath.substr(0, scriptPath.find_last_of('.'));
    executablePath = baseName + ".out";
    struct stat scriptStat, binStat;

    if (stat(scriptPath.c_str(), &scriptStat) != 0)
        return (false);
    if (stat(executablePath.c_str(), &binStat) == 0)
    {
        if (binStat.st_mtime >= scriptStat.st_mtime)
            return (true);
    }

    std::string gccPath = "/usr/bin/gcc";

    int errPipe[2];
    if (pipe(errPipe) < 0)
        return (false);

    int pid = fork();
    if (pid < 0)
    {
        close(errPipe[0]);
        close(errPipe[1]);
        return (false);
    }

    if (pid == 0)
    {
        char *argv2[5];
        argv2[0] = Libft::xstrdup(gccPath.c_str());
        argv2[1] = Libft::xstrdup(scriptPath.c_str());
        argv2[2] = Libft::xstrdup("-o");
        argv2[3] = Libft::xstrdup(executablePath.c_str());
        argv2[4] = NULL;

        close(errPipe[0]);
        dup2(errPipe[1], STDERR_FILENO);
        close(errPipe[1]);

        execve(gccPath.c_str(), argv2, NULL);
        perror("execve gcc failed");
        g_err = 127;
    }

    close(errPipe[1]);

    std::string compileErr;
    char buf[1024];
    ssize_t nread;
    int status = 0;
    if (waitpid(pid, &status, 0) == -1)
    {
        close(errPipe[0]);
        return (false);
    }
    while ((nread = read(errPipe[0], buf, sizeof(buf))) > 0)
    {
        compileErr.append(buf, nread);
    }

    close(errPipe[0]);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
        return (true);
    }

    if (!compileErr.empty())
        Logger::error("gcc compile error for " + scriptPath + " :" + compileErr);
    else
        Logger::error("gcc compile failed for " + scriptPath + " (no stderr captured)");
    return (false);
}
