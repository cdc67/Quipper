#ifndef QUIP_
#define QUIP_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omdb.h"
#include <time.h>
#endif

#include <sys/types.h>

char* linebrk = 
	"****************************************************************";
int MAX_LEN = 300;
int MAX_QUOTES = 25;

/* Wrapper for a resizable array:
   char** movies - List of movie titles
   int size - Current number of titles in movies
   int cap - Current capacity of movies */
struct movieList {
	char** movies;
	int size;
	int cap;
};


// Declare local functions
int Search_in_File(char* fname, char* title, int bg);
int getQuote(char* movieName, char* movieId, int bg);
char* welcomePage();
void getMovieList(struct movieList* ml);
void getRandomQuote(struct movieList movieList);
void showQuote(char* quote, char* title, int bg);
char* getMoreQuotes(char* movieName, char* movieId, int new);
void init_movieList(struct movieList* ml);
void do_bg(struct movieList* movies);

/* Function that facilitates running the program in background mode. This function
	will continually generate a random movie from movies and make the call to 
	getMoreQuotes(). The function contains an infinite while loop as well as a call
	to sleep(15). This allows the function to output a quote every 15 seconds. Since
	the program is run in the background it will continue indefinitely, call the 
	makefile target finish to kill this process. */
void do_bg(struct movieList* movies) {
	int max = movies->size;
	if (max == 0) {
		printf("Movie list is empty\n");
		exit(-1);
	}

	int i ;
	while (1) {
		int num = rand() % max;
		char* movieName = movies->movies[num];
		if ((movieName == NULL) || (strlen(movieName) == 0)) {
			printf("Bad movieName\n");
			printf("Num: %d, Name: %s\n", num, movies->movies[num]);
			continue;
		}
		char* title = toHexSpace(movieName);
		char* id = returnId(title);
		if (strcmp(id,"bad") == 0) {
			printf("Bad Title: %s\n", movieName);
			continue;
		}

		getQuote(movieName, strdup(id), 1);
		int s;
		for (s = 0; s < 10; s++)
			sleep(1);
	}

}

/* Function that initializes the movieList by mallocing space for array ml.movies
	and sets ml.size and ml.cap. */

void init_movieList(struct movieList* ml) {
	ml->movies = NULL;
	ml->movies = (char**)malloc(sizeof(char*)*4);
	ml->size = 0;
	ml->cap = 4;
}

/* Populates the movieList with the lines contained in the file movie_list so that
	there is a list of random movies to fetch a quote from if the user chooses. 
	Modifies the memory address pointed to by ml. Calls init_movieList(). */
void getMovieList(struct movieList* movies) {
	FILE* fp;
	if ((fp = fopen("movie_list", "r")) == NULL) {
		printf("Error opening movie_list\n");
		exit(-1);
	}

	init_movieList(movies);

	int n = 101;
	char line[n];
	char raw[n];
	memset(raw, 0, sizeof(raw));
	memset(line, 0, sizeof(line));
	while (fgets(raw, sizeof(line), fp) != NULL) {
		sscanf(raw, "%101[0-9a-zA-Z ]s", line);
		if (movies->size >= movies->cap) { // Need to resize
			movies->cap = movies->cap*2;
			movies->movies = (char**)realloc(movies->movies, sizeof(char*)*movies->cap);
		}

		movies->movies[movies->size] = (char*)malloc(sizeof(char)*strlen(line));
		movies->movies[movies->size] = strdup(line);
		movies->size++;
		memset(line, 0, sizeof(line));
		memset(raw, 0, sizeof(raw));
	}

	if (fp)
		close(fp);

}
		
/* Randomly generates a number in the range [0-movieList.cap-1] and selects the
	movie title at that index of movieList.movies. Then proceeds fetch a quote in
	the same manner as if the movie title were input by the user. Prompts the user
	to approve recursive call to generate another random quote. */
void getRandomQuote(struct movieList movieList) {
	int max = movieList.size;
	if (max == 0) {
		printf("Movie list is empty\n");
		exit(-1);
	}

	int num = rand() % max;
	char* movieName = movieList.movies[num];
	char* title = toHexSpace(movieName);
	char* id = returnId(title);

	getQuote(movieName, id, 0);

	char again[15];
	printf("Fetch another random quote? [y/n]: ");
	fflush(stdout);
	scanf("%s", again);
	if ((strcmp(again, "y") == 0) || (strcmp(again, "Y") == 0)) {
		getRandomQuote(movieList);
	}
			
	exit(0);
}
	
