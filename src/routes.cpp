#include "routes.h"
#include <iostream>

void httpserver::start(){
    if(is_started){
        cerr << "versuche server nicht zweimal zu starten!" << endl;
    } else {
        is_started = true;
        server_thread = new thread ([this]()
        {
            cout << "starte http server" << endl;
            //Start server
            this->server->start();
        });
    }

}

void httpserver::stop(){  // FIXME kill thread?
    if(is_started){
        server_thread = nullptr;
        is_started = false;
    } else {
        cerr << "versuche server zu stoppen, obwohl er nicht läuft!" << endl;
    }

}
