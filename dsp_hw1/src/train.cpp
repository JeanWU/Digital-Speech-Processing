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
void cal_alpha(HMM *hmm, int state, int timestamp, int sample, double** alpha, int** observ);
void cal_beta(HMM *hmm, int state, int timestamp, int sample, double** beta, int** observ);
void cal_gamma(int state, int timestamp, double** alpha, double** beta, double** gamma);
void cal_b_update(int state, int timestamp, int sample, double** gamma, int** observ, double** b_update);
double*** get3Darray_double(int x, int y, int z);
void cal_epsilon(HMM *hmm, int state, int timestamp, int sample, double** alpha, double** beta, int** observ, double*** epsilon);
void accu(int state, int timestamp, double** gamma, double*** epsilon, double** accu_gamma, double** accu_epsilon);
void update_model(HMM *hmm, int state, int timestamp, int sample, double** accu_gamma, double** accu_epsilon, double** b_update);
void train(HMM *hmm, char *train_file, int iteration);
void delete_2Darray(double** arr, int row, int col);
void delete_3Darray(double*** arr, int x, int y, int z);



int main(int argc, char *argv[]){
	int iteration = atoi(argv[1]);  //convert string to int, iteration number
	char init_file[FILE_LENGTH], //model_init.txt
	     train_file[FILE_LENGTH], //seq_model_01-05.txt
	     dump_file[FILE_LENGTH];  //MODEL_01-05.TXT

	strcpy(init_file, argv[2]);
	strcpy(train_file, argv[3]);
	strcpy(dump_file, argv[4]);

	HMM hmm;  //use the structure define in hmm.h
	loadHMM(&hmm, init_file);

	train(&hmm, train_file, iteration);

	FILE *fp =fopen(dump_file, "wb");
	dumpHMM(fp, &hmm);
	fclose(fp);

	return 0;

}

