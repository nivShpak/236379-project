//
// File created by Idan Fischman on 17/02/2021.
// This file is used to set the arguments for the 2-indel ball radius calculator.
// To find out only the largest vector, we recommend setting TWO_MAX_RUN_LENGTH = 1.
// And only unset it if you want to calculate a full histogram.
// VERBOSITY could be 0,1,2 depending how much info to print. VERBOSITY = 1 could be helpful to get the status when running
// with a large vector length (20 or higher for example). VERBOSITY = 2 will show the vector generating the max ball so far, for each thread.
//

#ifndef UNTITLED2_SETTINGS_H
#define UNTITLED2_SETTINGS_H

#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)

// general arguments
#define VECTORS_LENGTH 15
#define NUM_THREADS    1
#define HISTOGRAM_PATH "./"
#define HISTOGRAM_FILE_NAME HISTOGRAM_PATH "2-indel_histogram_n=" STR(VECTORS_LENGTH) ".csv"

// flags that could be used to run the calculator
#define TWO_MAX_RUN_LENGTH 1
#define EXPORT_HISTOGRAM 0
#define PRINT_HISTOGRAM 1
#define HISTOGRAM_BUCKET_SIZE 20
#define VERBOSITY 2

#endif //UNTITLED2_SETTINGS_H
