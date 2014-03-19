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

// Declare helper struct for movieList
struct movieList {
	char** movies;
	int size;
	int cap;
};


// Declare local functions
int Search_in_File(char* fname, char* str, int bg);
int getQuote(char* movieName, char* movieId, int bg);
char* welcomePage();
void getMovieList(struct movieList* ml);
void getRandomQuote(struct movieList movieList);
void showQuote(char* quote, char* title, int bg);
char* getMoreQuotes(char* movieName, char* movieId, int new);
void init_movieList(struct movieList* ml);
void do_bg(struct movieList* movies);

void do_bg(struct movieList* movies) {
	int max = movies->size;
	if (max == 0) {
		printf("Movie list is empty\n");
		exit(-1);
	}


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

		getQuote(movieName, id, 1);
		sleep(10);
	}
}

void init_movieList(struct movieList* ml) {
	ml->movies = NULL;
	ml->movies = (char**)malloc(sizeof(char*)*4);
	ml->size = 0;
	ml->cap = 4;
}

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
	
char* getMoreQuotes(char* movieName, char* movieId, int new) {
	if (new) {
		printf("\nEnter name of a movie to pull a quote from: ");
		fflush(stdout);
		char movieName[101];
		memset(movieName, 0, sizeof(movieName));
		scanf("\n%99[0-9a-zA-Z ]s", movieName);

		char* title = toHexSpace(movieName);
		char* movieId = returnId(title);

		getQuote(movieName, movieId, 0);
		return strdup(movieName);
	}
	else {
		getQuote(movieName, movieId, 0);
		return movieName;
	}
}

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

int getQuote(char* title, char* movieId, int bg) {

   // Curl web site, save to file
	char* cmd_pre = "curl -so tmp http://www.imdb.com/title/";
   char* cmd_post = "/quotes?ref_=ttqu_ql_4";
   int cmd_length = strlen(cmd_pre) + strlen(cmd_post) + strlen(movieId);
   char* cmd = (char*)malloc(sizeof(char)*cmd_length);
	memset(cmd, 0, cmd_length);
   strcat(cmd, cmd_pre);
   strcat(cmd, strdup(movieId));
   strcat(cmd, cmd_post);
   system(cmd);

   int i = Search_in_File("tmp", title, bg);
	remove("tmp");
	return i;
    
}

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
	char* static_id = (char*)malloc(sizeof(char)*strlen(movieId));
	strncat(static_id, movieId, strlen(movieId));
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

			movieName = getMoreQuotes(movieName, static_id, new_title);
			if (new_title) {
				title = toHexSpace(movieName);
				movieId = returnId(title);
			}
		}
		else  {
			another = 0;
		}
	} while (another);
   
   return 0;
} // end main
