//
// File created by Idan Fischman on 17/02/2021.
// This file is used to set the arguments for the 2-indel ball radius calculator.
// We do not recommend to edit this file directly, but use the 2-indel.py file to generate it for each run.
//

#ifndef UNTITLED2_SETTINGS_H
#define UNTITLED2_SETTINGS_H

#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)

// general arguments
#define VECTORS_LENGTH 18
#define NUM_THREADS    57
#define HISTOGRAM_FILE_NAME "./2indel_histogram_n" STR(VECTORS_LENGTH) ".csv"
#define MAX_VECTORS_FILE "./max_vectors_n" STR(VECTORS_LENGTH) ".csv"
#define VERBOSITY 0

// flags to manipulate the type of run and the output
#define MAX_RUN_LENGTH -1
#define EXPORT_HISTOGRAM 0
#define PRINT_HISTOGRAM 0
#define HISTOGRAM_BUCKET_SIZE 20
#define OUTPUT_MAX_VECTORS 1
#define TWO_INSERTIONS_BALL_SIZE ((VECTORS_LENGTH)*(VECTORS_LENGTH + 1))/2
#define IS_HISTOGRAM (PRINT_HISTOGRAM || EXPORT_HISTOGRAM)

#endif //UNTITLED2_SETTINGS_H
