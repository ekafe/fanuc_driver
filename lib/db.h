#include "D:\code_library\mysql_lib\include\mysql.h"

#define eos(s) ((s)+strlen(s))

struct DbParameter {
    int machine_id;
    float cycle_time;
    float operating_time;
    float cutting_time;
    int setup_time;
    int power_on_time;
    int r1;
    int r2;
    int r3;
};

void test_error(MYSQL*, int);

MYSQL* startConnections() {
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    char* server = "localhost";
    char* user = "root";
    char* password = ""; /* set me first */
    char* database = "oee_db";

    conn = mysql_init(NULL);

    /* Connect to database */
    if (!mysql_real_connect(conn, server, user, password,
        database, 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    return conn;
    //int status = { 0 };
    //status = mysql_query(conn, "CALL check_db(1,0)");
    //test_error(conn, status);

    ///* did current statement return data? */
    //res = mysql_store_result(conn);

    //if (res) {
    //    /* process result set, then deallocate it */
    //    MYSQL_ROW row;
    //    while ((row = mysql_fetch_row(res)) != NULL) {
    //        printf("Data : %s", row[0]);
    //    }
    //}

    ///* close connection */
    //mysql_free_result(res);
    //mysql_close(conn);
}

static void test_error(MYSQL* mysql, int status)
{
    if (status)
    {
        fprintf(stderr, "Error: %s (errno: %d)\n",
            mysql_error(mysql), mysql_errno(mysql));
        exit(1);
    }
}

static void test_stmt_error(MYSQL_STMT* stmt, int status)
{
    if (status)
    {
        fprintf(stderr, "Error: %s (errno: %d)\n",
            mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
        exit(1);
    }
}

void checkRepition(float* data1, int* data2){

    // Storing the Data as String (Only Taking Operating Time and Power On Time)
    if(first_time){
        previous_data = floatToString(data1[1])+floatToString(data1[3]);
        LOG(INFO)<<previous_data;
        // cout<<previous_data<<endl;
        first_time = false;
    }else{
        current_data = floatToString(data1[1])+floatToString(data1[3]);
        if(current_data == previous_data){
            repetition = true;
        }else{
            previous_data = current_data;
            repetition = false;
        }
    }
}

/*
   Send the Query to Database to Check Whether Reset in Required Or Nor
*/
bool isResetRecquired(){

    MYSQL* conn = startConnections();
    MYSQL_RES* res;

    int data_result = -1;

    int status = { 0 };
    status = mysql_query(conn, "CALL check_db(2,0)");
    test_error(conn, status);

    /* did current statement return data? */
    res = mysql_store_result(conn);

    if (res) {
        /* process result set, then deallocate it */
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != NULL) {
            //printf("Data : %s", row[0]);
            data_result = atoi(row[0]);
            // printf("Data Converted : %d\n", data_result);
        }
    }

    /* close connection */
    mysql_free_result(res);
    mysql_close(conn);

    // Return Result Based on the Count
    if (data_result > 0) return false; else return true;
}

void assignParameter(struct DbParameter *db, float *data1, int* data2) {

    // Assign the Parameters of Element
    db->machine_id = 2;
    db->cycle_time = data1[0];
    db->operating_time = data1[1];
    db->cutting_time = data1[2];
    db->setup_time = 0;
    db->power_on_time = (int)data1[3];
    db->r1 = data2[0];
    db->r2 = data2[1];
    db->r3 = data2[2];

    checkRepition(data1,data2);

}

void printParameters(struct DbParameter* db) {

    printf("Machine Id : %d\n", db->machine_id);
    printf("Cycle Time : %f\n", db->cycle_time);
    printf("Operating Time : %f\n", db->operating_time);
    printf("Cutting Time : %f\n", db->cutting_time);
    printf("Setup Time : %d\n", db->setup_time);
    printf("Power On Time : %d\n", db->power_on_time);
    printf("R1 : %d\n", db->r1);
    printf("R2 : %d\n", db->r2);
    printf("R3 : %d\n", db->r3);

}

void updateToDatabase(struct DbParameter* db) {

    char url[100] = "CALL add_data(";
    sprintf(eos(url),"%d",db->machine_id);
    sprintf(eos(url),",");
    sprintf(eos(url), "%.3f", db->cycle_time);
    sprintf(eos(url), ",");
    sprintf(eos(url), "%.3f", db->operating_time);
    sprintf(eos(url), ",");
    sprintf(eos(url), "%.3f", db->cutting_time);
    sprintf(eos(url), ",");
    sprintf(eos(url), "%d", db->setup_time);
    sprintf(eos(url), ",");
    sprintf(eos(url), "%d", db->power_on_time);
    sprintf(eos(url), ",");
    sprintf(eos(url), "%d", db->r1);
    sprintf(eos(url), ",");
    sprintf(eos(url), "%d", db->r2);
    sprintf(eos(url), ",");
    sprintf(eos(url), "%d", db->r3);
    sprintf(eos(url), ")");
    /*printf("Url : %s", url);*/

    MYSQL* conn = startConnections();

    int data_result = -1;

    int status = { 0 };
    status = mysql_query(conn, url);
    test_error(conn, status);

    if (!status) {
        LOG(INFO)<<"Data Updated Successfully";
        // printf("Data Updated Successfully\n");
    } 
    else {
        LOG(WARNING) << "Data Updation Failed";
        // printf("Data Updation Failed");
    }

    /* close connection */
    mysql_close(conn);

}

