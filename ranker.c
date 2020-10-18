#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>
#include<ctype.h>

FILE *promtFile(char *r_or_w) {
    char *line = NULL;
    size_t nLine;
    int lineLength;
    FILE *inputfile;

    printf("%s\n", "please specify a file:");

    for( ; ; ) {
        printf("%s ", ">");

        lineLength = getline(&line, &nLine, stdin);

        if(lineLength <= 0) {
            printf("no input file found on stdin");
            exit(0);
        }
        else {
            line[lineLength - 1] = '\0'; //set the newline to null so that fopen works properly
            inputfile = fopen(line, r_or_w);
            if(inputfile == NULL)
                printf("%s\n", "could not open that file");
            else {
                return inputfile;
            }
        }
    }
}

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

//passed in an already open file pointer
//returns the number of lines in the file
//does not close the file
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

void swap(float* a, float* b) {
    float t = *a;
    *a = *b;
    *b = t;
}

void swap_strings(char** a, char** b) {
    char *t = *a;
    *a = *b;
    *b = t;
}

int partition (float *arr, char **stringVals, int lowIndex, int highIndex) {
    float pivot = arr[highIndex];    // pivot
    int i = (lowIndex - 1);  // Index of smaller element

    for (int j = lowIndex; j <= highIndex - 1; j++) {
        // If current element is smaller than the pivot
        if (arr[j] > pivot) {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
            swap_strings(&stringVals[i], &stringVals[j]);

        }
    }
    swap(&arr[i + 1], &arr[highIndex]);
    swap_strings(&stringVals[i+1], &stringVals[highIndex]);
    return (i + 1);
}

void quickSort(float *arr, char **stringVals, int lowIndex, int highIndex) {
    if (lowIndex < highIndex) {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int partition_index = partition(arr, stringVals, lowIndex, highIndex);

        // Separately sort elements before
        // partition and after partition
        quickSort(arr, stringVals, lowIndex, partition_index - 1);
        quickSort(arr, stringVals, partition_index + 1, highIndex);
    }
}

void rank(int *selected, int *appeared, char **elements, int elementCount) {
    float *selectedPercent = (float *)malloc(elementCount * sizeof(float));

    for(int i = 0; i < elementCount; i++) {
        selectedPercent[i] = (float)selected[i] / (float)appeared[i];
    }
    quickSort(selectedPercent, elements, 0, elementCount - 1);
    for(int i = 0; i < elementCount; i++) {
        printf("%d) %s\n", i + 1, elements[i]);
    }

}

int main(int argc, char **argv) {

    FILE *inputfile, *saveFile = NULL;

    if(argc == 1) { //has to get a file to rank.
        inputfile = promtFile("r");
    } else if(argc == 3) {
        inputfile = fopen(argv[1], "r");
        if(inputfile == NULL) {
            printf("%s\n", "could not open the input file");
            inputfile = promtFile("r");
        }
        saveFile = fopen(argv[2], "r");
        if(saveFile == NULL) {
            printf("%s\n", "could not find the save file");
            saveFile = promtFile("r");
        }
    } else if(argc == 2) {
        inputfile = fopen(argv[1], "r");
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

    while((lineLength = getline(&line, &nLine, inputfile)) >= 0) {
        if((lineLength == 2 && line[0] == '\r') || lineLength == 1) {
            continue;
        } else {
            line[lineLength - 1] = '\0';
            asprintf(&element, "%s", line);
            elements[pos] = element;
            pos++;
        }
    }
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

    rank(timesSelected, timesAppeared, elements, elementCount);
    int total_trials = 0;
    for(int i = 0; i < elementCount; i++) {
        total_trials += timesAppeared[i];
    }
    total_trials = total_trials/2;
    printf("results for %d trials\n", total_trials);

    return EXIT_SUCCESS;
}







