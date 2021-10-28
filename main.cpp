#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include <iomanip>
#include <sstream>
#include "assets/fwlib32.h"
#include "D:/code_library/c-logger-0.4.1/src/logger.h"

using namespace std;
bool connected = false;

// Check For Caching
bool first_time = true;
string current_data;
string previous_data;
bool repetition = false;

// Adding Custom Library
#include "easylogging++.h"
#include "lib/machine.h"
#include "lib/db.h"
#include "lib/lib.h"

INITIALIZE_EASYLOGGINGPP
using namespace el;

int main(int argc, char *argv[]){

    // Load configuration from file
    el::Configurations conf("log.conf");
    // Reconfigure single logger
    el::Loggers::reconfigureLogger("default", conf);
    // Actually reconfigure all loggers instead
    el::Loggers::reconfigureAllLoggers(conf);
    // Now all the loggers will use configuration from file

    LOG(INFO) << "Program Started";
    allocated = 0;
    do{
        connected = connectToMachine();
        if(!connected){
            LOG(WARNING) << "Will Try To Connect in 3 Seconds";
            // cout<<"Will Try To Connect in 3 Seconds"<<endl;
            Sleep(3000);    
        }
    }while (!connected);

    // Infinite Loop To Collect The Data
    for(;;){

        // Check the Connectivity is Still There,IF then Collect Parameters
        if(connected){
            collectParameters();
        //Else Do Connection Check until it is Connected
        }else{
            do{
                connected = connectToMachine();
                if(!connected){
                    LOG(WARNING) << "Will Try To Connect in 3 Seconds";
                    // cout<<"Will Try To Connect in 3 Seconds"<<endl;
                    Sleep(3000);    
                }
            }while (!connected);
        }
        Sleep(1000);
    }
    return 0;
}