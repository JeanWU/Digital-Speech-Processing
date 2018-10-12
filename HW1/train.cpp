#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "hmm.h"

#define FILE_LENGTH 64
using namespace std;


void count_train_file_rc(char *train_file, int &row, int &col);
int** get2Darray_int(int row, int col);
double** get2Darray_double(int row, int col);
void train_to_observ(char *train_file, int row, int col, int** arr);


int main(int argc, char *argv[]){
	int iteration = atoi(argv[1]);  //convert string to int, iteration number
	char init_file[FILE_LENGTH+1], //model_init.txt
	     train_file[FILE_LENGTH+1], //seq_model_01-05.txt
	     dump_file[FILE_LENGTH+1];  //MODEL_01-05.TXT

	strcpy(init_file, argv[2]);
	strcpy(train_file, argv[3]);
	strcpy(dump_file, argv[4]);

	HMM hmm;  //use the structure define in hmm.h
	loadHMM(&hmm, init_file);
	
	int row=0, col=0, state=6;
	count_train_file_rc(train_file,row,col);

	int** observ=get2Darray_int(row,col);
	train_to_observ(train_file, row, col, observ);
	
	double** alpha=get2Darray_double(state,col);



	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			cout<<gamma[i][j];
		}
		cout<<endl;
	}
	return 0;

}


//count the rows(sequences) and the characters in each rows (columns)
void count_train_file_rc(char *train_file, int &row, int &col){
	FILE *fp = fopen(train_file, "r");
	int newRows=0;
	int newCols=0;
	char ch;

	while(!feof(fp))
	{	
		ch=fgetc(fp);
		if(ch == '\n')
		{	
			newRows++;
			col=newCols;
			newCols=0;
		}
		else
		{
			newCols++;
		}
	}
	row=newRows;

	fclose(fp);
}


int** get2Darray_int(int row, int col)
{
	int** arr = new int*[row];
	for(int i=0; i<row; i++){
		arr[i] = new int[col];
	}
	return arr;
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

//transfer train_file into 2D observation matrix
void train_to_observ(char *train_file, int row, int col, int** arr)
{	
	FILE *fp = fopen(train_file, "r");
	for(int i=0; i<row; i++){
		char observ_str[col];
		fscanf(fp,"%s", observ_str);
		for(int j=0; j<col; j++)
			arr[i][j]=observ_str[j]-'A';
	}

	fclose(fp);

}


//calcuate alpha
void cal_alpha(HMM *hmm, int state, int row, int col, double** alpha, int** observ)
{
	for(int i=0; i<state; i++){
		alpha[i][0]=hmm->initial[i]*hmm->observation[observ[][]][i];
	}
	for(int i=0; i<row; i++)
}