void train(HMM *hmm, char *train_file, int iteration)
{
	int sample=0, timestamp=0, state=hmm->state_num;
	count_train_file_rc(train_file,sample,timestamp);

	int** observ=get2Darray_int(sample,timestamp);
	train_to_observ(train_file, sample, timestamp, observ);

	for(int j=0; j<iteration; j++)
	{

		double** accu_gamma=get2Darray_double(state,timestamp);
		double** accu_epsilon=get2Darray_double(state,state);
		double** b_update=get2Darray_double(state, state);

		for(int i=0; i<sample; i++)
		{
			double** alpha=get2Darray_double(state,timestamp);
			cal_alpha(hmm, state, timestamp, i, alpha, observ);
			double** beta=get2Darray_double(state, timestamp);
			cal_beta(hmm, state, timestamp, i, beta, observ);
			double** gamma=get2Darray_double(state, timestamp);
			cal_gamma(state, timestamp, alpha, beta, gamma);
			cal_b_update(state, timestamp, i, gamma, observ, b_update);
			double*** epsilon=get3Darray_double(timestamp-1,state,state);
			cal_epsilon(hmm, state, timestamp, i, alpha, beta, observ, epsilon);
			accu(state, timestamp, gamma, epsilon, accu_gamma, accu_epsilon);

			delete_2Darray(alpha,state,timestamp);
			delete_2Darray(beta,state,timestamp);
			delete_2Darray(gamma,state,timestamp);
			delete_3Darray(epsilon,timestamp-1,state,state);
		}
		update_model(hmm,state,timestamp,sample,accu_gamma,accu_epsilon,b_update);

		delete_2Darray(accu_gamma,state,timestamp);
		delete_2Darray(accu_epsilon,state,state);
		delete_2Darray(b_update,state,state);
	}

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


double*** get3Darray_double(int x, int y, int z)
{
	double*** arr = new double**[x];
	for(int i=0; i<x; i++){
		arr[i] = new double*[y];
		for(int j=0; j<y; j++){
			arr[i][j]=new double[z];
			for(int k=0; k<z; k++){
				arr[i][j][k]={0};
			}
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
void cal_alpha(HMM *hmm, int state, int timestamp, int sample, double** alpha, int** observ)
{
	//initialize alpha
	for(int i=0; i<state; i++){
		alpha[i][0]=hmm->initial[i]*hmm->observation[observ[sample][0]][i];
	}
	//alpha induction
	for(int t=1; t<timestamp; t++)
	{
		for(int j=0; j<state; j++)
		{
			for(int i=0; i<state; i++)
			{
				alpha[j][t]+=alpha[i][t-1]*hmm->transition[i][j];
			}
			alpha[j][t]*=hmm->observation[observ[sample][t]][j];
		}
	}
}


//calcuate beta
void cal_beta(HMM *hmm, int state, int timestamp, int sample, double** beta, int** observ)
{
	//initialize beta
	for(int i=0; i<state; i++){
		beta[i][timestamp-1]=1.;
	}
	//beta induction
	for(int t=timestamp-2; t>=0; t--)
	{
		for(int i=0; i<state; i++)
		{
			for(int j=0; j<state; j++)
			{
				beta[i][t]+=hmm->transition[i][j]*hmm->observation[observ[sample][t+1]][j]*beta[j][t+1];
			}
		}
	}
}


//calcuate gamma
void cal_gamma(int state, int timestamp, double** alpha, double** beta, double** gamma)
{
	for(int t=0; t<timestamp; t++){
		double sum=0.;
		for(int i=0; i<state; i++)
		{
			gamma[i][t]+=alpha[i][t]*beta[i][t];
			sum+=gamma[i][t];
		}
		for(int i=0; i<state; i++)
		{
			gamma[i][t]/=sum;
		}
	}
}


//calcuate b_updat for updating observation in the last step after calculating all samples
void cal_b_update(int state, int timestamp, int sample, double** gamma, int** observ, double** b_update)
{
	for(int t=0; t<timestamp; t++){
		for(int i=0; i<state; i++)
		{
			b_update[observ[sample][t]][i]+=gamma[i][t];
		}
	}
}


//calcuate epsilon
void cal_epsilon(HMM *hmm, int state, int timestamp, int sample, double** alpha, double** beta, int** observ, double*** epsilon)
{
	for(int t=0; t<timestamp-1; t++){
		double sum=0.;
		for(int i=0; i<state; i++)
		{
			for(int j=0; j<state; j++)
			{
				epsilon[t][i][j]=alpha[i][t]*hmm->transition[i][j]*hmm->observation[observ[sample][t+1]][j]*beta[j][t+1];
				sum+=epsilon[t][i][j];
			}
		}
		for(int i=0; i<state; i++)
		{
			for(int j=0; j<state; j++)
			{
				epsilon[t][i][j]/=sum;
			}
		}
	}
}

//accumulate gamma and beta
void accu(int state, int timestamp, double** gamma, double*** epsilon, double** accu_gamma, double** accu_epsilon)
{
	for(int i=0; i<state; i++)
	{
		for(int j=0; j<timestamp; j++)
		{
			accu_gamma[i][j]+=gamma[i][j];
		}
	}

	for(int i=0; i<timestamp-1; i++)
	{
		for(int j=0; j<state; j++)
		{
			for(int k=0; k<state; k++)
			{
				accu_epsilon[j][k]+=epsilon[i][j][k];
			}
		}
	}
}


//update model parameters
void update_model(HMM *hmm, int state, int timestamp, int sample, double** accu_gamma, double** accu_epsilon, double** b_update)
{
	//update initial
	for(int i=0; i<state; i++)
	{
		hmm->initial[i]=accu_gamma[i][0]/sample;
	}

	//update transition
	double gamma_list[state]={0};
	for(int t=0; t<timestamp-1; t++)
	{
		for(int i=0; i<state; i++)
		{
			gamma_list[i]+=accu_gamma[i][t];
		}
	}
	for(int i=0; i<state; i++)
	{
		for(int j=0; j<state; j++)
		{
			hmm->transition[i][j]=accu_epsilon[i][j]/gamma_list[i];
		}
	}

	//update observation
	for(int i=0; i<state; i++)
	{
		gamma_list[i]+=accu_gamma[i][timestamp-1];
	}
	for(int j=0; j<state; j++)
	{
		for(int k=0; k<hmm->observ_num; k++)
		{
			hmm->observation[k][j]=b_update[k][j]/gamma_list[j];
		}
	}
}


void delete_2Darray(double** arr, int row, int col)
{
	for(int i=0; i<row; i++)
	{
		delete[] arr[i];
	}
	delete[] arr;
}


void delete_3Darray(double*** arr, int x, int y, int z)
{
	for (int i = 0; i < x; ++i)
	{
		for (int j = 0; j < y; ++j)
		{
			delete [] arr[i][j];
		}
		delete [] arr[i];
	}
	delete [] arr;
}

