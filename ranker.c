#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>
#include<ctype.h>

typedef struct {
    char *element;
    int selected;
    int appeared;
} element_t;

/* Function: initElement
 * ---------------------
 * Allocates memory for an element_t and initializes it's values to default
 *
 * returns: a pointer to an initialized element_t or NULL if malloc fails
 */
element_t *initElement() {
    element_t *e = (element_t *)malloc(sizeof(element_t));
    if(e == NULL) {
        fprintf(stderr, "%s\n", "malloc Failed: could not create a\
            new element_t");
        return NULL;
    }
    e->element = NULL;
    e->selected = 0;
    e->appeared = 0;
    return e;
}

/* Function: freeElement
 * ---------------------
 * Frees all memory associated with an element_t pointer
 *
 * e: a pointer to an element struct allocated on the heap
 */
void freeElement(element_t *e) {
    free(e->element);
    free(e);
}


/* Function: promtFile
 * ---------------------
 * Promts a file from stdin until a valid file is received
 *
 * r_or_w: A char * that either holds one of the two values:
 *         "r": specifying the file should be opened in read mode.
 *         "w": specifying the file should be opened in write mode.
 *
 * message: The message printed to standard out before the function
 *          begins promting for lines from stdin.
 *
 * returns: An open FILE pointer for the user inputted file.
 */
FILE *promtFile(char *r_or_w, char *message) {

    //error checking input, user should never input non s or w;
    if(strcmp(r_or_w, "r") != 0 && strcmp(r_or_w, "w") != 0) {
        fprintf(stderr, "%s\n", "invalid argument to promtFile()");
        exit(1);
    }

    char *line = NULL;
    size_t nLine;
    int lineLength;
    FILE *inputfile;

    //print promt message
    printf("%s\n", message);

    //loop until it receives a valid file.
    for( ; ; ) {
        printf("%s ", ">");

        line = NULL;
        lineLength = getline(&line, &nLine, stdin);

        //check for the end of stdin
        if(lineLength <= 0) {
            printf("%s\n", "no file found on stdin");
            free(line);
            exit(0);
        } else {
            //set the newline to null so that fopen works properly
            line[lineLength - 1] = '\0';
            inputfile = fopen(line, r_or_w);
            free(line);
            //check whether fopen succeded
            if(inputfile == NULL)
                printf("%s\n", "could not open that file");
            else
                return inputfile;
        }
    }
}

/* Function: randomAtMost
 * ---------------------
 * Generates a random long below a certain value
 *
 * max: The maximum value possible for the randomly generated long.
 *
 * returns: a random long <= max.
 */
