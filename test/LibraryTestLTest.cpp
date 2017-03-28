#include "LTest.h"
#include "LTestAssert.h"
#include "routes.h"
#include "simple-web-server/client_http.hpp"

#include <chrono>
#include <thread>

using std::this_thread::sleep_for;
using std::ostringstream;
using std::chrono::seconds;


using boost::starts_with;
using LTAssert::True;
using LTAssert::Equal;

typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

TestSuite httpServerSuite =
{
    ltest().addTest("test server is running", []()
    {
        httpserver_config c("test/resources/html", 8080, 1);
        httpserver server(c);
        server.start();
        sleep_for(seconds(1));
        HttpClient client("localhost:8080");
        auto r1=client.request("GET", "/test.txt");
        ostringstream a;
        a << r1->content.rdbuf();
        server.stop();
        Equal(string("testpage"), a.str());
    }),

    ltest().addTest("test file is not found", []()
    {
        httpserver_config c("test/resources/html", 8080, 1);
        httpserver server(c);
        server.start();
        sleep_for(seconds(1));
        HttpClient client("localhost:8080");
        auto r1=client.request("GET", "/notfindable");
        ostringstream a;
        a << r1->content.rdbuf();
        server.stop();
        bool havingRightboostError = starts_with(a.str(), "Could not open path /notfindable: boost::filesystem::canonical: No such file or directory");
        True(havingRightboostError);
    }),
};

int main()
{
    ltest().run(httpServerSuite);

    return 0;
}
