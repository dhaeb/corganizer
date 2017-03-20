#include "LTest.h"
#include "LTestAssert.h"
#include "routes.h"
#include "simple-web-server/client_http.hpp"

#include <chrono>
#include <thread>

using std::this_thread::sleep_for;

typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

TestSuite httpServerSuite =
{
    ltest().addTest("test server is running", []()
    {
        httpserver_config c("test/resources/html", 8080, 1);
        httpserver server(c);
        server.start();
        sleep_for(std::chrono::seconds(1));
        HttpClient client("localhost:8080");
        auto r1=client.request("GET", "/test.txt");
        std::ostringstream a;
        a << r1->content.rdbuf();
        server.stop();
        LTAssert::Equal(string("testpage"), a.str());
    }),
};

int main()
{
    ltest().run(httpServerSuite);
    return 0;
}