/* Function that facilitates continued quote searching without needing to restart
	the program. New determines whether the user will input the title of a new
	movie. If it is set then the movieId must be found by calling returnId().
	Ultimately calls getQuote() with the appropriate movieName and movieId. */
char* getMoreQuotes(char* movieName, char* movieId, int new) {
	if (new) {
		printf("\nEnter name of a movie to pull a quote from: ");
		fflush(stdout);
		char movie_name[101];
		memset(movie_name, 0, sizeof(movie_name));
		scanf("\n%99[0-9a-zA-Z ]s", movie_name);

		char* title = toHexSpace(movie_name);
		char* movie_id = returnId(title);

		getQuote(movie_name, strdup(movie_id), 0);
		return strdup(movie_name);
	}
	else {
		if (strlen(movieId) == 0) {
			char* title = toHexSpace(movieName);
			char* movie_id = returnId(title);
			getQuote(movieName, strdup(movie_id), 0);
			return movieName;
		}
		getQuote(movieName, strdup(movieId), 0);
		return movieName;
	}
}

/* This function searches through the raw html curl response in file fname and 
	parses out an array of quotes. A random index of the quotes array is generated
	and then the quote at that index is passed to showQuote(). 
	The bg parameter is simply passed through to showQuote() and determines whether
	or not the quote should be displayed with bg formatting. */
int Search_in_File(char *fname, char* title, int bg) {
	char* str = "span class=\"character\"";
	FILE *fp;
	int line_num = 1; // line numbers for file
	int copy_line_num = 0;// line numbers for array storing only quotes
	int find_result = 0;
	char temp[512];
	char charName[512]; // for storing the name of the character speaking
	char text[MAX_QUOTES][3000];
	memset(text, 0, sizeof(text));

	if((fp = fopen(fname, "r")) == NULL) {
		printf("Could not open file\n");
		return -1;
	}
	
	// read file
	int copy_true = 0; // marks when to copy a line into the text array or not
	while(fgets(temp, 512, fp) != NULL && find_result < MAX_QUOTES) {
		if((strstr(temp, str)) != NULL) {
			strcpy(charName, temp + 67);

			// extract charname
			char *pch;
			pch = strtok(charName,"<");
		
			// add to current quote in text array
			strcat(text[find_result], "\n*-------");
			strcat(text[find_result], charName);
			strcat(text[find_result], "-------*");
			strcat(text[find_result], "\n");
			copy_true = 1;
		}
		if((strstr(temp, "found this interesting")) != NULL) {
			copy_true = 0;
			find_result++;
		}
		if (copy_true == 1 && strstr(temp, "narrating") == NULL 
				&& strstr(temp, "]") == NULL
		    	&& strstr(temp, "did-you-know-actions") == NULL 
				&& strstr(temp, str) == NULL){

			char *pch;
         char *ptr;
         pch = strtok(temp,"<");
         if ((ptr = strstr(temp, "<")) != NULL) {
            int i;
            i = strlen(temp) - strlen(ptr);
            strncat(text[find_result], temp, i);
         }
         else {
            strcat(text[find_result], temp);
            strcat(text[find_result], "\n");
         }

			copy_line_num++;
		}
		line_num++;
	}

	// Chose random quote
	int i;
	i = rand();
	if (find_result == 0) {
		printf("No quotes found for %s\n", title);
		return;
	}
	int num = i % find_result;
	while (strlen(text[num]) > MAX_LEN) {
		num = (num+1) % MAX_QUOTES;
	}

	// Display Quote
	showQuote(strdup(text[num]), title, bg);
	
	// Close file
  	if(fp) {
		fclose(fp);
   }

	return 0;
}

/* Function that makes the curl call to the imdb url. The url is built using the
	movieId parameter. The bg parameter is passed through to Search_in_File().
	movieName is passed through as well for display in showQuote(). */
int getQuote(char* title, char* movieId, int bg) {

   // Curl web site, save to file
	char* cmd_pre = "curl -so tmp http://www.imdb.com/title/";
   char* cmd_post = "/quotes?ref_=ttqu_ql_4";
   int cmd_length = strlen(cmd_pre) + strlen(cmd_post) + strlen(movieId);
   char* cmd = (char*)malloc(sizeof(char)*cmd_length);
	char* id = strdup(movieId);
	memset(cmd, 0, cmd_length);
   strcat(cmd, cmd_pre);
   strcat(cmd, id);
   strcat(cmd, cmd_post);
   system(cmd);

   int i = Search_in_File("tmp", title, bg);
	remove("tmp");
	return i;
    
}

