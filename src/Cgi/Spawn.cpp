
#include <sys/resource.h>
#include <fcntl.h>
#include <fstream>
#include <limits.h>

#include "Webserv.hpp"

static int g_err = 0;

CGI::CGI() {}

CGI::CGI(const CGI &obj)
{
    (void)obj;
}

CGI &CGI::operator=(const CGI &obj)
{
    (void)obj;
    return (*this);
}

CGI::~CGI() {}

bool CGI::spawn(const ServerConf &srv, const RouteConf &loc, const Request &req, const std::string &script, CGIProcess &proc, int timeoutSecs, size_t maxInput)
{

    (void)srv;

    size_t actualInputSize = 0;
    std::string inputData;

    if (!req.bodyFile.empty())
    {
        bool ok = false;
        inputData = Libft::readFile(req.bodyFile, &ok);
        if (!ok)
        {
            Logger::error("Failed to read body file: " + req.bodyFile);
            return false;
        }
        actualInputSize = inputData.size();
    }
    else if (!req.body.empty())
    {
        inputData = req.body;
        actualInputSize = inputData.size();
    }

    if (actualInputSize > maxInput)
    {
        Logger::error("CGI input " + Libft::itoa((long)actualInputSize) + " bytes exceeds limit " + Libft::itoa((long)maxInput));
        return false;
    }

    std::string executable;
    std::string scriptToRun = script;
    bool needsCompilation = false;
    std::vector<std::string> shebangTokens;
    bool shebangUsed = false;

    size_t dot = script.find_last_of('.');
    if (dot != std::string::npos)
    {

        std::string ext = script.substr(dot);
        std::map<std::string, std::string>::const_iterator it = loc.cgiMap.find(ext);

        if (it != loc.cgiMap.end())
        {

            executable = it->second;

            if (ext == ".c" && executable == "/usr/bin/gcc")
            {

                needsCompilation = true;
                std::string compiledPath;

                if (!handleCFile(script, compiledPath))
                    return false;

                scriptToRun = compiledPath;
                executable = scriptToRun;
            }
        }
        else
        {

            std::ifstream ifs(script.c_str());
            if (ifs.is_open())
            {

                std::string line;
                if (std::getline(ifs, line))
                {

                    if (line.size() >= 2 && line[0] == '#' && line[1] == '!')
                    {

                        size_t pos = 2;
                        while (pos < line.size() && Libft::isspace((unsigned char)line[pos]))
                            ++pos;

                        std::istringstream iss(line.substr(pos));
                        std::string tok;

                        while (iss >> tok)
                            shebangTokens.push_back(tok);

                        if (!shebangTokens.empty())
                            shebangUsed = true;
                    }
                }
                ifs.close();
            }

            if (shebangUsed)
                executable = shebangTokens[0];
            else
            {

                if (!loc.cgiBin.empty())
                    executable = loc.cgiBin;
                else
                    return (false);
            }
        }
    }
    else
    {
        return (false);
    }

    int inPipe[2], outPipe[2], errPipe[2];

    if (pipe(inPipe) < 0)
        return (false);
    if (pipe(outPipe) < 0)
    {
        close(inPipe[0]);
        close(inPipe[1]);
        return (false);
    }
    if (pipe(errPipe) < 0)
    {
        close(inPipe[0]);
        close(inPipe[1]);
        close(outPipe[0]);
        close(outPipe[1]);
        return (false);
    }

    Logger::debug("CGI spawn: executable='" + executable + "' script='" + scriptToRun + "'");

    int pid = fork();
    if (pid < 0)
    {
        close(inPipe[0]);
        close(inPipe[1]);
        close(outPipe[0]);
        close(outPipe[1]);
        close(errPipe[0]);
        close(errPipe[1]);
        return (false);
    }

    if (pid == 0)
    {

        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);
        dup2(errPipe[1], STDERR_FILENO);

        close(inPipe[0]);
        close(inPipe[1]);
        close(outPipe[0]);
        close(outPipe[1]);
        close(errPipe[0]);
        close(errPipe[1]);

        struct rlimit rl;
        rl.rlim_cur = 200 * 1024 * 1024;
        rl.rlim_max = 200 * 1024 * 1024;
        setrlimit(RLIMIT_AS, &rl);

        rl.rlim_cur = timeoutSecs + 2;
        rl.rlim_max = timeoutSecs + 5;
        setrlimit(RLIMIT_CPU, &rl);

        char absPath[PATH_MAX];
        std::string scriptAbsolute = script;
        if (realpath(script.c_str(), absPath) != NULL)
        {
            scriptAbsolute = std::string(absPath);
        }

        std::vector<std::string> envv;

        envv.push_back("REQUEST_METHOD=" + req.method);
        envv.push_back("QUERY_STRING=" + req.query);
        envv.push_back("CONTENT_LENGTH=" + Libft::itoa((long)actualInputSize));

        std::map<std::string, std::string>::const_iterator ctIt = req.headers.find("content-type");
        if (ctIt != req.headers.end())
            envv.push_back("CONTENT_TYPE=" + ctIt->second);
        else
            envv.push_back("CONTENT_TYPE=application/octet-stream");

        envv.push_back("GATEWAY_INTERFACE=CGI/1.1");
        envv.push_back("SERVER_PROTOCOL=HTTP/1.1");
        envv.push_back("SERVER_NAME=localhost");
        envv.push_back("SERVER_PORT=8080");
        envv.push_back("SCRIPT_FILENAME=" + scriptAbsolute);
        envv.push_back("SCRIPT_NAME=" + req.path);
        envv.push_back("PATH_INFO=" + req.path);
        envv.push_back("REQUEST_URI=" + req.path + (req.query.empty() ? "" : "?" + req.query));
        envv.push_back("REDIRECT_STATUS=200");

        std::map<std::string, std::string>::const_iterator hostIt = req.headers.find("host");
        if (hostIt != req.headers.end())
            envv.push_back("HTTP_HOST=" + hostIt->second);

        std::map<std::string, std::string>::const_iterator uaIt = req.headers.find("user-agent");
        if (uaIt != req.headers.end())
            envv.push_back("HTTP_USER_AGENT=" + uaIt->second);

        envv.push_back("REMOTE_ADDR=127.0.0.1");

        std::vector<char *> argv;

        if (needsCompilation)
            argv.push_back(Libft::xstrdup(scriptToRun.c_str()));
        else
        {
            if (!shebangTokens.empty())
            {
                for (size_t i = 0; i < shebangTokens.size(); ++i)
                    argv.push_back(Libft::xstrdup(shebangTokens[i].c_str()));

                size_t sb = scriptToRun.find_last_of('/');
                std::string scriptBase = (sb == std::string::npos) ? scriptToRun : scriptToRun.substr(sb + 1);
                argv.push_back(Libft::xstrdup(scriptBase.c_str()));
            }
            else
            {
                argv.push_back(Libft::xstrdup(executable.c_str()));
                size_t sb = scriptToRun.find_last_of('/');
                std::string scriptBase = (sb == std::string::npos) ? scriptToRun : scriptToRun.substr(sb + 1);
                argv.push_back(Libft::xstrdup(scriptBase.c_str()));
            }
        }
        argv.push_back(NULL);

        std::vector<char *> envp;
        for (size_t i = 0; i < envv.size(); ++i)
            envp.push_back(Libft::xstrdup(envv[i].c_str()));
        envp.push_back(NULL);

        size_t lastSlash = scriptToRun.find_last_of('/');
        if (lastSlash != std::string::npos)
        {
            std::string dir = scriptToRun.substr(0, lastSlash);
            if (!dir.empty() && chdir(dir.c_str()) != 0)
            {
                perror("chdir fail");
                g_err = 127;
            }
        }

        if (needsCompilation)
        {
            size_t lastSlash = scriptToRun.find_last_of('/');
            std::string basename = (lastSlash != std::string::npos) ? scriptToRun.substr(lastSlash + 1) : scriptToRun;
            execve(basename.c_str(), &argv[0], &envp[0]);
        }
        else
        {
            execve(executable.c_str(), &argv[0], &envp[0]);
        }

        perror("execve failed");
        g_err = 127;
    }

    close(inPipe[0]);
    close(outPipe[1]);
    close(errPipe[1]);

    proc.pid = pid;
    proc.inFd = inPipe[1];
    proc.outFd = outPipe[0];
    proc.errFd = errPipe[0];
    proc.startTime = time(NULL);
    proc.inBuf = inputData;

    Logger::debug("CGI spawned pid=" + Libft::itoa(proc.pid) +
                  " inFd=" + Libft::itoa(proc.inFd) +
                  " outFd=" + Libft::itoa(proc.outFd) +
                  " errFd=" + Libft::itoa(proc.errFd));

    return true;
}
