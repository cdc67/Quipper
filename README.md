# Quipper #

Movie Quote generator written in C.

By:
* Chris Callans
* Stephen Calabro
* Ben Greenfield

------

### Overview ###
Quipper is a movie quote generator written in C. It provides functionallity to gain quotes from random movie sources, or to lookup quotes for a specific movie by title. Quipper uses IMDB to gain all of the information for the movies.

### Features ###
* List movie quotes by Title
* Randomly select movie quotes from a list
* Run as a background process

### Usage ###

To use this program. Download all of the files that are in this repository to a location on your computer. Navigate to that location using a termial program. Once inside simply type make, or make all. To run the program. This will then bring you to the main ui of the program.

Note: You may also just run the following command to build the program (make build), and then run it as any other executible file (./quipper) 


### Documentation ###
This is all of the documentation for the files listed in the program.

===
#### quipper.c ####

###### int main ######

===

#### omdb.c ####

###### char\* toHexSpace(const char \*s) ######
This Method is used to convert a string with spaces to a string with %20 so that the string may be used in a URL

* @param const char \*s 
* @return char\* b

###### char\* returnId(char\* title) ######
This method is used to call the OMDB api
and parse the return string to return 
the IMDB id for a specfic movie title.
This title can then be passed to a url for gaining information on the movie.

* @param char\* title 
* @return char\* idFinal



### Development Log ###

* Using CURL turned out to be a problem with libcurl
  * To fix this we were able to use CURL through a c system call outputing it to a file. Then we were able to parse that file and get the info we needed. After that we remove the file to clean up.
* Using libjson0 did not work
  * To fix this we simply parsed the OMDB api through CURL as described above. We then parsed out just the IMDB id
* Sending movie titles to the OMDB would not work with a space
  * To counter this we had to create a method toHexSpace() that changed all occurances of a space in a string to $20. The hex value. This would work when sending the title to the OMDB api
