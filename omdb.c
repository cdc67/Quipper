#ifndef QUIP_
#define QUIP_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omdb.h"
#include <time.h>
#endif

/* Helper function used to test just this class
int main( int argc, char *argv[] )
{
	char input[256];
	
	printf("Enter a title: ");
	//fgets(input, 256, stdin);
	scanf("%99[0-9a-zA-Z ]s", input);

	//replace space with %20
	char* title  = toHexSpace(input);
	//printf("%s\n", title);
	
	char* id = returnId(title);
	printf("%s\n", id);

	return 0;
}
*/
/** This Method is used to convert a string 
 *  with spaces to a string with %20 so that
 *  the string may be used in a URL
 *  @param const char *s
 *  @return char* b
 * */
char* toHexSpace(const char *s)
{
	char *b=strcpy(malloc(3*strlen(s)+1),s),*p;
	while( p=strchr(b,' ') )
	{
      memmove(p+3,p+1,strlen(p));
		strncpy(p,"%20",3);
	}
	return b; 
}
/** This method is used to call the OMDB api
 *  and parse the return string to return 
 *  the IMDB id for a specfic movie title.
 *  This title can then be passed to a url for
 *  gaining information on the movie.
 *  @param char* title
 *  @return char* idFinal
 *  */
char* returnId(char* title)
{
	if (title[0] == '\0') {
		printf("OMDB - returnId: Bad movie Title\n");
		return "bad";
	}
	char ch, file_name[256];
	FILE *fp;
	
	char returnString[2000];
	memset(returnString, 0, sizeof(returnString));

	char* url = "curl -s -o output_ID.id http://www.omdbapi.com/?t=";
	char* finalUrl = malloc(strlen(url)+strlen(title)+1);
	memset(finalUrl, 0, sizeof(finalUrl));
	
	strcat(finalUrl, url);
	strcat(finalUrl, title);
	
	system(finalUrl);

	fp = fopen("output_ID.id", "r");
	
	while(fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(0);
	}	
	int count = 0;
	while((ch=fgetc(fp)) != EOF)
	{
		returnString[count] = ch;
		count++;	
	}

	char* idSearch = "imdbID\":\""; 
	char* id = strstr(returnString, idSearch); 
	
	id = id+9;

	char* idFinal = strtok(id, "\"");
	
	fclose(fp);
	remove("output_ID.id");

	return idFinal;
}
