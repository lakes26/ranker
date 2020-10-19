#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>
#include<ctype.h>


/*
Description:
    promts files from stdin until it receives a valid file
Args:
    char *r_or_w, a string of length 1 that is either "r" of "w"
    specifying weather the file is to be opened in read mode or write mode
Returns:
    An open file pointer to the user inputed file */
FILE *promtFile(char *r_or_w) {
    //declare promt message;
    char *message = "please specify a file";

    //error checking input;
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
void swap(float* a, float* b) {
    float t = *a;
    *a = *b;
    *b = t;
}

/*
Description:
    Swaps two string elements.
Args:
    char **a, memory address of string to be swapped
    char **b, memory address of other string to be swapped.
Return:
    void */
void swap_strings(char** a, char** b) {
    char *t = *a;
    *a = *b;
    *b = t;
}

void swap_ints(int *a, int *b) {
    int t = *a;
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
int partition (float *arr, char **stringVals, int *selected, int *appeared, int lowIndex, int highIndex) {
    float pivot = arr[highIndex];    // pivot
    int i = (lowIndex - 1);  // Index of smaller element

    for (int j = lowIndex; j <= highIndex - 1; j++) {
        // If current element is smaller than the pivot
        if (arr[j] > pivot) {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
            swap_ints(&selected[i], &selected[j]);
            swap_ints(&appeared[i], &appeared[j]);
            swap_strings(&stringVals[i], &stringVals[j]);
        }
    }
    swap(&arr[i + 1], &arr[highIndex]);
    swap_ints(&selected[i + 1], &selected[highIndex]);
    swap_ints(&appeared[i + 1], &appeared[highIndex]);
    swap_strings(&stringVals[i+1], &stringVals[highIndex]);
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
void quickSort(float *arr, char **stringVals, int *selected, int *appeared, int lowIndex, int highIndex) {
    if (lowIndex < highIndex) {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int partition_index = partition(arr, stringVals, selected, appeared, lowIndex, highIndex);

        // Separately sort elements before
        // partition and after partition
        quickSort(arr, stringVals, selected, appeared, lowIndex, partition_index - 1);
        quickSort(arr, stringVals, selected, appeared, partition_index + 1, highIndex);
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

void rank(int *selected, int *appeared, char **elements, int elementCount, bool print_frequencies, int longest_string) {
    float selectedPercent[elementCount];

    for(int i = 0; i < elementCount; i++) {
        selectedPercent[i] = (float)selected[i] / (float)appeared[i];
    }
    quickSort(selectedPercent, elements, selected, appeared, 0, elementCount - 1);

    for(int i = 0; i < elementCount; i++) {
        if(print_frequencies) {
            printf("%2d) %-*s %d/%d\n", i + 1, longest_string, elements[i], selected[i], appeared[i]);
        } else {
            printf("%2d) %s\n", i + 1, elements[i]);
        }
    }
}


int main(int argc, char **argv) {

    FILE *inputfile, *saveFile = NULL;
    int first_file = 1;
    bool print_frequencies = false;

    if(argc == 1) { //has to get a file to rank.
        inputfile = promtFile("r");
    } else if(strcmp(argv[1], "-f") == 0) {
        first_file = 2;
        print_frequencies = true;
    }

    if(argc == first_file + 2) {
        inputfile = fopen(argv[first_file], "r");
        if(inputfile == NULL) {
            printf("%s\n", "could not open the input file");
            inputfile = promtFile("r");
        }
        saveFile = fopen(argv[first_file + 1], "r");
        if(saveFile == NULL) {
            printf("%s\n", "could not find the save file");
            saveFile = promtFile("r");
        }
    } else if(argc == first_file + 1) {
        inputfile = fopen(argv[first_file], "r");
        if(inputfile == NULL) {
            printf("could not open that file");
            inputfile = promtFile("r");
        }
    } else {
        fprintf(stderr, "%s\n", "invalid number of command line arguments");
    }

    int elementCount = countLines(inputfile);

    char **elements = (char **)malloc(elementCount * sizeof(char *));
    int *timesAppeared = (int *)malloc(elementCount * sizeof(int));
    int *timesSelected = (int *)malloc(elementCount * sizeof(int));

    char *line = NULL;
    size_t nLine;
    int lineLength;

    if(saveFile != NULL) {
        int numLines = 0;
        char *selected, *appeared;
        while((lineLength = getline(&line, &nLine, saveFile)) != -1 && numLines < elementCount) {
            line[lineLength - 1] = '\0';
            selected = strtok(line, "/");
            timesSelected[numLines] = atoi(selected);
            appeared = strtok(NULL, "/");
            timesAppeared[numLines] = atoi(appeared);
            numLines++;
        }
        if(numLines != elementCount){
            fprintf(stderr, "%s\n", "invalid save file");
            exit(1);
        }
        fclose(saveFile);
    } else {
        for(int i = 0; i < elementCount; i++) {
            timesAppeared[i] = 0;
            timesSelected[i] = 0;
        }
    }

    if(!elements || !timesAppeared || !timesSelected) {
        fprintf(stderr, "%s\n", "malloc failed");
        exit(1);
    }

    rewind(inputfile);

    int pos = 0;
    char *element;

    int longest_string = 0; //used for formatted printing
    while((lineLength = getline(&line, &nLine, inputfile)) >= 0) {
        if((lineLength == 2 && line[0] == '\r') || lineLength == 1) {
            continue;
        } else {
            if(lineLength >= longest_string)
                longest_string = lineLength;
            line[lineLength - 1] = '\0';
            if(line[lineLength - 2] == '\r')
                line[lineLength - 2] = '\0';
            asprintf(&element, "%s", line);
            elements[pos] = element;
            pos++;
        }
    }
    fclose(inputfile);
    srand(time(0)); //initialize the random seed

    int choice1, choice2;
    for( ; ; ) {
        choice1 = random_at_most(elementCount - 1);
        choice2 = random_at_most(elementCount - 1);
        bool running = true;

        if(choice1 == choice2) {
            continue;
        }

        printf("%s %s\n", "1)", elements[choice1]);
        printf("%s %s\n", "2)", elements[choice2]);
        printf("> ");

        while((lineLength = getline(&line, &nLine, stdin)) != -1) {
            line[lineLength - 1] = '\0';
            if(lineLength != 2) {
                printf("> ");
                continue;
            }

            if(line[0] == '1'){
                timesSelected[choice1]++;
                break;
            }

            else if(line[0] == '2'){
                timesSelected[choice2]++;
                break;
            }

            else if(line[0] == 'q') {
                running = false;
                printf("%s\n", "do you want to save? y/n");
                while((lineLength = getline(&line, &nLine, stdin)) != -1) {
                    if(lineLength != 2)
                        continue;
                    if(line[0] == 'y') {
                        saveFile = promtFile("w");
                        for(int i = 0; i < elementCount; i++){
                            fprintf(saveFile, "%d/%d\n", timesSelected[i], timesAppeared[i]);
                        }
                        break;
                    } else if(line[0] == 'n') {
                        break;
                    } else {
                        printf("%s\n", "please enter a valid response");
                    }
                }
                break;
            }

            else if(line[0] == 's') {
                saveFile = promtFile("w");
                for(int i = 0; i < elementCount; i++){
                    fprintf(saveFile, "%d/%d\n", timesSelected[i], timesAppeared[i]);
                }
                fclose(saveFile);
                break;
            }

            printf("> ");
        }
        if(!running) {
            break;
        }


        timesAppeared[choice1]++;
        timesAppeared[choice2]++;
    }

    //
    rank(timesSelected, timesAppeared, elements, elementCount, print_frequencies, longest_string);
    int total_trials = 0;
    for(int i = 0; i < elementCount; i++) {
        total_trials += timesAppeared[i];
    }
    total_trials = total_trials/2;
    printf("results for %d trials\n", total_trials);

    //free all memeory
    free(line);
    for(int i = 0; i < elementCount; i++) {
        free(elements[i]);
    }
    free(elements);
    free(timesAppeared);
    free(timesSelected);

    return EXIT_SUCCESS;
}







