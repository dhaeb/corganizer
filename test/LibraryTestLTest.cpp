#include "LTest.h"
#include "LTestAssert.h"
#include "routes.h"
#include "simple-web-server/client_http.hpp"
#include "LibraryTestMySQL.hpp"

#include <string>
#include <fstream>
#include <streambuf>
#include <chrono>
#include <thread>
#include <string>

using std::this_thread::sleep_for;
using std::ostringstream;
using std::chrono::seconds;
using std::ifstream;
using std::istreambuf_iterator;
using std::string;

using boost::starts_with;
using LTAssert::True;
using LTAssert::Equal;
using LTAssert::ExpectException;

typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

httpserver_config c("test/resources/html", 8080, 1);

auto httpserverInstance = manageFixture<httpserver*>()
                          .before([](httpserver* server)
{
    server = new httpserver(c);
    server->start();
    sleep_for(seconds(1));
})
.after([](httpserver* server)
{
    delete server;
});

TestSuite httpServerSuite =
{

    ltest().addTest("server stops when it should stop", [](){
        httpserver* s = new httpserver(c);
        s->start();
        sleep_for(seconds(1));
        delete s;
        ExpectException<exception>([](){
            HttpClient client("localhost:8080");
            auto r1=client.request("GET", "index.html");
            ostringstream a;
            a << r1->content.rdbuf();
            string returnable = a.str();
        });
    }),

    ltest().addTest("paramtest with validating expect", [](string i)
    {
        httpserverInstance();
        HttpClient client("localhost:8080");
        auto r1=client.request("GET", i);
        ostringstream a;
        a << r1->content.rdbuf();
        string returnable = a.str();
        return returnable;
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