/* Function that is run at the start of the program. Displays a title header and
	the mode options. Prompts the user to enter a movie Returns the title of a movie
	entered by user. */
char* welcomePage() {
	char* dashes = "----------------------";
	printf("%s\n%s Welcome to Quipper %s\n%s\n", linebrk, dashes,
			dashes, linebrk);
	printf("Which mode would you like to run?\n");
	printf(" 1) Enter move title and retrieve quote\n");
	printf(" 2) Retrieve quote from random movie\n");
	printf(" 3) Quit\n");
	printf("-- Your choice: ");
	fflush(stdout);
	char buf[15];
	scanf("%s", buf);
	if (strcmp(buf, "1") == 0) {
		printf("Enter name of a movie to pull a quote from: ");
		fflush(stdout);
		char movieName[101];
		memset(movieName, 0, sizeof(movieName));
		scanf("\n%99[0-9a-zA-Z ]s", movieName);

		char* title = strdup(movieName);
		return title;
	}
	else if (strcmp(buf, "2") == 0) {
		struct movieList movies;
		getMovieList(&movies);
		getRandomQuote(movies);
		exit(1);
	}
	else 
		exit(0);
}

/* Function to display the quote parameter. Also prints the title of the movie that
	the quote came from. There is some formatting done to keep the text from over
	running a line. Extra newlines are output if bg parameter is set to 1. */
void showQuote(char* q, char* title, int bg) {
	char* cpy = (char*)malloc(sizeof(char)*strlen(q));
	strcpy(cpy,q);
	char* ptr = cpy;
	char buf[80];
	memset(buf, 0, sizeof(buf));
	int count = 0;
	if (bg)
		printf("\n\n");

	printf("%s\n   Quote brought to you from %s\n%s\n", linebrk, title, linebrk);
	while (*ptr != '\0') {
		if (*ptr == '\n') {
			if (count == 0) {
				ptr++;
				printf("\n");
			}
			else {
				printf("   * %-70s*\n",buf);
				ptr++;
				memset(buf, 0, sizeof(buf));
				count = 0;
			}
			continue;
		}
		if (count >= 65 && *ptr == ' ') {
			ptr++;
			printf("   * %-70s*\n", buf);
			fflush(stdout);
			memset(buf, 0, sizeof(0));
			count = 0;
			continue;
		}
		strncat(buf, ptr, 1);
		ptr++;
		count++;
	}
	if (buf[0] != '\0') {
		printf("%s", buf);
		fflush(stdout);
	}
	memset(buf, 0, sizeof(buf));
	printf("\n");
}

/* background mode or not. If so then the movieList is constructed and do_bg is
	called. Otherwise the main function will call the welcomePage to determine the
	mode and then allow the user to continually generate quotes in the mode of their
	chosing. */
int main(int argc, char *argv[]) {

	srand(time(NULL));
	
	
	if (argc > 1) {
		if (strcmp(argv[1], "bg") == 0) { // Call made to random background
			struct movieList movies;
			getMovieList(&movies);
			do_bg(&movies);
		}
	}

	char* movieName = welcomePage();
	char* title = toHexSpace(movieName);
	char* movieId = returnId(title);
	int i = getQuote(movieName, strdup(movieId), 0);
	char again[15];
	char sameMovie[15];
	int another = 0;
	int new_title = 1;
	do {
		printf("Find another quote? [y/n]: ");
		fflush(stdout);
		scanf("%s", again);
		new_title = 1;

		if ((strcmp(again,"y") == 0) || (strcmp(again,"Y") == 0)) {
			another = 1;
			printf("Fetch another quote from %s? [y/n]: ", movieName);
			fflush(stdout);
			scanf("%s", sameMovie);
			if ((strcmp(sameMovie,"y") == 0) || (strcmp(sameMovie,"Y") == 0)) 
				new_title = 0;

			movieName = getMoreQuotes(movieName, movieId, new_title);
			
			//if (new_title) {
				title = toHexSpace(movieName);
				movieId = returnId(title);
			//}
		}
		else  {
			another = 0;
		}
	} while (another);
   
   return 0;
} // end main
