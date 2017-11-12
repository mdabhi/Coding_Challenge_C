//*************************************************************************************************************************************
// DESCRIPTION
// The dumplog utility prints each record in the log file specified by the –f option to standard
// output. Printed output is decoded in a user-friendly way, eliminating the need to refer to the file
// format specification
// TO EXECUTE THE CODE: ./dumplog 4 -f logfile.txt 
//                                       
//
//*************************************************************************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char          *logfile;

int main(int argc, char* argv[])
{
    int c,i;

    if(argc <4){
        printf("Error Message: Check your arguments");
        exit(1); 
    }
    logfile= malloc(10);
    
    if(argv[2][0] == '-' && argv[2][1] == 'f'){  /// check for argument -f followed by logfile name
        strcpy(logfile, argv[3]);
    }
          
    FILE* fp = fopen(logfile, "r"); /// open the file in reading mode
    
    if(fp == NULL){
        printf("Error Message: File can not be opened..");
        free(logfile);
        
        exit(1); 
    }
    
    while ((c = fgetc(fp)) != EOF) {    /// runs the loop till end of life, scans logfile
        printf("%c",c);            /// calculates the checksum of all the previous bytes written in logfile 
        for(i=0;i<200;i++){}
    }
    
    if(((c = fgetc(fp)) == EOF)){
        fseek(fp, 0, SEEK_SET);
        printf("\nReading lofgile completed!!!!\n");
    }
    
    fflush(fp);
    fclose(fp);
    free(logfile);
  
    return 0;
}
