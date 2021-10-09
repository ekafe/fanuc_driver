#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <Windows.h>


//#include "../../fwlib32.h"

unsigned short libh;
int allocated = 0;
int ret = 0;

// To Close the Connection
void closeConnection();

#include "lib.h"
#include "db.h"

#define MACHINE_HOST "172.18.30.147"
#define MACHINE_PORT 8193



int main(int argc, char *argv[]) {

    allocated = 0;
    char cnc_id[40] = "";
    uint32_t cnc_ids[4];

    printf("Program Started\n");

    // Connecting to Machine
    printf("connecting to machine at %s:%d...\n", MACHINE_HOST, MACHINE_PORT);
    if ((ret = cnc_allclibhndl3(MACHINE_HOST, MACHINE_PORT, 10, &libh)) != EW_OK) {
        fprintf(stderr, "Failed to connect to cnc! (%d)\n", ret);
        // If Error = -16 - Investigate CNC power supply, Ethernet cable and I/F board.
        ret = 1;
        closeConnection();
    }
    else {
        printf("Machine Connected \n");
    }

    //resetTimerForDay();
    //float cycle_time = readMachineTimer();
    // Check Database Whether Reset Timer is Recquired Or Not
    bool status = isResetRecquired();
    if (status) {
        // Reset All The Timer
        printf("Resetting the Data in Timer");
        resetTimerForDay();
    }

    //int a = 0;
    while (1) {
        float machine_parameter[4];
        int macro_parameters[3];
        readMachineTimer(machine_parameter);
        readMacroVariables(macro_parameters);

        struct DbParameter db;
        assignParameter(&db, machine_parameter, macro_parameters);
        /*printParameters(&db);*/
        updateToDatabase(&db);
        Sleep(2000);
        //a += 1;
    }

    //readMacroVariables();
    //readMachineTimer();
    //Collect All the Timer Paramter and Update to Database

    printf("Program Stopped");

}

// To Close the Connection
void closeConnection() {
    if (allocated && cnc_freelibhndl(libh) != EW_OK)
        fprintf(stderr, "Failed to free library handle!\n");
#ifndef _WIN32
    cnc_exitprocess();
#endif
    printf("Connection Closed");
    exit(1);
}


