#include "routes.h"
#include <iostream>

void httpserver::start(){
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
    }
}
