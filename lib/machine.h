
// Machine Parameters
#define MACHINE_HOST "172.18.30.147"
#define MACHINE_PORT 8193

// Focas Library Handler
unsigned short libh;
int allocated = 0;
int connection_result = 0;

bool connectToMachine(){

    // Connect to Fanuc Macine
    // cout<<"Connecting To Machine:\t";
    LOG(INFO) <<"Connecting To Machine";
    connection_result = cnc_allclibhndl3(MACHINE_HOST,MACHINE_PORT,10, &libh);

    if(connection_result == EW_OK){
        LOG(INFO) <<"Machine Connected";
        // cout<<"Machine Connected"<<endl;
        return true;
    }else{
        // fprintf(stderr, "Failed to connect to cnc! (%d)\n", connection_result);
        if(connection_result == -16){
            LOG(WARNING) <<"Connection Failed - Not Connected To Network/ OFF";
            // cout<<"Connection Failed - Not Connected To Network/ OFF"<<endl;
        }
        #ifndef _WIN32
            cnc_exitprocess();
        #endif
            LOG(INFO) <<"Connection Closed";
            // printf("Connection Closed - \t");
        return false;
    }

}

string floatToString(float x){
    string s;
    stringstream ss;
    ss << std::fixed << std::setprecision(2) << x;
    s = ss.str();
    return s;
}