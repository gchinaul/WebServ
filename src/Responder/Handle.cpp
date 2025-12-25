
#include <dirent.h>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>

#include "Responder.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "MultipartParser.hpp"
#include "HTTP.hpp"
#include "LibftCpp.hpp"
#include "Session.hpp"
#include "Logger.hpp"

Response Responder::handleGet(const ServerConf &srv, const RouteConf &loc, const Request &req)
{

    if (loc.hasRedirect)
    {

        Response r;
        r.status = loc.redirectCode;
        r.headers["Date"] = HTTP::dateNow();
        r.headers["Location"] = loc.redirectUrl;
        r.headers["Connection"] = "keep-alive";

        return (r);
    }

    std::string rel = "";

    if (req.path.size() >= loc.path.size())
        rel = req.path.substr(loc.path.size());
    if (rel.size() && rel[0] == '/')
        rel = rel.substr(1);

    std::string fsPath;

    if (loc.root.empty())
        fsPath = Libft::safePath(srv.root, req.path.size() > 0 ? req.path.substr(1) : "");
    else
        fsPath = Libft::safePath(loc.root, rel);

    Logger::debug("handleGet: req.path=" + req.path + ", loc.path=" + loc.path + ", rel=" + rel);
    Logger::debug("handleGet: srv.root=" + srv.root + ", loc.root=" + loc.root + ", fsPath=" + fsPath);

    if (Libft::isDir(fsPath))
    {
        Logger::debug("handleGet: " + fsPath + " is a directory");

        if (!loc.index.empty())
        {

            std::string idx = Libft::pathJoin(fsPath, loc.index);
            Logger::debug("handleGet: trying index file: " + idx);

            if (Libft::fileExists(idx))
            {

                Logger::debug("handleGet: index file exists, serving: " + idx);
                return sendFile(srv, idx);
            }
            else
                Logger::debug("handleGet: index file not found: " + idx);
        }
        else if (!srv.defaultFile.empty())
        {

            std::string idx = Libft::pathJoin(fsPath, srv.defaultFile);
            Logger::debug("handleGet: trying server default index file: " + idx);

            if (Libft::fileExists(idx))
            {

                Logger::debug("handleGet: server default index exists, serving: " + idx);
                return (sendFile(srv, idx));
            }
        }

        if (loc.autoindex)
        {

            Logger::debug("handleGet: autoindex enabled, listing directory");
            return (listDir(srv, req.path, fsPath));
        }
        Logger::debug("handleGet: directory forbidden (no index, no autoindex)");
        return makeError(srv, 403);
    }
    if (Libft::fileExists(fsPath))
    {

        Logger::debug("handleGet: file exists, serving: " + fsPath);
        return sendFile(srv, fsPath);
    }
    Logger::debug("handleGet: file not found: " + fsPath);

    if (req.path == "/session/set")
    {

        std::string sid = SessionManager::instance().createSession();

        Response r;
        r.status = 200;
        r.headers["Date"] = HTTP::dateNow();
        r.headers["Content-Type"] = "text/plain";
        r.body = "session set: " + sid + "\n";
        r.headers["Content-Length"] = Libft::itoa(r.body.size());
        unsigned int ttl = SessionManager::instance().getTTL();
        r.headers["Set-Cookie"] = Libft::makeSetCookie("SID", sid, "/", true, false, (int)ttl, "Lax");

        return (r);
    }

    if (req.path == "/session/get")
    {

        std::string sid = "";
        std::map<std::string, std::string> cookies;
        std::map<std::string, std::string>::const_iterator it = req.headers.find("cookie");

        if (it != req.headers.end())
            cookies = Libft::parseCookies(it->second);
        std::map<std::string, std::string>::const_iterator got = cookies.find("SID");
        if (got != cookies.end())
            sid = got->second;

        Response r;
        r.status = 200;
        r.headers["Date"] = HTTP::dateNow();
        r.headers["Content-Type"] = "text/plain";

        if (!sid.empty())
        {

            std::map<std::string, std::string> *s = SessionManager::instance().getSession(sid);

            if (s)
                r.body = "session found: " + sid + "\n";
            else
                r.body = "no session for id: " + sid + "\n";
        }
        else
            r.body = "no SID cookie sent\n";

        r.headers["Content-Length"] = Libft::itoa(r.body.size());

        return (r);
    }

    return (makeError(srv, 404));
}

