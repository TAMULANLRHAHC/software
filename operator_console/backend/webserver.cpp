//
// Created by kemptonburton on 8/29/2025.
//

#include "webserver.h"

//Standard Libraries
#include <sstream>
#include <fstream>
#include <thread>
#include <crow/app.h>

void setup_routes(crow::SimpleApp& app) {
    /*
      adds routes to the crow app
      @param app: the crow app object
      @returns: none
     */

    // Serve static files and fallback to index.html for SPA routing
    CROW_ROUTE(app, "/<path>")
    ([](const crow::request& req, crow::response& res, std::string path) {
        std::string baseDir = "../frontend/dist/";
        std::string fullPath = baseDir + path;

        std::ifstream file(fullPath, std::ios::binary);
        if (file) {
            // Serve the static file if it exists
            std::ostringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();

            if (path.ends_with(".js")) {
                res.set_header("Content-Type", "application/javascript");
            } else if (path.ends_with(".css")) {
                res.set_header("Content-Type", "text/css");
            } else if (path.ends_with(".png")) {
                res.set_header("Content-Type", "image/png");
            } else {
                res.set_header("Content-Type", "application/octet-stream");
            }

            res.write(content);
            res.end();
            return;
        }

        // Fallback to index.html for all other routes (SPA support)
        std::string indexPath = "../frontend/dist/index.html";
        std::ifstream indexFile(indexPath, std::ios::binary);
        if (!indexFile) {
            std::cout << "index.html not found" << std::endl;
            res.code = 404;
            res.end("index.html not found");
            return;
        }

        std::ostringstream buffer;
        buffer << indexFile.rdbuf();
        res.set_header("Content-Type", "text/html");
        res.write(buffer.str());
        res.end();
    });

    // Explicit root route to serve index.html
    CROW_ROUTE(app, "/")([](){
        std::string indexPath = "../frontend/dist/index.html";
        std::ifstream indexFile(indexPath, std::ios::binary);
        if (!indexFile) {
            std::cout << "index.html not found" << std::endl;
            return crow::response(404, "index.html not found");
        }
        std::ostringstream buffer;
        buffer << indexFile.rdbuf();
        crow::response res(buffer.str());
        res.set_header("Content-Type", "text/html");
        return res;
    });
}


std::thread create_web_server(crow::SimpleApp &app, const std::string &ip, int port) {
    /*
      Starts a crow app on another thread.
      @param app: Reference to the crow app.
      @param ip: The IP address that the server binds to.
      @param port: The port that the server is created on.
      @returns: crow app, socket, and worker thread.
    */

    // Set up routes
    setup_routes(app);

    // Start the Crow server in a separate worker thread
    std::thread server_thread([&app, ip, port]() {
        app.bindaddr(ip).port(port).multithreaded().run();
    });

    return std::move(server_thread);
}