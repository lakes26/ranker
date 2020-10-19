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

element_t *initElement() {
    element_t *e = (element_t *)malloc(sizeof(element_t));
    e->element = NULL;
    e->selected = 0;
    e->appeared = 0;
    return e;
}

void freeElement(element_t *e) {
    free(e->element);
    free(e);
}

/*
Description:
    promts files from stdin until it receives a valid file
Args:
    char *r_or_w, a string of length 1 that is either "r" of "w"
    specifying weather the file is to be opened in read mode or write mode
Returns:
    An open file pointer to the user inputed file */
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
            printf("no file found on stdin");
            free(line);
            exit(0);
        } else {
            line[lineLength - 1] = '\0'; //set the newline to null so that fopen works properly
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

/*
Description:
    Produces a random long in a specified range
Args:
    long max - the maximum value that the function can return.
Return:
    a random value within that range */
long random_at_most(long max) {
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

/*
Description:
    counts the number of lines in a file.
Args:
    FILE *file - the file from which to have newlines counted
Return:
    int - number of newlines found */
int countLines(FILE *file) {
    char ch;
    char prevch = '\0';
    char secondprevch = '\0';
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

/*
Description:
    Swaps two float elements.
Args:
    float *a, memory address of float to be swapped
    float *b, memory address of other float to be swapped.
Return:
    void */
void swap(element_t** a, element_t** b) {
    element_t *t = *a;
    *a = *b;
    *b = t;
}


/*
Description:
    Utility for quicksort - determines the array index on which to partition quicksort.
    swaps all arrays to appropriate sorted posistions.
    strings and other integers are sorted based on the float in the associated index of arr.
    last element is used as the splitter.
Args:
    float *arr - array of float elements to be partitioned.
    char **stringVals - array of strings associated with the float *arr.
    int *selected - array of ints associated with the amount of times the elements were selected
    int *appeared - array of ints associated with the amount of times the elements appeared
    int lowIndex - lower bound index for sort;
    int highIndex - upper bound index for sort;
Return:
    returns the array index for quicksort partition */
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

/*
Description:
    Recursively performs the quicksort algorithm
Args:
    float *arr - array of float elements to be partitioned.
    char **stringVals - array of strings associated with the float *arr.
    int lowIndex - lower bound index for sort;
    int highIndex - upper bound index for sort;
Return:
    void;
    *arr and *stringVals are modified in place; the float array is sorted by value and
    the an equivilant spaw for the string array is performed every time a float is sawpped
    i.e. the strings are sorted based on the values of the floats in thier associated index */
void quickSort(element_t **arr, int lowIndex, int highIndex) {
    if (lowIndex < highIndex) {

        int partition_index = partition(arr, lowIndex, highIndex);

        // Separately sort elements before and after partition
        quickSort(arr, lowIndex, partition_index - 1);
        quickSort(arr, partition_index + 1, highIndex);
    }
}

/*
Description:
    Creates the ranked_list and prints it to standard out.
Args:
    int *selected - array containing number of times elements were selected
    int *appeared - array containing number of times elements appered
    int elementCount - the number of total elements.
Return:
    void; */

void sortElements(element_t **e, int elementCount) {
    quickSort(e, 0, elementCount - 1);
}

void printElements(element_t **e, int elementCount, bool print_frequencies, int longest_string) {

    int total_trials = 0;
    for(int i = 0; i < elementCount; i++) {
        total_trials += e[i]->appeared;
        if(print_frequencies) {
            printf("%02d) %-*s %3d/%-3d\n", i + 1, longest_string, e[i]->element, e[i]->selected, e[i]->appeared);
        } else {
            printf("%02d) %s\n", i + 1, e[i]->element);
        }
    }
    printf("results over %d trials\n", total_trials/2);
}

int getFlags(int argc, char **argv, char **flags) {
    int flag_counter = 0;
    for(int i = 1; argv[i][0] == '-'; i++) {
        flags[i-1] = argv[i];
        flag_counter++;
    }
    return flag_counter;
}

void getFiles(int argc, char **argv, int nFlags, FILE **inputfile, FILE **saveFile) {
    if(argc == 1 + nFlags) { //has to get a file to rank.
        *inputfile = promtFile("r", "please enter an input file containing the elements to be ranked");
    } else
    if(argc == 2 + nFlags) {
        *inputfile = fopen(argv[nFlags + 1], "r");
        if(*inputfile == NULL) {
            printf("could not open that file");
            *inputfile = promtFile("r", "please enter an input file containing the elements to be ranked");
        }
    } else
    if(argc == 3 + nFlags) {
        *inputfile = fopen(argv[nFlags + 1], "r");
        if(inputfile == NULL) {
            printf("%s\n", "The file you entered for input could not be opened:\n");
            *inputfile = promtFile("r", "please enter an input file containing the elements to be ranked");
        }
        *saveFile = fopen(argv[nFlags + 2], "r");
        if(*saveFile == NULL) {
            printf("%s\n", "the file you entered for save could not be opened:\n");
            *saveFile = promtFile("r", "please enter a save value containing a valid save");
        }
    }
}

element_t** initElementArray(int nElements) {
    element_t **elements = (element_t **)malloc(nElements * sizeof(element_t *));
    for(int i = 0; i < nElements; i++) {
        elements[i] = initElement();
    }
    return elements;
}

void freeElementArray(element_t **elements, int nElements) {
    for(int i = 0; i < nElements; i++) {
        freeElement(elements[i]);
    }
    free(elements);
}

bool readDataFromSaveFile(element_t **elements, int nElements, FILE *saveFile) {
    char *line = NULL;
    size_t nLine;
    int lineLength, nLines;

    nLines = countLines(saveFile);
    if(nLines != nElements)
        return false;
    nLines = 0;

    while((lineLength = getline(&line, &nLine, saveFile)) != -1 && nLines < nElements) {
        line[lineLength - 1] = '\0';
        elements[nLines]->selected = atoi(strtok(line, "/"));
        elements[nLines]->appeared = atoi(strtok(NULL, "/"));

        nLines++;
    }
    free(line);
    return true;
}

int readDataFromInputFile(element_t **elements, int nElements, FILE *file) {
    char *element_string, *line = NULL;
    size_t nLine;
    int lineLength, longest_string = 0, pos = 0;

    while((lineLength = getline(&line, &nLine, file)) >= 0) {
        if((lineLength == 2 && line[0] == '\r') || lineLength == 1) {
            continue;
        } else {
            if(lineLength >= longest_string)
                longest_string = lineLength;

            line[lineLength - 1] = '\0';
            if(line[lineLength - 2] == '\r')
                line[lineLength - 2] = '\0';

            asprintf(&element_string, "%s", line);
            elements[pos]->element = element_string;
            pos++;
        }
    }
    return longest_string;
}

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

void generateOptions(element_t **elements, int nElements, int *choice1, int *choice2) {
    *choice1 = random_at_most(nElements - 1);
    while ((*choice2 = random_at_most(nElements - 1)) == *choice1) {
        ;
    }
}

void printOptions(element_t **elements, int choice1, int choice2) {
    printf("%s %s\n", "1) ", elements[choice1]->element);
    printf("%s %s\n", "2) ", elements[choice2]->element);
}

void writeDataToSaveFile(element_t **elements, int nElements, FILE *file) {
    for(int i = 0; i < nElements; i++){
        fprintf(file, "%d/%d\n", elements[i]->selected, elements[i]->appeared);
    }
}

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
    int nElements, nFlags, longest_string, counter = 0;
    element_t **elements;

    nFlags = getFlags(argc, argv, flags);
    parseFlags(flags, nFlags, &fFlag, &aFlag);
    getFiles(argc, argv, nFlags, &inputfile, &saveFile);

    nElements = countLines(inputfile);
    elements = initElementArray(nElements);

    if(saveFile != NULL) {
        isReadSuccess = readDataFromSaveFile(elements, nElements, saveFile);
        if(!isReadSuccess) {
            freeElementArray(elements, nElements);
            fprintf(stderr, "invalid number of elements in save file -- expexed: %d lines\n", nElements);
            return EXIT_FAILURE;
        }
        fclose(saveFile);
    }

    longest_string = readDataFromInputFile(elements, nElements, inputfile);
    fclose(inputfile);

    srand(time(0)); //initialize the random seed

    int choice1, choice2;
    bool running = true;

    while(running) {
        if(aFlag){
            if(counter % 10 == 9) {
                printf("%s\n", "autosaving . . .");
                saveFile = fopen(argv[nFlags + 2], "w");
                writeDataToSaveFile(elements, nElements, saveFile);
                fclose(saveFile);
            }
        }

        generateOptions(elements, nElements, &choice1, &choice2);
        printOptions(elements, choice1, choice2);

        char input = getValidResponse(inputCharacterSet);

        switch(input){
            case '1':
                elements[choice1]->selected++;
                break;
            case '2':
                elements[choice2]->selected++;
                break;
            case 's':
                saveFile = promtFile("w", "please input the file where you'd like to save the results");
                writeDataToSaveFile(elements, nElements, saveFile);
                fclose(saveFile);
                break;
            case 'q':
                running = false;
                printf("Would you like to save the results of your current session? [y/n]\n");
                char toSave = getValidResponse(quitCharacterSet);
                if(toSave == 'y') {
                    saveFile = promtFile("w", "please input the file where you'd like to save the results");
                    writeDataToSaveFile(elements, nElements, saveFile);
                    fclose(saveFile);
                }
                break;
        }
        if(running) {
            elements[choice1]->appeared++;
            elements[choice2]->appeared++;
        }
        counter++;
    }

    sortElements(elements, nElements);
    printElements(elements, nElements, fFlag, longest_string);

    freeElementArray(elements, nElements);

    return EXIT_SUCCESS;
}







