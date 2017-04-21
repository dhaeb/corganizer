#ifndef ROUTES_HPP_INCLUDED
#define ROUTES_HPP_INCLUDED

#include "simple-web-server/server_http.hpp"

#include <boost/filesystem.hpp>

#include <algorithm>
#include <string>
#include <map>
#include <fstream>
#include <exception>
#include <vector>

using std::distance;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::make_shared;
using std::ifstream;
using std::string;
using std::function;
using std::shared_ptr;
using std::thread;
using std::map;
using std::invalid_argument;
using std::exception;
using std::ios;
using std::streamsize;
using std::equal;

using SimpleWeb::ServerBase;
using SimpleWeb::Server;
using SimpleWeb::HTTP;

typedef Server<HTTP> HttpServer;
typedef ServerBase<HTTP>::Request Request;
typedef ServerBase<HTTP>::Response Response;

enum http_method
{
    GET, POST, PUT, DELETE, HEAD
};

class httpserver_config
{

    string root;
    int port,
        count_threads;


public :

    httpserver_config(string r, int port, int count_threads) : root(r), port(port), count_threads(count_threads) {}

    int getPort()
    {
        return port;
    }
    int getCountThreads()
    {
        return count_threads;
    }
    string getRoot()
    {
        return root;
    }
};

class httpserver
{

    httpserver_config conf;
    map<http_method, string> method_to_string;

    HttpServer* server;
    thread* server_thread;
    bool is_started = false;

public :

    httpserver(httpserver_config conf) : conf(conf)
    {
        server = new HttpServer(conf.getPort(), conf.getCountThreads(), 5, 300);
        method_to_string[http_method::HEAD] = "HEAD";
        method_to_string[http_method::GET] = "GET";
        method_to_string[http_method::POST] = "POST";
        method_to_string[http_method::PUT] = "PUT";
        method_to_string[http_method::DELETE] = "DELETE";

        //Default GET-example. If no other matches, this anonymous function will be called.
        //Will respond with content in the web/-directory, and its subdirectories.
        //Default file: index.html
        //Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
        server->default_resource["GET"]=[this](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request)
        {
            try
            {
                auto web_root_path=boost::filesystem::canonical(this->conf.getRoot().c_str());
                auto path=boost::filesystem::canonical(web_root_path/request->path);
                //Check if path is within web_root_path
                bool isRelativePath = distance(web_root_path.begin(), web_root_path.end()) > distance(path.begin(), path.end()),
                     isStartingWithRoot = equal(web_root_path.begin(), web_root_path.end(), path.begin()),
                     isNoDirectoryTraversal = isRelativePath | not isStartingWithRoot;
                if(isNoDirectoryTraversal){
                    throw invalid_argument("path must be within root path");
                }
                if(boost::filesystem::is_directory(path)){
                    path/="index.html";
                }
                bool fileIsNotUsable = not boost::filesystem::exists(path) or not boost::filesystem::is_regular_file(path);
                if(fileIsNotUsable){
                    throw invalid_argument("file does not exist");
                }


                auto ifs=make_shared<ifstream>();
                ifs->open(path.string(), ifstream::in | ios::binary);

                if(*ifs)
                {
                    ifs->seekg(0, ios::end);
                    auto length=ifs->tellg();

                    ifs->seekg(0, ios::beg);

                    *response << "HTTP/1.1 200 OK\r\nContent-Length: " << length << "\r\n\r\n";
                    default_resource_send(server, response, ifs);
                }
                else
                    throw invalid_argument("could not read file");
            }
            catch(const exception &e)
            {
                string content="Could not open path "+request->path+": "+e.what();
                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
            }
        };
    }

    // this class does not need copies from itself
    httpserver(const httpserver& that) = delete;
    httpserver& operator=(const httpserver&) = delete;

    void register_route(http_method, string, function<void(Request, Response)>);
    void start();

    ~httpserver()
    {
        delete server_thread;
        delete server;
    }

private :

    void default_resource_send(HttpServer* server, const shared_ptr<HttpServer::Response> &response, const shared_ptr<ifstream> &ifs)
    {
        //read and send 128 KB at a time
        static vector<char> buffer(131072); // Safe when server is running on one thread
        streamsize read_length;
        if((read_length=ifs->read(&buffer[0], buffer.size()).gcount())>0)
        {
            response->write(&buffer[0], read_length);
            if(read_length==static_cast<streamsize>(buffer.size()))
            {
                server->send(response, [this, server, response, ifs](const boost::system::error_code &ec)
                {
                    if(!ec)
                        default_resource_send(server, response, ifs);
                    else
                        cerr << "Connection interrupted" << endl;
                });
            }
        }
    }

};

class routes
{

};


#endif // ROUTES_HPP_INCLUDED
