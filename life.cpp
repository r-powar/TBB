// CPSC 4600 / 5600 - Life
//
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include "tbb/tbb.h"
using namespace tbb;


class Checkgrid{
    int rows;
    int cols;
    int **src;
    
public:
    Checkgrid(int _rows, int _cols, int** _src) :
    rows(_rows), cols(_cols), src(_src){}
    
    void operator()(const blocked_range2d<int> &r) const{
        for(int i = r.rows().begin(); i < r.rows().end(); i++){
            for(int j = r.cols().begin(); j < r.cols().end(); j++){
                 int neighbors = 0;
                for(int a = -1; a <= 1 ; a++){
                    for(int b = -1; b <= 1  ; b++){
                        if(a == 0 && b == 0){
                            continue;
                        }
                        if(i + a >= 0 && i + a < rows && j + b >= 0 && j + b < cols ){
                            if(src[i+a][j +b] == 1){
                                neighbors++;
                            }
                        }
                    }
                }
                if(src[i][j] == 1){
                    if((neighbors < 2) || (neighbors > 3)){
                        src[i][j] = 0;
                    }else{
                        src[i][j] = 1;
                    }
                }else{
                    if(neighbors == 3){
                        src[i][j] = 1;
                    }else{
                        src[i][j] = 0;
                    }
                    
                }
            }
        }
    }
};


int main(int argc, char *argv[])
{
  if (argc != 3) {
    cerr << "Invalid command line - usage: <input file> <number of threads>" << endl;
    exit(-1);
  }

  // Extract parameters
  ifstream ifile(argv[1]);
  int num_threads = atoi(argv[2]);

  // Set the number of threads
  task_scheduler_init init(num_threads);

  // Get the size of the problem - on the first line of the input file.
  int size;
  ifile >> size;
  
  // TODO: Create and initialize data structures
    int rows = size;
    int cols = size;
    int **grid_A = new int*[rows];
    for(int i = 0; i < rows; i++){
        grid_A[i] = new int[cols];
    }
    
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            ifile >> grid_A[i][j];
        }
    }
    
    
  // Start the timer
  tick_count start = tick_count::now();

  // TODO: Execute the parallel algorithm
    for(int i = 0; i < 10; i++){
        parallel_for(blocked_range2d<int>(0, rows, 0, cols), Checkgrid(rows, cols, grid_A));

    }


  // Stop the timer
  tick_count end = tick_count::now();
  double run_time = (end-start).seconds();
  cout << "Time: " << (end-start).seconds() << endl;
   

  // TODO: Print the output to a file
  ofstream outfile("output.txt");
    for(int i =0; i < size; i++){
        for(int j = 0; j < size; j++){
            outfile << grid_A[i][j];
        }
    }

  outfile.close();

   // Append the peformance results to the results file.
  ofstream ofile("life.csv", ios::app);
  ofile << size << "," << num_threads << "," << run_time << endl;
  ofile.close();
    
    for(int i = 0; i < rows; i++){
        delete[] grid_A[i];
    }
    delete []grid_A;

  return 0;
}
