# Ranker
A Linux command line program written in C that generates rankings from pariwise comparisons.

The program takes up to 2 optional command line arguments. <br/>
The first is an input file containing the list of elements to rank with each element appearing on its own line.<br/>
The second is a save file where each line is directly associated with the line in the input file and contains 2 comma-seperated integers.<br/>
If there are no command line arguments, the program will promt an input file from standard in.<br/>

The program loops each time printing two random elements to standard out and promting a response from stdin.

Valid responses are:<br/><br/>
    '1' or '2' indicating your prefered element from the 2 given<br/>
    or<br/>
    's' indicating "save"<br/>
    or<br/>
    'q' indicating "quit"<br/>
  
on "save" the program will promt the user for a file in which to save the results. If the file does not exist, one will be created if possible.

on "quit" the program will promt the user whether he wants to save. The user will then input:<br/>
    'y' indicating "yes"<br/>
    or <br/>
    'n' indicating "no"<br/><br/>
 on "yes," the program will follow the save protocol.<br/>
 
 on "quit," the program will stop execution and will then output a list to stdout containing the elements ranked.
 
 The ranking algorithm works by ranking the elements based on the fraction: how many times the element is selected / how many times the element has appeared.
 This algorithm requires a reletivly large data set to produce accurate results but over an infinatly large sample, it converges to the actual rankings.
 In the case that there are not enough pairwise comparisons to yeild a unique fraction for each element, the elements with the same fraction appear in random order in the output.
 
