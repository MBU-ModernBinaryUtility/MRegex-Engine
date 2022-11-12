# MRegex-Engine
MBU Regex engine is an under-development regular expression engine and supports the following:

    
    Sub-expressions:
		 (hello): Match hello.
    
    Anchors:
		 ^: Match must start at the beginning of the string.
		 $: Match must occur at the end of the string.
    
    Character class:
		 [char-group]: Match any single character in the char-group.
		 [^char-group]: Negation match any single character not in the char-group.
		 [start-end]: Char range: Match any character between the start and end in the ASCII Table.
		 . : Match any single character in the ASCII table instead '\n' and NULL Byte
	
	 Quantifier:
		 *:  Match the previous element zero or more times
		 +: Match the previous element one or more times
		 ?: Match the previous element zero or one time
		 {min,max}: Match the previous element min times or more but no more than max
	IMPORTANT:
		Whenever you use '.+' or '.*' make sure that the next character is a valid character or escaped character like '\(' or 'v' but MAKE SURE IT'S NOT '('
	
	Modes:
		NoCase: use it whit case insensitive
		NoWhit: ignore all the white spaces
		MultiLine: Use multiline mode, where ^ and $ match the beginning and end of each line (instead of the beginning and end of the input string)
	
Examples:


```
#include <Mregex.h>
int main (void) {
		
char a [] = "[h-v]+", b [] = "H vkk lmn"; 
// will return SUCCES (0)
int c = Regex ( a, b, NULL, NoCase | NoWhite );
		
}
```

Currently, there are more things under-development.

### The Regex function

 The Regex function has 4 parameters 
 the first parameter is the regex string
 the second parameter is the text string (the string to be matched)
 the third parameter is not yet implemented
 the fourth parameter is the modes where it can be ( 0 or NoCase | NoWhite)

`Regex ( regex, str, NULL, MultiLine | NoWhite )`

So, what are you waiting for, go ahead and try it out.
Found a bug Let us know at modernbinaryutility@gmail.com
