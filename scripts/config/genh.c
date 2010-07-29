#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef RBSIZE
#define RBSIZE 100
#endif

#ifndef WBSIZE
#define WBSIZE 100
#endif

#ifndef SPATH
#define SPATHSIZE 100 
#endif

#ifndef DPATH
#define DPATHSIZE 100 
#endif


int main(int argc , char *argv[])
{
        int configfds,hfds;
        int linenumber = 3,rdsize = 0;
        int charcount = -1,errorsyntaxflag = 0;
        int searchcount = 0,sp = 1;
        int linefeedflag = 0 , failflag = 0;
        char *searchstringp = NULL;
        char sourcepath[SPATHSIZE],destpath[DPATHSIZE];
        char readbuffer[RBSIZE],writebuffer[WBSIZE];
  
        memset(readbuffer,0x00,sizeof(readbuffer));
        memset(writebuffer,0x00,sizeof(writebuffer));
        memset(sourcepath,0x00,sizeof(sourcepath));
        memset(destpath,0x00,sizeof(destpath));  

        sprintf(sourcepath,"%s",argv[1]);
        sprintf(destpath,"%s",argv[2]);
        strcat(sourcepath,"/.config");
        strcat(destpath,"/diag_config.h");
        
        configfds = open(sourcepath,O_RDWR);
        if( configfds < 0 ) {
                printf("open .config fail\n");
                return -1;
        }  
      
        hfds = open(destpath,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU|S_IRWXG|S_IRWXO);
        if ( hfds < -1 ) {
                printf("create diag_config.h fail\n");
                return -1;
        }
        
        lseek(configfds,54,SEEK_SET);
        sprintf(writebuffer,"#ifndef __SQ_DIAG_CONFIG_H\n#define __SQ_DIAG_CONFIG_H\n");
        write(hfds,writebuffer,strlen(writebuffer));
        memset(writebuffer,0x00,sizeof(writebuffer));
        sprintf(writebuffer,"/*\n * Automatically generated header file: don't edit\n */\n");
        write(hfds,writebuffer,strlen(writebuffer));
  
        while(1) {
                charcount++;
                rdsize = read(configfds,&readbuffer[charcount],1);
                
                if( rdsize < 0 ){
                        printf("read .config fail\n");
                        perror("read");
                        break;
                }
                else if( rdsize == 0 ) {
                        memset(writebuffer,0x00,sizeof(writebuffer));
                        sprintf(writebuffer,"#endif /* __SQ_DIAG_CONFIG_H */\n");
                        write(hfds,writebuffer,strlen(writebuffer));
                        break;
                }  
                else if( readbuffer[charcount] == 0x0a ) {
                        linenumber++;
                        memset(writebuffer,0x00,sizeof(writebuffer));                        
                        if( (searchstringp = strstr(readbuffer,"#CONFIG_")) || (searchstringp = strstr(readbuffer,"# CONFIG_")) || strstr(readbuffer,"=n")) {
                                errorsyntaxflag = 0; 
                                sp = (int)( searchstringp - readbuffer );
                                for( searchcount = 0 ; searchcount < sp ; searchcount++ ) {
                                        if( readbuffer[searchcount] != 0x20 ) {
                                                errorsyntaxflag = 1;
                                        }        
                                }
                                
                                if( errorsyntaxflag ) {
                                        printf("line %d syntax error at .config\n",linenumber);
                                        printf("generate diag_config.h fail \n");
                                        failflag = 1;
                                        charcount=-1;
                                        memset(readbuffer,0x00,sizeof(readbuffer)); 
                                        continue;
                                }
                                        
                                searchstringp = strstr(readbuffer,"_");
                                sp = (int)( ( searchstringp + 1 ) - readbuffer);
                                
                                for ( searchcount = sp ; searchcount < charcount ; searchcount++ ) {
                                        if( (readbuffer[searchcount] == 0x20 || readbuffer[searchcount] == 0x3d) ) {
                                                readbuffer[searchcount]=0x00;
                                                break;
                                        }                                        
                                }
                                
                                sprintf(writebuffer,"#undef CONFIG_%s\n#define ENABLE_%s 0\n",&readbuffer[sp],&readbuffer[sp]);
                                write(hfds,writebuffer,strlen(writebuffer));
                                memset(writebuffer,0x00,sizeof(writebuffer));
                                sprintf(writebuffer,"#define USE_%s(...)\n#define SKIP_%s(...)  __VA_ARGS__\n\n",&readbuffer[sp],&readbuffer[sp]);
                                write(hfds,writebuffer,strlen(writebuffer));                                                      
                        }
                        else if( searchstringp = strstr(readbuffer,"CONFIG_") ) {
                                errorsyntaxflag = 0; 
                                sp = (int)( searchstringp - readbuffer );
                                for( searchcount = 0 ; searchcount < sp ; searchcount++ ) {
                                        if( readbuffer[searchcount] != 0x20 ) {
                                                errorsyntaxflag = 1;
                                        }        
                                }
                                
                                if( errorsyntaxflag ) {
                                        printf("line %d syntax error at .config\n",linenumber);
                                        printf("generate diag_config.h fail \n");
                                        failflag = 1;
                                        charcount=-1;
                                        memset(readbuffer,0x00,sizeof(readbuffer)); 
                                        continue;
                                }
                                                                                                                                
                                searchstringp = strstr(readbuffer,"_");
                                sp = (int)( ( searchstringp + 1 ) - readbuffer);
                                
                                for ( searchcount = sp ; searchcount < charcount ; searchcount++ ) {
                                        if( (readbuffer[searchcount] == 0x20 || readbuffer[searchcount] == 0x3d) ) {
                                                readbuffer[searchcount]=0x00;
                                                break;
                                        }  
                                }
                                                                                                
                                sprintf(writebuffer,"#define CONFIG_%s 1\n#define ENABLE_%s 1\n",&readbuffer[sp],&readbuffer[sp]);
                                write(hfds,writebuffer,strlen(writebuffer));
                                memset(writebuffer,0x00,sizeof(writebuffer));
                                sprintf(writebuffer,"#define USE_%s(...)  __VA_ARGS__\n#define SKIP_%s(...)\n\n",&readbuffer[sp],&readbuffer[sp]);
                                write(hfds,writebuffer,strlen(writebuffer));          
                        }                        
                        else if( (searchstringp = strstr(readbuffer,"#")) ) {
                                sp = (int)(searchstringp - readbuffer);
                                if( readbuffer[sp+1] != 0x0a ) {
                                        for( searchcount = sp ; searchcount < charcount ; searchcount++ ) {
                                                if( readbuffer[searchcount] == 0x20 ) {
                                                        linefeedflag = 1;
                                                }
                                                else {
                                                        linefeedflag = 0;
                                                }        
                                        }                                
                                
                                        if( !linefeedflag ) {
                                                readbuffer[charcount] = 0x00;
                                                sprintf(writebuffer,"/*\n * %s\n */\n",&readbuffer[sp+1]);
                                                write(hfds,writebuffer,strlen(writebuffer));                                
                                        }
                                        else {
                                                sprintf(writebuffer,"\n");
                                                write(hfds,writebuffer,strlen(writebuffer));
                                        }        
                                }
                                else {
                                        sprintf(writebuffer,"\n");
                                        write(hfds,writebuffer,strlen(writebuffer));
                                }        
                        }
                        else if( readbuffer[0] == 0x0a || (searchstringp = strstr(readbuffer," ")) ) {
                                errorsyntaxflag = 0;
                                if( (searchstringp && !((int)(searchstringp - readbuffer))) || readbuffer[0] == 0x0a ) {
                                        for( searchcount = 0 ; searchcount < charcount ; searchcount++ ) {                                                
                                                if( (readbuffer[searchcount] != 0x20 && readbuffer[searchcount] != 0x0a) ) {                       
                                                        errorsyntaxflag = 1;
                                                }
                                        }       
                                        
                                        if( errorsyntaxflag ) {
                                                printf("line %d syntax error at .config\n",linenumber);
                                                printf("generate diag_config.h fail \n");
                                                failflag = 1;
                                        }
                                        else {
                                                sprintf(writebuffer,"\n");                                        
                                                write(hfds,writebuffer,strlen(writebuffer));                                                                                
                                        }                                        
                                }
                                else {
                                        printf("line %d syntax error at .config\n",linenumber);
                                        printf("generate diag_config.h fail \n");
                                        failflag = 1;                                                                         
                                }                                                                

                        }
                        else {
                                printf("line %d syntax error at .config\n",linenumber);
                                printf("generate diag_config.h fail \n");
                                failflag = 1;                                       
                        }                        
                        charcount=-1;
                        memset(readbuffer,0x00,sizeof(readbuffer)); 
                }     
        }  
        
        close(configfds);
        close(hfds);
        
        if( failflag ) {        
                if( remove(destpath) ) {
                        perror("remove");
                }        
                return -1;
        }                        

        return 0;
}