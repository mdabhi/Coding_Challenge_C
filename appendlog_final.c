//*************************************************************************************************************************************
// DESCRIPTION
// The appendlog utility appends the text message specified by the –t option to the log file
// specified by the –f option. If the specified file does not exist, it is created.
// If the optional –a parameter is set, the auxiliary flag will be set for the log record.
// Synopsis: appendlog argc [-a] –f LOGFILE –t TEXT_MESSAGE
// TO EXECUTE THE CODE: ./appendlog 10 -f logfile.txt -t this is my first log 
//                                          OR
//                        ./appendlog 11 -a -f logfile.txt -t this is my first log
//
//*************************************************************************************************************************************

#include "myLib.h"


int main (int argc, char *argv[])
{
    logfile = malloc(10);
    temp.timestmp = malloc(30);
    if ( argc < 5 ) /// argc should be 2 for correct execution 
    {
        /// Print the error message 
        printf("\nEnter all the arguments");
    }
    else 
    {
        if(argv[2][0] == '-' && argv[2][1] == 'f'){  /// check for argument -f folowed by logfile name
            strcpy(logfile, argv[3]);
            if(argv[4][0] == '-' && argv[4][1] == 't'){  /// check for -t followed by text_message
                j=0; /// initialize index of text_msg
                for (i=5; i< argc; i++){
                    if(j<30 ){
                        strcpy(temp.text_msg[j], argv[i]);
                        strcpy(temp.text_msg[j+1]," ");   /// pads the " " between 2 words of txt message
                        j=j+2;
                    }
                }
            }
        }
        else if(argv[2][0] == '-' && argv[2][1] == 'a'){  /// check for optional -a argument
                AUX = 0x01;
                if(argv[3][0] == '-' && argv[3][1] == 'f'){    /// if -a exists, next argument is -f
                    strcpy(logfile,argv[4]);
                }
            
                if(argv[5][0] == '-' && argv[5][1] == 't'){  /// check for -t followed by text_message
                    j=0; /// initialize index of text_msg
                    for (i=6; i<argc; i++){
                        if(j<30 ){
                            strcpy(temp.text_msg[j],argv[i]);
                            strcpy(temp.text_msg[j+1]," ");    /// pads the " " between 2 words of txt message
                            j=j+2;
                        }
                    }
                }
         }
     }    
    
    create_record(logfile);
    write_data(logfile,j);
    checksum_lgfile = checksum_file(logfile);

    printf("Appending log completed!!!!\n");
    free(logfile);
    return 0;
}    


//**********************************************************************************************************
//	create_record()
//	Param:
//		IN :	*lgfile
//		OUT:	record_cnt,temp.lb_sqno,temp.hb_sqno,temp.aux_dst,temp.spare[0],temp.spare[1],temp.checksum 
//	Returns:
//		NONE
//	Description:
//		This API fills up the template for record. It reads the no. of records from textfile, updates the 
//      no.,checks if lower sequence no. is less than 255 or greater,if greater,increaments higher sequence no.
//      It reads the local time through function timestamp(). Also calls a function for calculating the record  
//      checksum.
//
//************************************************************************************************************
void create_record(char *lgfile){
    /////////// create the record according to given format 
    unsigned char *ptr=(unsigned char *)&temp;
    int struct_size=sizeof(struct record);
  
    fp = fopen(lgfile,"r+");  /// opens file in r+ mode,reading+writing mode. Overwrites the existing data
    
    if(fp==NULL){
        printf("Error opening file..");   /// prints error message if file doesn't exist or file cannot be opened
        exit(0);
    }
    
    fseek(fp,0,SEEK_SET);    /// takes the cursor to the beginning of file
    fscanf(fp,"%d",&record_cnt);  /// Updates the no.of records

    /// byte0 and byte1
    if (record_cnt == 0){ // for the first record in log file
        temp.lb_sqno = 1; // Lower byte of sqn no. , byte0 of record. lower byte, starts with right side
        temp.ub_sqno = 0; // Upper byte of sqn no. , byte1 of record
        
        record_cnt = 1;     
     }
     else{
        
        record_cnt++;
        if(record_cnt<=255){    /// lb_sqno is unsigned char and max.value allowed is 255
            temp.lb_sqno = record_cnt;
            temp.ub_sqno = 0;
        }
        else if(record_cnt<511){ ////max. value allowed for record_cnt
            temp.lb_sqno = 255;  /// when record_cnt value>255, upper byte of sq no.starts increamenting
            temp.ub_sqno = record_cnt - 255;
        }
        
      }
    
    fseek(fp,0,SEEK_SET);
    fprintf(fp,"%d",record_cnt);
    fprintf(fp,"\n");
    fprintf(fp,"\n");
    
    fflush(fp);
    fclose(fp);
    
    
    timestamp();       //// byte 3,4,5,6 bytes are for timestamp. byte 2 is for AUX and DST
    if (AUX == 1 && DST == 1){ ///Check if day light saving time is in effect and AUX=SET,if yes, set bit6 and bit7=1, 0xC0
       temp.aux_dst = 0xC0; // byte2 
    }
    else if (AUX == 1 && DST == 0){ /// if AUX=SET and DST=0, bit7 = 1, bit6=0 
        temp.aux_dst = 0x80 ;
    }
    else if(AUX == 1 && DST == 0){  /// if AUX=0 and DST=1, bit7 = 1, bit6=0 
        temp.aux_dst = 0x40 ;
    }
    else{
        temp.aux_dst = 0x00;
    }
        
    /// byte 7,8 spare
    temp.spare[0] = 0;
    temp.spare[1] = 0;
    
    temp.checksum=checksum(ptr,struct_size);
    //printf("\nRecord checksum: 0x%x\n",temp.checksum);  
}


