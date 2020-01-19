#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char *argv[])
{
    FILE *fpin, *fpout;
    int len_in=0, tmp_len=0, i;
    int specify_size=0;
    int padd_len=0;
    char *in_buff;
    char *out_buff;
    char ch, filled;
/*    //for (i=0;i<argc;i++){
    for(i=0;i<argc;i++){
        printf("argc:%d: argv[%d]:%s\n",argc,i,argv[i]);

        ch = getopt(argc,argv,"i:o:p:f:h");
	if (ch == -1) break;

        switch(ch)
        {
            case 'i':
                 //argv[i+2]
            break;
        }

    }
*/
    if(argc < 5 || argc > 6)
    {  printf("The correct format is: append in_file out_file pad_to_len filled_bytes_as[ex:0xff]\n");
       exit(1);
    }

    specify_size = strtol(argv[3], NULL, 0);
    filled = (char)strtol(argv[4], NULL, 0);
//printf("specify_size %d, filled 0x%02x\n",specify_size,(char)filled);
    fpin = fopen(argv[1], "rb");
    if( !fpin )
    {   printf("The file: %s is not found!\n", argv[1]);
        return;
    }

    fseek(fpin, 0, SEEK_END); //seek to end of file
    len_in = ftell(fpin);
//printf("len_in %d\n",len_in);
    fseek(fpin, 0, SEEK_SET); //seek to head of file

//get memory space!
    out_buff = malloc(sizeof(char)*specify_size);
    if(!out_buff){
        printf("malloc output buff fail! size %d\n", specify_size);
        return;
    }
    memset(out_buff, filled, sizeof(char)*specify_size);
//printf("1\n");

    in_buff = malloc(sizeof(char)*len_in);
    if(!in_buff){
        printf("malloc input buff fail! size %d\n", len_in);
        return;
    }

//read data from in file!
    tmp_len = fread(in_buff,sizeof(char),sizeof(char)*len_in,fpin);
    if(tmp_len != len_in)
    {
        printf("error with file len %d != read len %d\n",len_in,tmp_len);
        return;
    }
//printf("2\n");
    padd_len = specify_size - len_in;
//printf("padd_len %d\n",padd_len);
    if(padd_len < 0)
    {
        printf("original size is bigger than you want\n");
        return;
    }
//printf("3\n");
    memcpy(out_buff, in_buff, sizeof(char)*len_in);

       fpout = fopen(argv[2], "wb");
       if( !fpout )
       {
              printf("The file: %s is not found, or no available space!\n", argv[2]);
              return;
       }
//printf("4\n");
    fwrite(out_buff, sizeof(char), sizeof(char)*specify_size, fpout);

    if(fpout) fclose(fpout);
    if(fpin) fclose(fpin);
    if(out_buff) free(out_buff);
    if(in_buff) free(in_buff);
    printf("append done!\n");
    return 0;
}
