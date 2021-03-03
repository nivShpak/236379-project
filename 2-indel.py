#!/usr/bin/python3

# This file is used to manipulate the settings.h file to run the indel-2 calculator easily.
import argparse
import os
import time


def create_h_file(arguments, vector_length):
    header_text = f'''//
// File created by Idan Fischman on 17/02/2021.
// This file is used to set the arguments for the 2-indel ball radius calculator.
// We do not recommend to edit this file directly, but use the 2-indel.py file to generate it for each run.
//

#ifndef UNTITLED2_SETTINGS_H
#define UNTITLED2_SETTINGS_H

#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)

// general arguments
#define VECTORS_LENGTH {vector_length}
#define NUM_THREADS    {arguments.threads}
#define HISTOGRAM_FILE_NAME "./2indel_histogram_n" STR(VECTORS_LENGTH) ".csv"
#define MAX_VECTORS_FILE "./max_vectors_n" STR(VECTORS_LENGTH) ".csv"
#define VERBOSITY {arguments.verbosity}

// flags to manipulate the type of run and the output
#define MAX_RUN_LENGTH {arguments.max_run_length}
#define EXPORT_HISTOGRAM {arguments.export_histogram}
#define PRINT_HISTOGRAM {arguments.print_histogram}
#define HISTOGRAM_BUCKET_SIZE {arguments.bucket_size}
#define OUTPUT_MAX_VECTORS {arguments.output}
#define TWO_INSERTIONS_BALL_SIZE ((VECTORS_LENGTH)*(VECTORS_LENGTH + 1))/2
#define IS_HISTOGRAM (PRINT_HISTOGRAM || EXPORT_HISTOGRAM)

#endif //UNTITLED2_SETTINGS_H
'''
    return header_text


def print_and_execute(command):
    print(command)
    os.system(command)


def create_main_file(vector):
    file_text = f'''
#include <iostream>
#include <cstring>
#include "vectors.h"

using std::cout;
using std::endl;

int main() {{
    vectors v("{vector}");
    cout << v.get_vector() << " 2-indel ball size is: " << v.twoBallSize() << endl; 
    return 0;
}}
'''
    return file_text


def check_vector(vector, timestamp):
    with open(f"main_{timestamp}.cpp", "w") as file:
        file.write(create_main_file(vector))
    os.system(f"g++ ./main_{timestamp}.cpp ./vectors.cpp -std=c++11 -pthread -O3 -o calculator_{timestamp}")
    os.system(f"./calculator_{timestamp}")
    os.system(f"rm ./calculator_{timestamp}")
    os.system(f"rm ./main_{timestamp}.cpp")


# parse argument to the calculator
parser = argparse.ArgumentParser(description='', formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-v', '--verbosity', default=0, help='how much logging to print. Could be 0,1,2,3. Defaults to 0')
parser.add_argument('-n', '--n', default=15, help='All vector length separated in commas. Defaults to 15')
parser.add_argument('-t', '--threads', default=57, help='Number of threads to run. Defaults to 57, Avoid using powers of 2 to improve performance.')
parser.add_argument('-r', '--max_run_length', default=-1, help='Calculate only vectors with max run length <= of the number provided')
parser.add_argument('-e', '--export_histogram', default=0, help='export histogram to a file')
parser.add_argument('-p', '--print_histogram', default=0, help='print histogram')
parser.add_argument('-b', '--bucket_size', default=20, help='histogram bucket size')
parser.add_argument('-o', '--output', default=1, help='output max vectors to a file')
parser.add_argument('-c', '--calculate', default=None, help='check the 2-indel ball size of the vectors provided (separated in commas)')
args = parser.parse_args()

cur_time = int(time.time())

if (args.calculate):
    for vector in args.calculate.split(","):
        check_vector(vector, cur_time)
    quit()


lengths = args.n.split(',')
for n in lengths:
    # create settings.h file
    settings = create_h_file(args, n)
    with open("settings.h", "w") as f:
        f.write(settings)

    # compile, run the calculator, and remove it
    print_and_execute(f"g++ ./*.cpp -std=c++11 -pthread -O3 -o {n}_calculator_{cur_time}")
    start = time.time()
    print_and_execute(f"./{n}_calculator_{cur_time}")
    end = time.time()

    print(f"calculation for n={n} with {args.threads} threads took {round(end - start, 1)}s\n")
    print_and_execute(f"rm ./{n}_calculator_{cur_time}")