//**********************************************************************************************************
//	checksum()
//	Param:
//		IN :	*data(pointer to struct temp),size of the struct record
//		OUT:	temp.checksum 
//	Returns:
//		temp.checksum(checksum of record data)
//	Description:
//		This API calculates the checksum of record.  
//
//************************************************************************************************************
unsigned char checksum(char *data, int size){
    int i;
    unsigned char checksum;
    
    for(i=0, checksum=0; i<size; i++){    /// Calculates the checksum of the struct record i.e. checksum of log/record
        checksum+=data[i];
    }
    checksum = ~checksum;   /// takes 1's complement of checksum
    return checksum;
}



//**********************************************************************************************************
//	checksum_file()
//	Param:
//		IN :	*lgfile
//		OUT:	checksum,sum of all previous bytes in file 
//	Returns:
//		checksum(checksum of all previous bytes in file)
//	Description:
//		This API calculates the checksum of all the bytes(data) written in file.  
//
//************************************************************************************************************
unsigned char checksum_file(char *lgfile){
    unsigned char checksum;
    int c;

    fp = fopen(lgfile,"a+");    /// opens file in a+ mode, reading+appending mode
    if (fp == NULL){
        printf("Error reading file");  /// Prints error message if file doesn't exist or can't open the file
        exit(0); 
    }
    
    while ((c = fgetc(fp)) != EOF) {    /// runs the loop till end of life, scans logfile
        checksum += (char)c;            /// calculates the checksum of all the previous bytes written in logfile 
    }
    checksum = ~checksum;     /// takes 1's complement of checksum
    //printf("\nFile Checksum: 0x%x\n",checksum);
    fprintf(fp,"\n0x%2x",checksum);
    
    fclose(fp);
    return checksum;
}



//**********************************************************************************************************
//	write_data()
//	Param:
//		IN :	*lgfile, int msg_length
//		OUT:	NONE 
//	Returns:
//		NONE
//	Description:
//		This API writes the log/record into the logfile.  
//
//************************************************************************************************************
void write_data(char *lgfile,int msg_length){
    fp = fopen(lgfile,"a");   /// reads logfile in appending mode i.e. log/info gets appnded to the already saved data 

    if(fp==NULL){
        printf("Error opening file..");   /// Prints error message if file doesn't exist or can't open the file
        exit(0);
    }
    
    fprintf(fp,"\n%3u \t%3u",temp.ub_sqno,temp.lb_sqno);   /// writes upper and lower bytes of sq.no
    fprintf(fp,"\t0x%2x",temp.aux_dst);                     /// writes the byte containing AUX and DST info
    fprintf(fp,"\t%s\t",temp.timestmp);                  /// writes current local time and date
    free(temp.timestmp);
    fprintf(fp,"\t%u\t%u\t",temp.spare[0],temp.spare[1]);  /// writes spare bytes
    /// byte 9-38 is for text message
    for(i=0; i<=msg_length;i++)
    {    
        fprintf(fp,"%s",temp.text_msg[i]);               /// writes text message
         
    }
    fprintf(fp,"\t0x%2x\t",temp.checksum);                 /// writes the record/log checksum
    
    fflush(fp);
    fclose(fp);
}


//**********************************************************************************************************
//	timestamp()
//	Param:
//		IN :	NONE
//		OUT:	Time string temp.timestmp- YYYY-MM-DD HH:MM:SS, DST
//	Returns:
//		NONE
//	Description:
//		This API reads the current local time and date. Also checks if DST mode is on or off.
//
//***********************************************************************************************************
void timestamp()
{    
int isdst;
char *buffer;
time_t rawtime;

    buffer = malloc(30);      // Allocates 30 bytes for buffer dynamically

    time(&rawtime);
    info = gmtime( &rawtime );        /// reads the current local time
    isdst = info->tm_isdst;           /// takes tm_isdst(Is DST flag) into isdst
    
    strftime(buffer,30,"%Y/%m/%d %H:%M:%S",info);
    if(isdst == 1){
        DST = 1;  /// Daylight Saving Time- ON     
    }
    else if(isdst == 0){
        DST = 0;  /// Daylight Saving Time- OFF 
    }
    else{
        printf("\nInvalid DST");   ///Prints error message
    }
    
    strcpy(temp.timestmp,buffer);   /// takes time string from buffer to temp.timestmp
    free(buffer);
}

