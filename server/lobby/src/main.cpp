/**
 * @file server/lobby/src/main.cpp
 * @ingroup lobby
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Main lobby server file.
 *
 * This file is part of the Lobby Server (lobby).
 *
 * Copyright (C) 2012-2016 COMP_hack Team <compomega@tutanota.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @todo This needs major cleanup.

#include "LobbyServer.h"
#include "Log.h"

#include <Decrypt.h>
#include <String.h>
#include <CivetServer.h>
#include <chrono>
#include <thread>

class WebFileHandler : public CivetHandler
{
public:
    WebFileHandler(const libcomp::String& path);

    /**
     * Callback method for GET request.
     *
     * @param server - the calling server
     * @param conn - the connection information
     * @returns true if implemented, false otherwise
     */
    virtual bool handleGet(CivetServer *server, struct mg_connection *conn);

private:
    libcomp::String mPath;
};

WebFileHandler::WebFileHandler(const libcomp::String& path) : mPath(path)
{
}

bool WebFileHandler::handleGet(CivetServer *server, struct mg_connection *conn)
{
    (void)server;

    std::vector<char> page = libcomp::Decrypt::LoadFile(mPath.ToUtf8());

    const char *szContentType;

    if(".png" == mPath.Right(strlen(".png")))
    {
        szContentType = "image/png";
    }
    else
    {
        szContentType = "text/html";
    }

    mg_printf(conn, "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s; charset=UTF-8\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n"
        "\r\n", szContentType, (unsigned int)page.size());
    mg_write(conn, &page[0], page.size());

    return true;
}

class LoginHandler : public CivetHandler
{
public:
    /**
     * Callback method for GET request.
     *
     * @param server - the calling server
     * @param conn - the connection information
     * @returns true if implemented, false otherwise
     */
    virtual bool handleGet(CivetServer *server, struct mg_connection *conn);

    /**
     * Callback method for POST request.
     *
     * @param server - the calling server
     * @param conn - the connection information
     * @returns true if implemented, false otherwise
     */
    virtual bool handlePost(CivetServer *server, struct mg_connection *conn);
};

bool LoginHandler::handlePost(CivetServer *server, struct mg_connection *conn)
{
    return handleGet(server, conn);
}

bool LoginHandler::handleGet(CivetServer *server, struct mg_connection *conn)
{
    (void)server;

    size_t contentLength = mg_get_request_info(conn)->content_length;
    char *postData = new char[contentLength + 1];

    contentLength = mg_read(conn, postData, contentLength);
    postData[contentLength] = 0;

    libcomp::String birthday = "1", id, idReadOnly, pass, passReadOnly,
        idsave = "checked", idsaveReadOnly, cv = "Unknown";

    libcomp::String page;

    std::string postValue;

    if(server->getParam(postData, "quit", postValue))
    {
        page = &libcomp::Decrypt::LoadFile("auth/quit.html")[0];
    }
    else
    {
        page = &libcomp::Decrypt::LoadFile("auth/index.html")[0];
    }

    libcomp::String msg = "<span style=\"font-size:12px;color:#4b4;"
        "font-weight:bold;\"><br>&nbsp;Please enter your "
        "username and password.</span>";

    libcomp::String submit = "<input class=\"login\" type=\"submit\" value=\"\" "
        "tabindex=\"4\" name=\"login\" height=\"60\" width=\"67\" />";

    if(server->getParam(postData, "ID", postValue))
    {
        id = postValue;
    }

    if(server->getParam(postData, "PASS", postValue))
    {
        pass = postValue;
    }

    if(server->getParam(postData, "cv", postValue) && "on" != postValue)
    {
        idsave.Clear();
    }

    if(server->getParam(postData, "cv", postValue))
    {
        cv = postValue;
    }

    libcomp::String cvDisp = cv;

    if(!server->getParam(postData, "login", postValue))
    {
        // Replace our variables
        page = page.Replace("{COMP_HACK_MSG}", msg);
        page = page.Replace("{COMP_HACK_SUBMIT}", submit);
        page = page.Replace("{COMP_HACK_ID}", id);
        page = page.Replace("{COMP_HACK_ID_READONLY}", idReadOnly);
        page = page.Replace("{COMP_HACK_PASS}", pass);
        page = page.Replace("{COMP_HACK_PASS_READONLY}", passReadOnly);
        page = page.Replace("{COMP_HACK_IDSAVE}", idsave);
        page = page.Replace("{COMP_HACK_IDSAVE_READONLY}", idsaveReadOnly);
        page = page.Replace("{COMP_HACK_BIRTHDAY}", birthday);
        page = page.Replace("{COMP_HACK_CV_INPUT}", cv);
        page = page.Replace("{COMP_HACK_CV}", cvDisp);
    }
    else
    {
        page = &libcomp::Decrypt::LoadFile("auth/authenticated.html")[0];

        if(idsave == "checked")
        {
            idsave = "1";
        }
        else
        {
            idsave = "0";
        }

        libcomp::String sid1 = libcomp::Decrypt::GenerateRandom(300).ToLower();
        libcomp::String sid2 = libcomp::Decrypt::GenerateRandom(300).ToLower();

        //mServer->registerWebAuth(id, sid1);

        // Replace our variables
        page = page.Replace("{COMP_HACK_ID}", id);
        page = page.Replace("{COMP_HACK_IDSAVE}", idsave);
        page = page.Replace("{COMP_HACK_BIRTHDAY}", birthday);
        page = page.Replace("{COMP_HACK_SID1}", sid1);
        page = page.Replace("{COMP_HACK_SID2}", sid2);
    }

    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
        "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "%s", page.C());

    delete[] postData;

    return true;
}

int main(int argc, const char *argv[])
{
    (void)argc;
    (void)argv;

#if 1
    std::vector<std::string> options;
    options.push_back("listening_ports");
    options.push_back("10999");

    CivetServer webServer(options);
    webServer.addHandler("/img/backform.png", new WebFileHandler("auth/img/backform.png"));
    webServer.addHandler("/img/btn_login_clicked.png", new WebFileHandler("auth/img/btn_login_clicked.png"));
    webServer.addHandler("/img/btn_login_disabled.png", new WebFileHandler("auth/img/btn_login_disabled.png"));
    webServer.addHandler("/img/btn_login_hover.png", new WebFileHandler("auth/img/btn_login_hover.png"));
    webServer.addHandler("/img/btn_login.png", new WebFileHandler("auth/img/btn_login.png"));
    webServer.addHandler("/img/btn_quit_clicked.png", new WebFileHandler("auth/img/btn_quit_clicked.png"));
    webServer.addHandler("/img/btn_quit_hover.png", new WebFileHandler("auth/img/btn_quit_hover.png"));
    webServer.addHandler("/img/btn_quit.png", new WebFileHandler("auth/img/btn_quit.png"));
    webServer.addHandler("/", new LoginHandler);
#endif

    libcomp::Log::GetSingletonPtr()->AddStandardOutputHook();

    LOG_INFO("COMP_hack Lobby Server v0.0.1 build 1\n");
    LOG_INFO("Copyright (C) 2010-2016 COMP_hack Team\n\n");

    lobby::LobbyServer server("any", 10666);

    return server.Start();
}
