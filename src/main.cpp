#include "routes.h"
#include <chrono>
#include <thread>
#include <climits>

using std::this_thread::sleep_for;
using std::chrono::hours;

int main()
{
    httpserver_config c("test/resources/html", 8081, 1);
    httpserver server(c);
    server.start();
    while(true){
      sleep_for(hours(LLONG_MAX));
    }
    return EXIT_SUCCESS;
}
