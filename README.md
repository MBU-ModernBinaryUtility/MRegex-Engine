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

Currently, there are more things under-development
