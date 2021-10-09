#include <math.h>

float convertToSeconds(IODBPRM param1, IODBPRM param2) {

    //printf("Param1 : %ld\n", param1.data[0].prm_val);
    //printf("Param2 : %ld\n", param2.data[0].prm_val);
    //printf("data Type : %hd\n", param1.type);
    //printf("data Type : %hd\n", param2.type);
    
    // Select the Minutes
    float minutes = (float)param2.data[0].prm_val * 60;
    /* printf("Minutes : %f\n", (float)minutes);*/
    
    // Select the Seconds
    float seconds = (float)param1.data[0].prm_val / 1000;

    return (minutes + seconds);
}


void readMultipleParameter(long* prmno, int length) {

    IODBPRM prm[5];
    short ret;

    int part_count;
    float cycle_time;

    ret = cnc_rdparam_ext(libh, prmno, length, prm);
    if (!ret) {
        for (int i = 0; i < length; ++i) {
            printf("No.%ld: = %d\n", prmno[i], prm[i].data[0].prm_val);
        }
        part_count = prm[0].data[0].prm_val;
        cycle_time = (prm[2].data[0].prm_val * 60) + (float)(prm[1].data[0].prm_val / 1000);
        //printf("Seconds : %f", (float)prm[1].data[0].prm_val / 1000);
        //printf("Minutes : %d", prm[2].data[0].prm_val);
    }
    printf("Part Count : %d\n", part_count);
    printf("Cycle Time : %f\n", cycle_time);

}

/*
* Power On Time can be read through 1 Parameter - 6750
* 6750 - Total Power On Period in Minutes (0-999999999)
* So Power on Time Can be Directly Read and It will be in Minutes.
*
* Operating Time can be read Through 2 Parameters - 6751 and 6752
* 6751 - Total Operating Time Containing MilliSeconds Only (0-60000)
* 6752 - Total Operating Time Containing Minutes Only (0-999999999)
* So Operating Time in Seconds = (6752*60)+(6751/1000)seconds
*
* Cutting Time can be read Through 2 Parameters -6753 and 6754
* 6753 - Total Cutting Time Containing Milliseconds Only (0-60000)
* 6754 - Total Cutting Time Contatining Minutes Only (0-999999999)
* So Cutting Time in Seconds = (6753*60)+(6754/1000)seconds
*
* Cycle Time can be read Through 2 Parameters - 6757 and 6758
* 6757 - In Current Cycle Time Containing MilliSeconds (0-60000)
* 6758 - In Current Cycle Time Containing Minutes (0-99999999)
* So Cycle Time in Seconds = (6758*60)+(6757/1000)seconds.
*
* Part Count can read Through 1 Parameter - 6711
* 6711 - Total Cycle Time in Numbers (0-999999999)
* It will be Incremented By M30 or Variable Provided by 6710 (6700 0-bit: 0 M02 or M30 or code Specified in 6710, 0-bit:1 Only MCode in 6710)
* cnc_rdparam_ext  = for reading Mutliple Values
*/
void readMachineTimer(float* returnArray) {

    float cycle_time = -1;
    float operating_time = -1;
    float part_count = -1;
    float power_on_time = -1;
    float cutting_time = -1;
    long parameter[] = { 6750,6751,6752,6753,6754,6757,6758,6711 };
    int length = 8;

    IODBPRM struct_param[8]; // 8 Variable to Store Data
    // cnc_rdparam_ext  = for reading Mutliple Values
    short result = cnc_rdparam_ext(libh, parameter, length, struct_param);

    // Check the Result
    if (!result) {
        power_on_time = (float)struct_param[0].data[0].prm_val;
        operating_time = convertToSeconds(struct_param[1], struct_param[2]);
        cutting_time = convertToSeconds(struct_param[3], struct_param[4]);
        cycle_time = convertToSeconds(struct_param[5], struct_param[6]);
        part_count = (float)struct_param[7].data[0].prm_val;
    }
    else {
        printf("Error : %hd", result);
    }

    //printf("Power On Time : %f\n", power_on_time);
    //printf("Cycle Time : %f\n", cycle_time);
    //printf("Operating Time : %f\n", operating_time);
    //printf("Cutting Time : %f\n", cutting_time);
    //printf("Part Count : %f\n", part_count);

    returnArray[0] = cycle_time; 
    returnArray[1] = operating_time;
    returnArray[2] = cutting_time;
    returnArray[3] = power_on_time;
}


/*
* Parameter to Reset Daily - Operating Time, Cutting Time, Power On Timeand Part Number
* 6750 - Power On Time
* 6751,6751 - Operating Time
* 6753,6754 - Cutting Time
* 6711 - Part Produced
*/
void resetTimerForDay() {

    // Parameter To Changes
    int parameters[6] = { 6750,6751,6752,6753,6754,6711 };

    // Pointers for Structures
    IODBPSD* param, * ptr;

    // Result and Index Variable
    short result, idx;

    // Assinging the Allocation for Pointer
    param = (IODBPSD*)calloc(1, 100);
    ptr = param;

    // Assinging the Variable to Pointer Structure
    for (idx = 0; idx < 6; idx++) {
        ptr->datano = parameters[idx];
        ptr->type = 0x0300;
        ptr->u.ldata = 0;
        ptr = (IODBPSD*)(((char*)ptr) + 8);
    }
    // Executing the Function
    result = cnc_wrparas(libh, 8 * 6, param);
    if (result != EW_OK) {
        fprintf(stderr, "Failed to Write parameters!\n");
        result = 1;
        // closeConnection();
    }
    else {
        printf("Value Updated\n");
    }

    // Freeing the Memory Allocation
    free(param);
}

void readMacroVariables(int * returnArray) {

    // Start and End Point of Macro Variable
    short start = 701;
    short end = 703;

    IODBMR* macror;
    char strbuf[12];
    short ret, idx;
    macror = (IODBMR*)malloc(1000);
    ret = cnc_rdmacror(libh, start, end, 1000, macror);
    if (!ret)
        for (idx = 0; idx <= end - start; idx++) {
            int decimal_points = (int)macror->data[idx].dec_val;
            int value = -1;
            if (decimal_points > 0) {
                value = (int)macror->data[idx].mcr_val / pow(10, decimal_points);
            }
            else {
                value = (int)macror->data[idx].mcr_val;
            }
            /*printf("Datas : %d\n", value);*/
            returnArray[idx] = value;
        }
    else
        printf("ERROR!(%d)\n", ret);
    free(macror);
    /*return (ret);*/
}





