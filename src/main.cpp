#include "routes.h"
#include <chrono>
#include <thread>
#include <climits>
#include <future>

using std::this_thread::sleep_for;
using std::chrono::hours;
using std::promise;
int main()
{
    httpserver_config c("src/resources/webroot", 8081, 1);
    httpserver server(c);
    server.start();
    std::promise<void>().get_future().wait(); // forever
    return EXIT_SUCCESS;
}