long randomAtMost(long max) {
  unsigned long
    // max <= RAND_MAX < ULONG_MAX.
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  long x;
  do {
   x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x/bin_size;
}

/* Function: countLines
 * ---------------------
 * Counts the number of lines in a file ignoring lines that are blank.
 * Tollerant to newline representations of "\n" and "\r\n"
 *
 * file: An open FILE pointer to the file where the newlines will be counted
 *
 * returns: A integer holding the number of non-empty lines found.
 */
int countLines(FILE *file) {
    char ch, prevch = '\0', secondprevch = '\0';
    int lineCount = 0;

    while((ch = fgetc(file)) != EOF) {
        if(ch == '\n'){
            if(prevch == '\r') { //to be tolerant to carriage retruns
                if(secondprevch != '\n')
                    lineCount++;
            } else if(prevch != '\n') {
                lineCount++;
            }
        }
        secondprevch = prevch;
        prevch = ch;
    }
    rewind(file);
    return lineCount;
}

/* Function: swap
 * ---------------------
 * Swaps two element_t pointers"
 *
 * a: The memory address of the first element_t pointer to be swapped.
 *
 * b: The memory address of the second element_t pointer to be swapped.
 *
 * returns: A integer holding the number of non-empty lines found.
 */
void swap(element_t** a, element_t** b) {
    element_t *t = *a;
    *a = *b;
    *b = t;
}


/* Function: partition
 * ---------------------
 * Utilty function called by quicksort. Performs the partitioning of the array
 *
 * arr: an array of element_t pointers to be sorted.
 *
 * lowIndex: specifies that no elements in indicies lower than this value will
 *           be sorted.
 *
 * highIndex: specifies that no elements in indicies higher than this value
 *            will be sorted.
 *
 * returns: A integer holding the number the index on which the array was
 *          Partitioned.
 */
int partition (element_t **arr, int lowIndex, int highIndex) {
    element_t *pivot = arr[highIndex];    // pivot
    float pivot_fraction = (float) pivot->selected / (float) pivot->appeared;
    int i = (lowIndex - 1);  // Index of smaller element

    for (int j = lowIndex; j <= highIndex - 1; j++) {
        // If current element is smaller than the pivot
        float j_fraction = (float) arr[j]->selected / (float) arr[j]->appeared;
        if (j_fraction > pivot_fraction) {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[highIndex]);
    return (i + 1);
}

/* Function: quickSort
 * ---------------------
 * Recursivley permorms the quick sort algorithm on a subsection of an array
 *
 * arr: an array of element_t pointers to be sorted. The list is modified
 *      in place.
 *
 * lowIndex: specifies that no elements in indicies lower than this value will
 *           be sorted.
 *
 * highIndex: specifies that no elements in indicies higher than this value
 *            will be sorted.
 *
 */
void quickSort(element_t **arr, int lowIndex, int highIndex) {
    if (lowIndex < highIndex) {

        int partition_index = partition(arr, lowIndex, highIndex);

        // Separately sort elements before and after partition
        quickSort(arr, lowIndex, partition_index - 1);
        quickSort(arr, partition_index + 1, highIndex);
    }
}

/* Function: sortElements
 * ---------------------
 * Sorts the elements of an array of element_t pointer
 *
 * e: The array of element pointers to be sorted. e is sorted in place.
 *
 * elementCount: The number of elements in the array, e, to be sorted
 *
 */
void sortElements(element_t **e, int elementCount) {
    quickSort(e, 0, elementCount - 1);
}

/* Function: printElements
 * ---------------------
 * Prints the string representation of the elements in a formatted manor
 *
 * e: The array of element pointers to print.
 *
 * elementCount: The number of elements in the array, e, to be printed
 *
 * printFrequencies: boolen specifying whether or not to print the frequencies
 *                    that the elements were selected and appeared. Yes if true
 *
 * int longestString:  the longest string that will be printed. This value
 *                      affected how the string is formated so that the
 *                      frequencies can appear in the same collumn
 */
void printElements(element_t **e, int elementCount, bool printFrequencies,
                    int longestString) {

    int total_trials = 0;
    for(int i = 0; i < elementCount; i++) {
        total_trials += e[i]->appeared;

        if(printFrequencies) {
            printf("%02d) %-*s %3d/%-3d\n", i + 1, longestString,
                    e[i]->element, e[i]->selected, e[i]->appeared);
        } else {
            printf("%02d) %s\n", i + 1, e[i]->element);
        }

    }
    printf("results over %d trials\n", total_trials/2);
}

/* Function: getFlags
 * ---------------------
 * Searches the begining of the argument vector and creates an array of flags
 * that appear in the command line arguments. Flags appear before file names.
 *
 * argc: The number of command line arguements input to the program.
 *
 * argv: The argument vector input to the program.
 *
 * flags: The array of (char *) pointers in which to store the flags.
 *
 * returns: The number of flags encountered by the function.
 */
int getFlags(int argc, char **argv, char **flags) {
    int flag_counter = 0;
    for(int i = 1; argv[i][0] == '-'; i++) {
        flags[i-1] = argv[i];
        flag_counter++;
    }
    return flag_counter;
}

/* Function: getFiles
 * ---------------------
 * Attempts to open the files passed into the program on the command line. If
 * any attempt is unsuccessful, it will promt for a file from stdin until a
 * valid file is recieved
 *
 * argc: The number of command line arguements input to the program.
 *
 * argv: The argument vector input to the program.
 *
 * nFlags: The number of flags in the argument vector.
 *
 * inputFile: the memory address where the input file gets be stored.
 *
 * saveFile: the memory address where the save file gest stored.
 */
void getFiles(int argc, char **argv, int nFlags, FILE **inputfile,
                FILE **saveFile) {

    char *inputMessage =
        "please enter an input file containing the elements to be ranked";
    char *saveMessage =
        "please enter a save value containing a valid save";
    char *inputErrorMessage =
        "The file you entered for input could not be opened";
    char *saveErrorMessage =
        "The file you entered for save could not be opened";

    if(argc == 1 + nFlags) {
        *inputfile = promtFile("r", inputMessage);
    } else
    if(argc == 2 + nFlags) {
        *inputfile = fopen(argv[nFlags + 1], "r");
        if(*inputfile == NULL) {
            printf("%s\n", inputErrorMessage);
            *inputfile = promtFile("r", inputMessage);
        }
    } else
    if(argc == 3 + nFlags) {
        *inputfile = fopen(argv[nFlags + 1], "r");
        if(inputfile == NULL) {
            printf("%s\n", inputErrorMessage);
            *inputfile = promtFile("r", inputMessage);
        }
        *saveFile = fopen(argv[nFlags + 2], "r");
        if(*saveFile == NULL) {
            printf("%s\n", saveErrorMessage);
            *saveFile = promtFile("r", saveMessage);
        }
    }
}

/* Function: initElementArray
 * ---------------------
 * Allocates memory for and element_t pointer array of specified size,
 * and populates the array with initialized elements.
 *
 * nElements: The number of elements the array should hold.
 *
 * returns: the memory address of the allocted array.
 */
element_t** initElementArray(int nElements) {
    element_t **elements = (element_t **)malloc(nElements*sizeof(element_t *));
    if(elements == NULL) {
        fprintf(stderr, "%s\n", "malloc failed in initElementArray");
        return NULL;
    }
    for(int i = 0; i < nElements; i++) {
        elements[i] = initElement();
        if(elements[i] == NULL) {
            for(int j = 0; j < i+1; j++)
                freeElement(elements[j]);
            free(elements);
            return NULL;
        }
    }
    return elements;
}

/* Function: freeElementArray
 * ---------------------
 * Frees memory allocated for the element_t pointer array and frees all memory
 * associated with each element in the array.
 *
 * e: The element pointer array to be freed.
 *
 * nElements: The number of elements the array holds.
 */
void freeElementArray(element_t **elements, int nElements) {
    for(int i = 0; i < nElements; i++) {
        freeElement(elements[i]);
    }
    free(elements);
}

/* Function: readDataFromSaveFile
 * ---------------------
 * reads data of the form int,int\n from each line of the save file and
 * assigns the fields slected and appeared those respective values for
 * the structs in an element array
 *
 * elements: the array of element pointers to where the data will be read
 *
 * nElements: the number of elements in the element array
 *
 * saveFile: the file from which to read the data
 *
 * returns: true on success, false on failure. fails if the save file is of
 *          invalid size for the number of elements.
 */
bool readDataFromSaveFile(element_t **elements, int nElements, FILE *saveFile){
    char *line = NULL;
    size_t nLine;
    int lineLength, nLines;

    nLines = countLines(saveFile);
    if(nLines != nElements)
        return false;
    nLines = 0;

    while(( lineLength = getline(&line, &nLine, saveFile)) != -1 &&
            nLines < nElements) {

        line[lineLength - 1] = '\0';
        elements[nLines]->selected = atoi(strtok(line, "/"));
        elements[nLines]->appeared = atoi(strtok(NULL, "/"));

        nLines++;
    }
    free(line);
    return true;
}

/* Function: readDataFromInputFile
 * ---------------------
 * reads data from an input file where each element appears on it's own
 * line. Assigns the char * read for the line to the element field in each
 * element_t in the element array.
 *
 * elements: the array of element pointers to where the data will be read
 *
 * nElements: the number of elements in the element array
 *
 * file: the file from which to read the data
 *
 * returns: the length of longest string read from the inputfile
 */
int readDataFromInputFile(element_t **elements, int nElements, FILE *file) {
    char *element_string, *line = NULL;
    size_t nLine;
    int lineLength, longestString = 0, pos = 0;

    while((lineLength = getline(&line, &nLine, file)) >= 0) {
        if((lineLength == 2 && line[0] == '\r') || lineLength == 1) {
            continue;
        } else {
            if(lineLength >= longestString)
                longestString = lineLength;

            line[lineLength - 1] = '\0';
            if(line[lineLength - 2] == '\r')
                line[lineLength - 2] = '\0';

            asprintf(&element_string, "%s", line);
            elements[pos]->element = element_string;
            pos++;
        }
    }
    return longestString;
}


/* Function: parseFalgs
 * ---------------------
 * searches the elements of an array of flags and assigns the boolean values of
 * fFlag and aFlag to true if "-f" and/or -"a" appear respectivly.
 *
 * flags: the array of character pointers to where the flags are stored
 *
 * nFlags: the number of flags in the flags array
 *
 * fFlag: the memory address where the boolean indicating the presence of a "-f"
 *        flag is stored.
 *
 * aFlag: the memory address where the boolean indicating the presence of a "-a"
 *        flag is stored.
 */
void parseFlags(char** flags, int nFlags, bool *fFlag, bool *aFlag) {
    for(int i = 0; i < nFlags; i++) {
        switch(flags[i][1]) {
            case 'f':
                *fFlag = true;
                break;
            case 'a':
                *aFlag = true;
                break;
            default:
                fprintf(stderr, "invalid flag %s used\n", flags[i]);
                exit(1);
        }
    }
}

/* Function: generateOptions
 * ---------------------
 * Generates two unique random numbers at most nElements
 *
 * choice1: the memory address where one of the random numbers is stored
 *
 * choice2: the memory address where the other random number is stored
 */
void generateOptions(int nElements, int *choice1, int *choice2) {

    *choice1 = randomAtMost(nElements - 1);
    while ((*choice2 = randomAtMost(nElements - 1)) == *choice1) {
        ;
    }
}

/* Function: printOptions
 * ---------------------
 * Prints the string representation of the elements at 2 specifed indicies
 * of an element array. The choices for the user at every interation of
 * the main program.
 *
 * elements: The element_t * array from which to print the options
 *
 * choice1: the index of one element in the element array to be printed
 *
 * choice2: the index of the other element in the element array to be printed
 */
void printOptions(element_t **elements, int choice1, int choice2) {
    printf("%s %s\n", "1) ", elements[choice1]->element);
    printf("%s %s\n", "2) ", elements[choice2]->element);
}

/* Function: writeDataToSaveFile
 * ---------------------
 * writes the current ranking data to a save file in the from int1,int2\n where
 * every line represents and element, int1 is the number of times the element
 * was selected, and int2 in the number of times the element appeared
 *
 * elements: The element_t * array to store
 *
 * nElements: The number of elements in the elemtent array
 *
 * file: the open file pointer in which to save the results
 */
void writeDataToSaveFile(element_t **elements, int nElements, FILE *file) {
    for(int i = 0; i < nElements; i++){
        fprintf(file, "%d/%d\n", elements[i]->selected, elements[i]->appeared);
    }
}

/* Function: getValidResponse
 * ---------------------
 * promts single character responses from stdin until is recieves a valid
 * character.
 *
 * characterSet: The set of valid characters for the current promt
 *
 * returns: the valid character recieved
 */
char getValidResponse(char *characterSet) {
    char *line = NULL;
    size_t nLine;
    int lineLength;

    while(printf("> ") && (lineLength = getline(&line, &nLine, stdin)) != -1) {
        if(lineLength != 2) {
            continue;
        }

        for(int i = 0; characterSet[i] != '\0'; i++) {
            if(characterSet[i] == line[0]){
                char ret = line[0];
                free(line);
                return ret;
            }
        }
    }
    return '\0';
}

int main(int argc, char **argv) {

    FILE *inputfile = NULL, *saveFile = NULL;
    char inputCharacterSet[] = {'1','2','q','s','\0'};
    char quitCharacterSet[] = {'y','n','\0'};
    char *flags[argc];
    bool fFlag = false, aFlag = false, isReadSuccess;
    int nElements, nFlags, longestString, counter = 0;
    element_t **elements;

    nFlags = getFlags(argc, argv, flags);
    parseFlags(flags, nFlags, &fFlag, &aFlag);
    getFiles(argc, argv, nFlags, &inputfile, &saveFile);

    nElements = countLines(inputfile);

    //program exits if initElementArray fails
    elements = initElementArray(nElements);

    if(saveFile != NULL) {
        isReadSuccess = readDataFromSaveFile(elements, nElements, saveFile);
        //if failure we free memeory and exit
        if(!isReadSuccess) {
            freeElementArray(elements, nElements);
            fprintf(stderr, "invalid number of elements in save file\
-- expexed: %d lines\n", nElements);
            return EXIT_FAILURE;
        }
        fclose(saveFile);
    }

    longestString = readDataFromInputFile(elements, nElements, inputfile);
    fclose(inputfile);

     //initialize the random seed using system time
    srand(time(0));

    int choice1, choice2;
    bool running = true;

    while(running) {
        //autosave every ten times if the -a flag is specified
        if(aFlag){
            if(counter % 10 == 9) {
                printf("%s\n", "autosaving . . .");
                saveFile = fopen(argv[nFlags + 2], "w");
                writeDataToSaveFile(elements, nElements, saveFile);
                fclose(saveFile);
            }
        }

        generateOptions(nElements, &choice1, &choice2);
        printOptions(elements, choice1, choice2);

        char input = getValidResponse(inputCharacterSet);

        char *saveMessage =
            "please input the file where you'd like to save the results";
        char *quitMessage =
            "Would you like to save the results of your current session?[y/n]";

        switch(input){
            case '1':
                elements[choice1]->selected++;
                break;
            case '2':
                elements[choice2]->selected++;
                break;
            case 's':
                saveFile = promtFile("w", saveMessage);
                writeDataToSaveFile(elements, nElements, saveFile);
                fclose(saveFile);
                break;
            case 'q':
                running = false;
                printf("%s\n", quitMessage);
                char toSave = getValidResponse(quitCharacterSet);
                if(toSave == 'y') {
                    saveFile = promtFile("w", saveMessage);
                    writeDataToSaveFile(elements, nElements, saveFile);
                    fclose(saveFile);
                }
                break;
        }
        //we don't want to increment if the user did not rank.
        if(input == 'q' || input == 's') {
            elements[choice1]->appeared++;
            elements[choice2]->appeared++;
        }
        counter++;
    }

    //once we stop ranking perfrom the following:
    sortElements(elements, nElements);
    printElements(elements, nElements, fFlag, longestString);

    freeElementArray(elements, nElements);

    return EXIT_SUCCESS;
}







