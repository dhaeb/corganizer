#include "LTest.h"
#include "LTestAssert.h"
#include "routes.h"
#include "simple-web-server/client_http.hpp"
#include "LibraryTestMySQL.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <chrono>
#include <chrono>
#include <string>

using std::ostringstream;
using std::ifstream;
using std::istreambuf_iterator;
using std::string;
using std::stringstream;

using boost::starts_with;
using LTAssert::True;
using LTAssert::Equal;
using LTAssert::ExpectException;

typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

const int port = 8080;
httpserver_config c("test/resources/html", port, 1);

string performGetRequest(const char* filepath){
    stringstream hostAndPort;
    hostAndPort << "localhost" << ":" << port;
    auto hostAndPortCstring = hostAndPort.str().c_str();
    HttpClient client(hostAndPortCstring);
    auto r1=client.request("GET", filepath);
    ostringstream a;
    a << r1->content.rdbuf();

    string returnable = a.str();
    return returnable;
}

auto httpserverInstance = manageFixture<httpserver*>()
.before([](httpserver* server)
{
    server = new httpserver(c);
    server->start();

})
.after([](httpserver*
           server)
{
    server->stop();
});

TestSuite httpServerSuite =
{

    ltest().addTest("server stops when it should stop", [](){
        httpserver* s = new httpserver(c);
        s->start();
        s->stop();
        ExpectException<exception>([](){
            performGetRequest("index.html");
        });
    }),

    ltest().addTest("paramtest with validating expect", [](string i)
    {
        httpserverInstance();
        return performGetRequest(i.c_str());
    },[](ParameterTest<string, string> test)
    {
        test.with("/notfindable").expect([](string a)
        {
            bool havingRightboostError = starts_with(a, "Could not open path /notfindable: boost::filesystem::canonical: No such file or directory");
            True(havingRightboostError);
        });
        test.with("/test.txt").expect([](string a)
        {
            Equal(string("testpage"), a, "test server is not running");
        });
        test.with("/").expect([](string a)
        {
            ifstream fixture("test/resources/html/index.html");
            string fixtureContent((istreambuf_iterator<char>(fixture)),
                                  istreambuf_iterator<char>());
            Equal(fixtureContent, a, "index.html is not used when folder is referenced");
        });
    }),


    ltest().addTest("run mysql library test", &testMysql),
};

int main()
{
    ltest().run(httpServerSuite);
    return 0;
}




