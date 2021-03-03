# Binary 2-Indel Ball calculator
This calculator is used to find the max 2-indel ball for binary vectors with length n.<br>
The script will iterate over all the vectors of size n, and for each one will calculate it's 2-indel ball size.<br>
At the end, it will print all the vectors that generate the maximum 2-indel ball.<br>
To just run the calculator (supporting only unix):
```
./2-indel.py -n <vector_length>
```
The calculator will take about O(2^n) time to run. For n=26 it could take more than an hour to finish.<br>
To just check the 2-indel ball of a given binary vector (supporting all platforms):
```
./2-indel.py -c <vector>
```

## Calculation Process
the script will check for each vector of length n all the possible 2 deletions and 2 insertions, and will count how many vectors are generated at the process.<br>
we did various optimizations in order to make the script as scalable as possible. It will be explained in a different chapter.

## How To Use
The heart of the calculator is written in C++.
Because some of the optimizations requiere to change the compiled code, we created a python script that will prepare the code according to the parameters given, and then will compile and run it.<br>
The calculator is using multithreaded calculations to speed the performance, and the default number of threads is 57. The decision for this number will be explained in the optimizations part.

In order to run the calculator, you just need to git-clone the repository run the file 2-indel.py, as follows:
```
git clone https://github.com/nivShpak/Yaakombi.git
cd Yaakombi
./2-indel.py -n <vector_length>
```
In order to check the 2-indel ball size of a given vector:
```
git clone https://github.com/nivShpak/Yaakombi.git
cd Yaakombi
./2-indel.py -c <given_vector>
```

### parameters
We provide the following arguments that could be passed to the python script:
```
-h, --help:      will show a short help with the possible arguments.
-v, --verbosity: how much logging to print. Could be 0,1,2,3. Defaults to 0.
                 0: will print only the max vectors
                 1: Will print as well the compilation command, the total time it took to run, and the max vector for each thread.
                 2: Will print as well the progress of the calculation.
                 3: Will print as well for each thread any time it found a vector that is the maximum so far.
-n, --n:         The vectors length. Defaults to 15. Could provide a comma separated list of the length, for example 15,20,25.
-t, --threads:   Number of threads to run. Defaults to 57, Avoid using powers of 2 to improve performance.
-r, --max_run_length: Calculate only vectors with max run length <= of the number provided. 
                      We assume (but couldn't prove) that the max run length is at 2.
-c, --calculate: check the 2-indel ball size of the vectors provided (separated in commas).

Histogram parameters - not recommended for big values of n:
-e, --export_histogram: flag to export the histogram to a file. Defaults to 0 (false). The histogram file name is 2indel_histogram_n<vector_length>.csv 
-p, --print_histogram:  flag to print the histogram. Defaults to 0 (false).
-b, --bucket_size:      histogram bucket size. Does nothing if the calculator is not calculating a histogram. Defaults to 20.

-o, --output: output max vectors to a file csv file. The file name is: max_vectors_n<vectors_size>
```
### examples
```
./2-indel.py -c 0010010010011010101,00000,1010101
```
will calculate the 2 insertions and 2 deletions ball radius of the provided vectors, printing:<br>
0010010010011010101 2-indel ball size is: 8084<br>
00000 2-indel ball size is: 16<br>
1010101 2-indel ball size is: 99<br>
```
./2-indel.py -n 14 -v 1 -e 1 
```
Will calculate the 2-indel balls for vectors in lengths 14, and export ball sizes histogram to a file named 2indel_histogram_n14.csv in the same directory where the script was executed. Also, the script will create a file named max_vectors_n14.csv in the same directory with all the vectors generating the maximum 2-indel ball size.

```
./2-indel.py -n 14 -p 1
```
Will calculate the 2-indel balls for vectors in lengths 14, and print the histogram values to the shell. 
Also, the script will create a file named max_vectors_n14.csv in the same directory with all the vectors generating the maximum 2-indel ball size.

```
./2-indel.py -n 28 -v 2 -o 0
```
Will calculate the 2-indel balls for vectors in lengths 28 (supposed to take a few hours), will print them to the screen, and will not export them to a file. 

## optimizations
To calculate the 2-indel ball of a given vector, the calculator will generate it's 2-deletion ball, and then for each vector in that ball, create its own 2-insertions ball. At the end, the calculator will combine all the insertion balls.<br>
As the numbers of vectors increases exponentially, we did the following optimizations:

1. If the histogram option is turned off (as in the default option), the calculator will try to stop the calculation for a vector in the first time it knows for sure that it will not generate the maximum ball size generated so far.<br>
This will save at least 90% of the work for large values of n (28 or higher), and will cause the histogram output to take way longer.<br>
This check is done in two places:<br>
  a. Before starting the deletions ball.<br>
  b. After calculating the 2-insertions ball for each vector in the 2-deletion ball.

2. We are not trying all the different deletions and insertions as there are many equivalents. The script will try only the different options. Although for each deleted vector we are not trying the same insertions, if a in the 2-indel ball could be generated from two different 2-deletion sources, we will check both of these vectors (but will count it only once).

3. Checking only vectors starting with 0: As the 2-indel ball of a vector n is equal to it's inverse, we could check only vectors starting with 0. For the histogram calculations, we later multiply everything by two so the sum of the bucket sizes will be the same as the number of binary vectors of length n. 

4. Parallelism: we separated the work to different threads. In our naive implementation, each thread got a number (from 0 to NUM_THREADS), and the work was separated using this simplified for loop: 
```
for (int i = thread_number; i < total_vectors; i += NUM_THREADS) { 
    vector = get_binary_representation_with_n_bits(i);
    int size = vector.twoBallSize();
}
``` 
Because of this, if the number of threads is a power of 2, the last bits that each thread check will always be the same. And the work will not split equally. To solve this, we found out that 57 is greater that 32 (the number of cores available in cs servers to use), and will split the work much better. 

5. For every n that we checked, the vectors generating the maximum 2-indel ball did not have runs longer than 2. We could not prove this, but we assume it is true for any n.<br>
Running the calculator with `-r 2` option will speed it significantly, as it will not do any work for the big majority of the vectors.<br>

## contact
For any questions regarding the project, or the code contact us:<br>
Idan Fischman idan-f@campus.technion.ac.il<br>
Niv Shpak     niv.shpak@campus.tachnion.ac.il<rb>