Response Responder::handlePost(const ServerConf &srv, const RouteConf &loc, const Request &req)
{

    if (loc.hasCGI)
        return (makeError(srv, 501));

    if (!loc.hasUpload && loc.uploadStore.empty())
    {

        Logger::debug("handlePost: upload not allowed for route '" + loc.path + "' (hasUpload=" + (loc.hasUpload ? "yes" : "no") + ", uploadStore='" + loc.uploadStore + "')");
        return (makeError(srv, 405));
    }

    std::map<std::string, std::string>::const_iterator ctIt = req.headers.find("content-type");
    std::string contentType = (ctIt != req.headers.end()) ? ctIt->second : "";

    if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
    {

        std::map<std::string, std::string> formData = parseFormData(req.body);
        std::vector<std::string> storePaths;
        storePaths.push_back("www/testced/received.txt");
        storePaths.push_back("www/testUpload/received.txt");
        storePaths.push_back("www/upload/received.txt");
        std::string toStore;

        if (formData.empty() && !req.body.empty())
            toStore = req.body + "\n";
        else
        {

            for (std::map<std::string, std::string>::const_iterator it = formData.begin(); it != formData.end(); ++it)
                toStore += it->first + "=" + it->second + "\n";
        }

        for (size_t i = 0; i < storePaths.size(); ++i)
        {

            if (!toStore.empty())
            {

                FILE *f = fopen(storePaths[i].c_str(), "a");

                if (f)
                {

                    fwrite(toStore.c_str(), 1, toStore.size(), f);
                    fclose(f);
                }
                else
                    Logger::error("Impossible d'ouvrir le fichier de stockage: " + storePaths[i]);
            }
        }

        std::string responseBody = "<html><body><h1>Formulaire reçu</h1>";
        responseBody += "<p>Données reçues :</p><ul>";

        if (formData.empty() && !req.body.empty())
            responseBody += "<li>" + Libft::htmlEscape(req.body) + "</li>";
        else
        {

            for (std::map<std::string, std::string>::const_iterator it = formData.begin(); it != formData.end(); ++it)
                responseBody += "<li><strong>" + Libft::htmlEscape(it->first) + "</strong>: " + Libft::htmlEscape(it->second) + "</li>";
        }

        responseBody += "</ul>";
        responseBody += "<p><a href=\"/\">Retour</a></p>";
        responseBody += "</body></html>";

        Response r;
        r.status = 200;
        r.headers["Date"] = HTTP::dateNow();
        r.headers["Content-Type"] = "text/html; charset=UTF-8";
        r.headers["Content-Length"] = Libft::itoa(responseBody.size());
        r.headers["Connection"] = "keep-alive";
        r.body = responseBody;

        return (r);
    }
    else
    {

        std::vector<std::string> storedFiles;

        if (contentType.find("multipart/form-data") != std::string::npos)
        {

            if (!loc.uploadStore.empty())
            {

                struct stat st;
                if (stat(loc.uploadStore.c_str(), &st) == 0)
                {

                    if (!S_ISDIR(st.st_mode))
                    {

                        unlink(loc.uploadStore.c_str());

                        if (mkdir(loc.uploadStore.c_str(), 0777) != 0)
                        {

                            Logger::error("Failed to create upload store directory: " + loc.uploadStore);
                            return (makeError(srv, 500));
                        }
                    }
                }
                else
                {

                    if (mkdir(loc.uploadStore.c_str(), 0777) != 0)
                    {

                        Logger::error("Failed to create upload store directory: " + loc.uploadStore);
                        return (makeError(srv, 500));
                    }
                }
            }

            std::vector<MultipartPart> parts;

            if (!req.bodyFile.empty())
                parts = MultipartParser::parseFile(req.bodyFile, contentType);
            else
                parts = MultipartParser::parse(req.body, contentType);

            for (size_t i = 0; i < parts.size(); ++i)
            {

                MultipartPart &p = parts[i];
                std::string fname = p.filename;

                if (fname.empty())
                {

                    if (!p.name.empty())
                        fname = p.name + ".bin";
                    else
                        fname = Libft::itoa((long)time(NULL)) + "_" + Libft::itoa((long)i) + ".bin";
                }

                std::string dest = Libft::safePath(loc.uploadStore, fname);

                if (Libft::writeFile(dest, p.body))
                    storedFiles.push_back(dest);
                else
                    Logger::error("Failed to write uploaded file to: " + dest);
            }

            if (!req.bodyFile.empty())
                std::remove(req.bodyFile.c_str());
        }
        else
        {

            std::string rel = "";

            if (req.path.size() >= loc.path.size())
                rel = req.path.substr(loc.path.size());

            if (rel.size() && rel[0] == '/')
                rel = rel.substr(1);

            if (rel.empty())
                rel = "upload.bin";

            std::string dest = Libft::safePath(loc.uploadStore, rel);

            if (Libft::writeFile(dest, req.body))
                storedFiles.push_back(dest);
        }

        if (storedFiles.empty())
            return (makeError(srv, 500));

        std::string body = "Uploaded files:\n";

        for (size_t i = 0; i < storedFiles.size(); ++i)
            body += storedFiles[i] + "\n";

        Response r;
        r.status = 201;
        r.headers["Date"] = HTTP::dateNow();
        r.headers["Content-Type"] = "text/plain";
        r.body = body;
        r.headers["Content-Length"] = Libft::itoa(r.body.size());
        r.headers["Connection"] = "keep-alive";

        return (r);
    }
}

Response Responder::handleDelete(const ServerConf &srv, const RouteConf &loc, const Request &req)
{
    std::string rel = "";

    if (req.path.size() >= loc.path.size())
        rel = req.path.substr(loc.path.size());

    if (rel.size() && rel[0] == '/')
        rel = rel.substr(1);

    std::string fsPath;
    if (loc.root.empty())
        fsPath = Libft::safePath(srv.root, req.path.size() > 0 ? req.path.substr(1) : "");
    else
        fsPath = Libft::safePath(loc.root, rel);

    if (!Libft::fileExists(fsPath))
        return (makeError(srv, 404));

    if (std::remove(fsPath.c_str()) != 0)
        return (makeError(srv, 403));

    Response r;
    r.status = 204;
    r.headers["Date"] = HTTP::dateNow();
    r.headers["Content-Length"] = "0";
    r.headers["Connection"] = "keep-alive";

    return (r);
}
