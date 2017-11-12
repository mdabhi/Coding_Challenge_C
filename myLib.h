#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#pragma pack(1)

char          *logfile;
char          AUX,DST;
int           i,j;
int           record_cnt;
unsigned char checksum_lgfile;

struct record{
    int lb_sqno; /// byte0
    int ub_sqno; /// byte1
    unsigned char aux_dst; /// byte2
    char *timestmp; /// byte 3,4,5,6 - not implemented , malloc(30)
    unsigned char spare[2]; // byte 7,8 - spare
    unsigned char text_msg[30][8]; // byte 9 to 38
    unsigned char checksum; // byte39 - total 40 bytes
};
struct   record temp = {};
struct   tm *info;
FILE     *fp;

void            create_record(char *);   /// creates the record in the given format 
void            append_record(char *,int);   /// appends the record in Logfile
void            timestamp();
void            write_data(char *,int);
unsigned char   checksum(char *, int);
unsigned char   checksum_file(char *);

