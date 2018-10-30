#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>

#include "hmm.h"

#define FILE_LENGTH 64
using namespace std;

void count_test_file_rc(char *test_file, int &row, int &col);
int** get2Darray_int(int row, int col);
double** get2Darray_double(int row, int col);
void delete_2Darray(double** arr, int row, int col);
void test_to_observ(char *test_file, int row, int col, int** arr);
void Viterbi(HMM *hmms,int state, int timestamp, int sample, int** observ, double &max_likelihood, int &max_p_model, double** delta, int m);


int main(int argc, char *argv[]){
  char model_list[FILE_LENGTH];
  char test_file[FILE_LENGTH];
  char dump_file[FILE_LENGTH];
  strcpy(model_list, argv[1]);
  strcpy(test_file, argv[2]);
  strcpy(dump_file, argv[3]);

  HMM hmms[5];
  load_models(model_list, hmms, 5);

  int sample=0, timestamp=0,state=hmms[1].state_num;
  count_test_file_rc(test_file,sample,timestamp);

  int** observ=get2Darray_int(sample,timestamp);
  test_to_observ(test_file, sample, timestamp, observ);

  FILE *fp=fopen(dump_file, "wb");
  for(int n=0; n<sample; n++){
    double max_likelihood=0.;
    int max_p_model=-1;
    for(int m=0;m<5;m++){
      double** delta=get2Darray_double(state,timestamp);
      Viterbi(hmms,state,timestamp,n,observ,max_likelihood,max_p_model,delta,m);
      delete_2Darray(delta,state,timestamp);
    }
    //Viterbi(hmms,state,timestamp,n,observ,max_likelihood,max_p_model);
    fprintf(fp, "model_0%d.txt %e\n", max_p_model + 1, max_likelihood);
  }
  fclose(fp);

  return 0;
}

void Viterbi(HMM *hmms,int state, int timestamp, int sample, int** observ, double &max_likelihood, int &max_p_model, double** delta, int m){

  //for(int m=0;m<5;m++){
    //double delta[state][timestamp]={0.};
    //double delta[6][50]={0.};
    //initialize
    for(int i=0;i<state;i++){
      delta[i][0] = hmms[m].initial[i] * hmms[m].observation[observ[sample][0]][i];
    }
    //recursion
    for(int t=1;t<timestamp;t++){
      for(int j=0;j<state;j++){
        for(int i=0;i<state;i++){
          double max_delta= delta[i][t - 1] * hmms[m].transition[i][j];
          if(max_delta>delta[j][t]){
            delta[j][t]=max_delta;
          }
        }
        delta[j][t] *= hmms[m].observation[observ[sample][t]][j];
      }
    }
    //termination
    double max_p=0.;
    for(int i=0;i<state;i++){
      if(delta[i][timestamp-1]>max_p){
        max_p=delta[i][timestamp-1];
      }
    }
    if(max_p>max_likelihood){
      max_likelihood=max_p;
      max_p_model=m;
    }
  //}
}

void count_test_file_rc(char *test_file, int &row, int &col){
  FILE *fp = fopen(test_file, "r");
  int r=0;
  int c=0;
  char ch;

  while(!feof(fp)){
    ch=fgetc(fp);
    if(ch=='\n'){
      r++;
      col=c;
      c=0;
    }
    else{
      c++;
    }
  }
  row=r;
  fclose(fp);
}


int** get2Darray_int(int row, int col){
  int** arr=new int*[row];
  for(int i=0;i<row;i++){
    arr[i]=new int[col];
  }
  return arr;
}

void test_to_observ(char *test_file, int row, int col, int** arr){
  FILE *fp=fopen(test_file, "r");
  for(int i=0; i<row; i++){
    char observ_str[col];
    fscanf(fp,"%s",observ_str);
    for(int j=0;j<col;j++){
      arr[i][j]=observ_str[j]-'A';
    }
  }
  fclose(fp);
}

double** get2Darray_double(int row, int col)
{
	double** arr = new double*[row];
	for(int i=0; i<row; i++){
		arr[i] = new double[col];
		for(int j=0; j<col; j++){
			arr[i][j]={0};
		}
	}
	return arr;
}

void delete_2Darray(double** arr, int row, int col)
{
	for(int i=0; i<row; i++)
	{
		delete[] arr[i];
	}
	delete[] arr;
}

