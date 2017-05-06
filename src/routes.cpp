#include "routes.h"
#include <iostream>
#include <chrono>
#include <thread>

using std::this_thread::sleep_for;
using std::chrono::seconds;

template<typename DurationType>  void httpserver::start(const DurationType& d){
     if(is_started){
        cerr << "versuche server zu starten, obwohl er bereits läuft!" << endl;
    } else {
        is_started = true;
        server_thread = new thread ([this]()
        {
            cout << "starte http server" << endl;
            //Start server
            this->server->start();
        });
        server_thread->detach();
        sleep_for(d);
    }
}

void httpserver::start(){
    this->start(seconds(1));
}

template<typename DurationType> void httpserver::stop(const DurationType& d){
    sleep_for(d);
    delete this;
}
void httpserver::stop(){
    this->stop(seconds(1));
}
