#include <stdio.h>
#include "arm_math.h"
#include "demo.h"

typedef struct stateInfo{
	float q, r, x, p, k;
}kalman_state;

extern int Kalmanfilter_asm (float* InputArray, float* OutputArray, kalman_state* kstate, int Length);

int Kalmanfilter_C(float* InputArray, float* OutputArray, kalman_state* kstate, int Length);
void c_sub(float* vector_one, float* vector_two, float* output_vector, int vector_length);
void c_mean(float* input_vector, int vector_length, float* vector_mean);
void c_std(float* input_vector, int vector_length, float* vector_std);
void c_correlate(float* vector_one, float* vector_two, float* vector_correlation, int vector_length);
void c_conv(float* vector_one, float* vector_two, float* vector_convolution, int vector_length);
void is_valid(float *ScrA, float *ScrB, int vector_length, float tol, char *errorCode);
int is_correct(float* actualOutput, float* controlOutput);

int is_correct(float* actualOutput, float* controlOutput)
{	
	/*int compareLength = (sizeof(actualOutput)/sizeof(float));
	printf("Check float length: %d\n", compareLength);*/

	//Compare size of the arrays
	if(sizeof(actualOutput) == sizeof(controlOutput)) 
	{
		//Compare each float in the arrays
		for(int i = 0; i < 10; i++) 
		{
			printf("Check output float value: %.9g\n", actualOutput[i]);
			printf("Check output float value: %.9g\n", controlOutput[i]);
			//if(actualOutput[i] != controlOutput[i]) return 0;
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

void c_sub(float* vector_one, float* vector_two, float* output_vector, int vector_length)
{
	int i;
	for(i = 0; i < vector_length; i++)
	{
		output_vector[i] = vector_one[i] - vector_two[i];
	}
}

void c_mean(float* input_vector, int vector_length, float* vector_mean)
{
	int i;
	float sum = 0;
	for(i = 0; i < vector_length; i++) 
	{
		sum = sum + input_vector[i];
	}

	*vector_mean = sum / vector_length;
}

void c_std(float* input_vector, int vector_length, float* vector_std)
{	
	float mean, sum, sumOfSquares =0.0;
	c_mean (input_vector,vector_length, &mean);
	int i;
	for(i = 0; i < vector_length; i++) 
	{
		float difference = fabsf(input_vector[i] - mean);
		sum += difference;
		sumOfSquares += difference*difference;
	}
	*vector_std = sqrt((sumOfSquares-(sum*sum)/vector_length)/(vector_length-1));
}

void c_correlate(float* vector_one, float* vector_two, float* vector_correlation, int vector_length)
{
	int correlation_length = vector_length + vector_length - 1;
	int i, kmin, kmax, j;
	for(i = 0; i < correlation_length; i++) 
	{
		vector_correlation[i]=0; 
		//determine the bounds
		kmin = (i < vector_length - 1) ? 0 : i - vector_length - 1;
		kmax = (i < vector_length - 1) ? i : vector_length - 1;
		
		for(j = kmin; j <= kmax; j++) 
		{
			vector_correlation[i] += vector_one[j] * vector_two [(vector_length - 1) -(i-j)];
		}
	}
}

void c_conv(float* vector_one, float* vector_two, float* vector_convolution, int vector_length)
{	
	int convolution_length = vector_length + vector_length - 1;
	int i, kmin, kmax, j;
	for(i = 0; i < convolution_length; i++) 
	{
		vector_convolution[i]=0; 
		
    kmin = (i >= vector_length - 1) ? i - (vector_length - 1) : 0;
    kmax = (i < vector_length - 1) ? i : vector_length - 1;
		
		for(j = kmin; j <= kmax; j++) 
		{
			vector_convolution[i] += vector_one[j] * vector_two [i - j];
		}
	}
}

int Kalmanfilter_C(float* InputArray, float* OutputArray, kalman_state* kstate, int Length)
{	
	int i;
	for(i = 0; i < Length; i++){
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (InputArray[i]-kstate->x);
		if (kstate->x != kstate->x) {
			printf("An error has occured -NaN value was found"); 
			return 1;
		}
		kstate->p = (1-kstate->k) * kstate->p;
		OutputArray[i] = kstate->x;

		printf("InputValue: %f\t Output: %f\t\n", InputArray[i], OutputArray[i]);
	}
	return 0; 
}

//checks if Source A and Source B have the same value
void is_valid(float *ScrA, float *ScrB, int vector_length, float tol, char *errorCode)
{
	int failed = 1;
	for(int i = 0; i < vector_length; i++)
	{
		if(ScrA[i] - ScrB[i] > tol || ScrA[i] - ScrB[i] < -tol)
		{
			failed = 0;
			printf("Tests fails at %s with absolute error tolerance %f:<SrcA:%f, SrcB:%f>\n", errorCode, tol,ScrA[i], ScrB[i]);
		}
		printf("Tests Succeed at %s with absolute error tolerance %f\n", errorCode, tol);

	}
	if (failed) 
	{
		printf("Tests Succeed at %s with absolute error tolerance %f\n", errorCode, tol);
	}
}

void is_valid_relative(float *ScrA, float *ScrB, int vector_length, float errTol, float errRelativeTol, char *errorCode)
{
	int failed = 1;
	for(int i = 0; i < vector_length; i++)
	{
		float relativeError; 
		if(ScrA[i] - ScrB[i] > errTol || ScrA[i] - ScrB[i] < -errTol)
		{
			failed = 0;
			printf("Relative Tests fails at %s :<SrcA:%f, SrcB:%f>\n", errorCode, ScrA[i], ScrB[i]);
		}

		relativeError= (ScrB[i] > ScrA[i]) ? fabsf(ScrB[i]-ScrA[i]) / ScrB[i] : fabsf(ScrA[i]-ScrB[i]) / ScrA[i];

		failed = (relativeError <= errRelativeTol) ? 1:0;
	}
	if (failed) 
	{
		printf("Relative Tests Succeed" );
	}
}


int main()
{
	//initialize testing array
	float testVector[] = {0.1f,0.2f,0.3f,0.4f,0.5f};
	
	/*COMMENTED OUT LENGTH PARAM AS IT IS INCLUDED IN HEADER FILE*/
	//get the size of the array
	//int length = sizeof(testVector)/sizeof(float);

	//initiate empty output array of size length
	float outputArrayC[length];
	//initialize the struct at p=r=q 0.1 and x=k=0 
	kalman_state currentState = {0.1f, 0.1f, 0.0f , 0.1f, 0.0f};
	//call function Kalmanfilter_C
	Kalmanfilter_C(measurements, outputArrayC, &currentState, length);
	
	//initiate empty output array of size length
	float outputArrayASM[length];
	
	//reinitialize the struct at p=r=q 0.1 and x=k=0 
	currentState.p = DEF_p; 
	currentState.r = DEF_r; 	
	currentState.k = DEF_k;
	currentState.q = DEF_q; 
	currentState.x = DEF_x; 
	
	//call subroutine Kalmanfilter_asm
	Kalmanfilter_asm(measurements, outputArrayASM, &currentState, length );

	//Check for correctness with a error tolerance of 0.000001
	float errorTolerance = 0.000001f;
	float errorPercentage = 0.01; 
	//is_valid(outputArrayC, outputArrayASM, length, errorTolerance, "c vs asm");
	//is_valid_relative(outputArrayC, outputArrayASM, length, errorTolerance, errorPercentage,"c vs asm");
	int p;
/*
	for ( p = 0; p < length;  p++ )
	{
		printf("OutputASM: %f \n", outputArrayASM[p]); 
	}*/

	float differenceC[length];
	float differenceCMSIS[length];
	
	//Difference
	arm_sub_f32 (measurements, outputArrayC, differenceCMSIS, length);
	c_sub(measurements, outputArrayC, differenceC, length);
	
	//is_valid(differenceC, differenceCMSIS, length, errorTolerance, "Difference");
	//is_valid_relative(differenceC, differenceCMSIS, length, errorTolerance, errorPercentage,"Difference");
	
	//Print difference vector
	for ( p = 0; p < length;  p++ )
	{
		printf("DifferenceC: %f & DifferenceCMSIS %f \n", differenceC[p], differenceCMSIS[p]); 
	}


	//Mean
	float meanCMSIS;
	float meanC; 
	arm_mean_f32 (differenceCMSIS, length , &meanCMSIS);
	c_mean(differenceC,length, &meanC); 
	//is_valid(&meanC, &meanCMSIS, 1, errorTolerance, "mean"); 
	//is_valid_relative(&meanC, &meanCMSIS, 1, errorTolerance, errorPercentage, "mean"); 
	
	//Print mean values
	printf("MeanC: %f & MeanCMSIS %f \n", meanC, meanCMSIS); 
	
	//STD
	float stdC;
	float stdCMSIS;
	arm_std_f32 (differenceCMSIS, length, &stdCMSIS);
	c_std(differenceC, length, &stdC);
	//is_valid(&stdC, &stdCMSIS, 1, errorTolerance, "STD");
	//is_valid_relative(&stdC, &stdCMSIS, 1, errorTolerance, errorPercentage,"STD");

	//Print std values
	printf("StandardDevC: %f & StandardDevCMSIS %f \n", stdC, stdCMSIS); 

	//correlation
	float corC[2*length-1];
	float corCMSIS[2*length-1];
	arm_correlate_f32 (measurements, length, outputArrayC, length, corCMSIS);
	c_correlate(measurements, outputArrayC, corC, length);
	//is_valid(corC, corCMSIS, 2*length-1, errorTolerance, "correlation"); 
	//is_valid_relative(corC, corCMSIS, 2*length-1, errorTolerance, errorPercentage, "correlation"); 
	
	//convolution
	float convC[2*length-1];
	float convCMSIS[2*length-1];
	arm_conv_f32 (measurements, length, outputArrayC, length, convCMSIS);
	c_conv(measurements, outputArrayC, convC, length);
	//is_valid(convC, convCMSIS, 2*length-1, errorTolerance, "convolution"); 
	//is_valid_relative(convC, convCMSIS, 2*length-1, errorTolerance, errorPercentage, "convolution"); 

	//Print correlation and convolution values
	for ( p = 0; p < (2*length-1);  p++ )
	{
		printf("ConvC: %f & ConvCMSIS: %f \n", convC[p], convCMSIS[p]); 
	}
	for ( p = 0; p < (2*length-1);  p++ )
	{
		printf("CorrelateC: %f & CorrelatCMSIS: %f \n", corC[p], corCMSIS[p]); 
	}



	return 0;
}

