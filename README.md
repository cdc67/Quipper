# Quipper #

Movie Quote generator written in C.

By:
* Chris Callans
* Stephen Calabro
* Ben Greenfield

------

### Overview ###
Quipper is a movie quote generator written in C. It provides functionallity to gain quotes from random movie sources, or to lookup quotes for a specific movie by title. Quipper uses IMDB to gain all of the information for the movies.

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
