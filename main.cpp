#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include "assets/fwlib32.h"

using namespace std;
// Adding Custom Library
#include "lib/machine.h"
#include "lib/lib.h"

int main(int argc, char *argv[]){

    allocated = 0;
    bool connected = false;
    do{
        connected = connectToMachine();
        cout<<"Will Try To Connect in 3 Seconds"<<endl;
        Sleep(3000);
    }while (!connected);

    while(connected){

    }
    return 0;
}