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
#define VECTORS_LENGTH 19
#define NUM_THREADS    32
#define HISTOGRAM_FILE_NAME "./2-indel_histogram_n=" STR(VECTORS_LENGTH) ".csv"
#define MAX_VECTORS_FILE "./max_vecrtors_n=" STR(VECTORS_LENGTH) ".csv"
#define VERBOSITY 0

// flags to manipulate the type of run and the output
#define TWO_MAX_RUN_LENGTH 1
#define EXPORT_HISTOGRAM 0
#define PRINT_HISTOGRAM 0
#define HISTOGRAM_BUCKET_SIZE 20
#define OUTPUT_MAX_VECTORS 1

#endif //UNTITLED2_SETTINGS_H
