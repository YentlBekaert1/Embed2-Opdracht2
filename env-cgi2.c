#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXLEN 80
#define EXTRA 5
/* 4 for field name "data", 1 for "=" */
#define MAXINPUT MAXLEN


void unencode(char *src, char *last, char *dest)
{
 for(; src != last; src++, dest++)
   if(*src == '+')
     *dest = ' ';
   else if(*src == '%') {
     int code;
     if(sscanf(src+1, "%2x", &code) != 1) code = '?';
     *dest = code;
     src +=2; }     
   else
     *dest = *src;
 *dest;
 *++dest;
}
//hier heb ik bij dest de "/n" moeten weg doen anders plaatste hij altijd op het einde van de post een enter en die kwam dan
//ook in mijn json.

int main(void)
{
  char *lenstr;
  char *cookie;
  char input[MAXINPUT], data[MAXINPUT];
  long len;
  printf("%s%c%c\n","Content-Type:text/html;charset=iso-8859-1",13,10);
  printf("<TITLE>Response</TITLE>\n");
  lenstr = getenv("CONTENT_LENGTH");

  if(lenstr == NULL || sscanf(lenstr,"%ld",&len)!=1 || len > MAXLEN){
    printf("<P>Error in invocation - wrong FORM probably.");
  }
  else {
  
    fgets(input, len+1, stdin);
    unencode(input+EXTRA, input+len, data);
    //verdeel de string in iedere form element
    char * data_splt = strtok(data, "&");
    char test[4][40] = {};
    int i=0;
    while( data_splt != NULL ) {
      strcat(test[i],data_splt);
      i++;
      data_splt = strtok(NULL, "&");
    }
    char test2[9][40] = {};
    int a=1;
    for(int o=0; o<4; o++){
        //printf("%s",test[o]);
        char * data_splt2 = strtok(test[o], "=");
        while( data_splt2 != NULL ) {
          strcat(test2[a],data_splt2);
          a++;
          data_splt2 = strtok(NULL, "=");
        }
    }

      printf(test2[8]);

    cookie = getenv("HTTP_COOKIE");
    FILE *f;
    char filename[40] = "";
    sprintf(filename,"../../../var/www/html/%s.json",cookie);
    f = fopen(filename, "a+");
    if(f == NULL){
      printf("<P>Sorry, cannot store your data.");
    }
    else{
        int isFileEmpty = fgetc(f);
        printf("%d",isFileEmpty);
        if(isFileEmpty <0){
          fputs("[{},", f);
          fclose(f);
        }
        fclose(f);
        f = fopen(filename, "r+");
        time_t current_time;
	      struct tm tm = *localtime(&current_time);
        //get the time and put it in a string
        current_time = time(NULL);
        tm = *localtime(&current_time);
        char time[1024];
        sprintf(time, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        //{"animal": "cat","age": "3","color": "grey","gender":"male"}
        char json[100] = "";
        sprintf(json, ",{\"time\":\"%s\",\"animal\":\"%s\",\"age\":\"%s\",\"color\":\"%s\",\"gender\":\"%s\"}]", time, test2[2],test2[4],test2[6],test2[8]);
        fseek(f, -1, SEEK_END);
        fputs(json, f);
        fclose(f);
      }
      printf("<P>File found");
      
      printf("<P>Thank you! Your contribution has been stored.");
    
  }
  const char * redirect_page_format =
    "<html>\n"
    "<head>\n"
    "<meta http-equiv=\"REFRESH\"\n"
    "content=\"0;url=http://rpi-yb/\">\n"
    "</head>\n"
    "</html>\n";
   printf (redirect_page_format);
  return 0;
  
}