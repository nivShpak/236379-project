#!/usr/bin/python3

# This file is used to manipulate the settings.h file to run the indel-2 calculator easily.
import argparse
import os
import time


def create_h_file(arguments, vector_length):
    header_text = f'''//
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
#define VECTORS_LENGTH {vector_length}
#define NUM_THREADS    {arguments.threads}
#define HISTOGRAM_FILE_NAME "./2-indel_histogram_n=" STR(VECTORS_LENGTH) ".csv"
#define MAX_VECTORS_FILE "./max_vecrtors_n=" STR(VECTORS_LENGTH) ".csv"
#define VERBOSITY {arguments.verbosity}

// flags to manipulate the type of run and the output
#define TWO_MAX_RUN_LENGTH {arguments.quick}
#define EXPORT_HISTOGRAM {arguments.export_histogram}
#define PRINT_HISTOGRAM {arguments.print_histogram}
#define HISTOGRAM_BUCKET_SIZE {arguments.bucket_size}
#define OUTPUT_MAX_VECTORS {arguments.output}

#endif //UNTITLED2_SETTINGS_H
'''
    return header_text


def print_and_execute(command):
    print(command)
    os.system(command)


# parse argument to the calculator
parser = argparse.ArgumentParser(description='', formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-v', '--verbosity', default=0, help='how much logging to print. Could be 0,1,2. Defaults to 0')
parser.add_argument('-n', '--n', default=15, help='All vector length separated in commas. Defaults to 15')
parser.add_argument('-t', '--threads', default=32, help='Number of threads to run. Defaults to 32')
parser.add_argument('-q', '--quick', default=1, help='Calculate only vectors with max run length <= 2')
parser.add_argument('-e', '--export_histogram', default=0, help='export histogram to a file')
parser.add_argument('-p', '--print_histogram', default=0, help='print histogram')
parser.add_argument('-b', '--bucket_size', default=20, help='histogram bucket size')
parser.add_argument('-o', '--output', default=1, help='output max vectors to a file')
args = parser.parse_args()

lengths = args.n.split(',')
for n in lengths:
    # create settings.h file
    settings = create_h_file(args, n)
    with open("settings.h", "w") as f:
        f.write(settings)

    # compile, run the calculator, and remove it
    print_and_execute(f"g++ ./*.cpp -std=c++11 -pthread -O3 -o {n}_calculator")
    start = time.time()
    print_and_execute(f"./{n}_calculator")
    end = time.time()

    print(f"calculation for n={n} with {args.threads} threads took {round(end - start, 1)}s\n")
    print_and_execute(f"rm ./{n}_calculator")
