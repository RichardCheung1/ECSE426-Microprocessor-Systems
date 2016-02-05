#include <stdio.h>
#include "arm_math.h"

typedef struct stateInfo{
	float q, r, x, p, k;
}kalman_state;


extern int Kalmanfilter_asm (float* InputArray, float* OutputArray, kalman_state* kstate, int length);
int Kalmanfilter_C(float* InputArray, float* OutputArray, kalman_state* kstate, int Length);
void c_sub(float* vector_one, float* vector_two, float* output_vector, int vector_length);
void c_mean(float* input_vector, int vector_length, float* vector_mean);
void c_std(float* input_vector, int vector_length, float* vector_std);
void c_correlate(float* vector_one, float* vector_two, float* vector_correlation, int vector_length);
void c_conv(float* vector_one, float* vector_two, float* vector_convolution, int vector_length);
int is_valid(float *ScrA, float *ScrB, int length, float tol, char *errorCode);
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
	float mean, sumOfSquares =0.0;
	c_mean (input_vector,vector_length, &mean);
	int i;
	for(i = 0; i < vector_length; i++) 
	{
		sumOfSquares += ((input_vector[i]-mean)*(input_vector[i]-mean));
	}
	*vector_std = sqrt(sumOfSquares/vector_length);
}

void c_correlate(float* vector_one, float* vector_two, float* vector_correlation, int vector_length)
{
	int correlation_length = vector_length + vector_length - 1;
	int i;
	for(i = 0; i < correlation_length; i++) 
	{
		int j = 0;
		for(j = 0; j < vector_length; j++) 
		{
			if((i - j) < 0 || (i - j) >= vector_length) 
			{
				vector_correlation[i] = 0;
			} 
			else 
			{
				vector_correlation[i] = vector_one[j] * vector_two [j - i];
			}
		}
	}
}

void c_conv(float* vector_one, float* vector_two, float* vector_convolution, int vector_length)
{	
	int convolution_length = vector_length + vector_length - 1;
	int i;
	for(i = 0; i < convolution_length; i++) 
	{
		int j = 0;
		for(j = 0; j < vector_length; j++) 
		{
			if((i - j) < 0 || (i - j) >= vector_length) 
			{
				vector_convolution[i] = 0;
			} 
			else 
			{
				vector_convolution[i] = vector_one[j] * vector_two [i - j];
			}
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
int is_valid(float *ScrA, float *ScrB, int length, float tol, char *errorCode)
{
	
	for(int i = 0; i < length; i++)
	{
		if(ScrA[i] - ScrB[i] > tol || ScrA[i] - ScrB[i] < -tol)
		{
			printf("Tests fails at %s with error tolerance %f:<SrcA:%f, SrcB:%f>\n", errorCode, tol,ScrA[i], ScrB[i]);
			return 0;
		}
	}
	printf("Tests Succeed at %s\n", errorCode);
	return 1;
}

int main()
{
	//initialize testing array
	float testVector[] = {0.1f,0.2f,0.3f,0.4f,0.5f};
	//get the size of the array
	int length = sizeof(testVector)/sizeof(float);
	//initiate empty output array of size length
	float outputArrayC[length];
	//initialize the struct at p=r=q 0.1 and x=k=0 
	kalman_state currentState = {0.1f, 0.1f, 0.0f , 0.1f, 0.0f};
	//call function Kalmanfilter_C
	Kalmanfilter_C(testVector, outputArrayC, &currentState, length);
	
	//initiate empty output array of size length
	float outputArrayASM[length];
	
	//reinitialize the struct at p=r=q 0.1 and x=k=0 
	currentState.p = 0.1f ; 
	currentState.r = 0.1f ; 	
	currentState.k = 0.0f;
	currentState.q = 0.1f ; 
	currentState.x = 0.0f ; 
	
	//call subroutine Kalmanfilter_asm
	Kalmanfilter_asm(testVector, outputArrayASM, &currentState, length );
	//Check for correctness with a error tolerance of 0.000001
	float errorTolerance = 0.000001f; 
	is_valid(outputArrayC, outputArrayASM, length, errorTolerance, "c vs asm");
	
	float differenceC[length];
	float differenceCMSIS[length];
	
	//Difference
	arm_sub_f32 (testVector, outputArrayC, differenceCMSIS, length);
	c_sub(testVector, outputArrayC, differenceC, length);
	
	is_valid(differenceC, differenceCMSIS, length, errorTolerance, "Difference");
	
	//Mean
	float meanCMSIS;
	float meanC; 
	arm_mean_f32 (differenceCMSIS, length , &meanCMSIS);
	c_mean(differenceC,length, &meanC); 
	is_valid(&meanC, &meanCMSIS, 1, errorTolerance, "mean"); 
	
	//STD
	float stdC;
	float stdCMSIS;
	arm_std_f32 (differenceCMSIS, length, &stdCMSIS);
	c_std(differenceC, length, &stdC);
	is_valid(&stdC, &stdCMSIS, 1, errorTolerance, "STD");
	
	//correlation
	float corC[2*length-1];
	float corCMSIS[2*length-1];
	arm_correlate_f32 (testVector, length, outputArrayC, length, corCMSIS);
	//c_correlate(testVector, outputArrayC, corC, 2*length-1);
	//is_valid(corC, corCMSIS, 2*length-1, errorTolerance, "correlation"); 
	
	//convolution
	float convC[2*length-1];
	float convCMSIS[2*length-1];
	arm_conv_f32 (testVector, length, outputArrayC, length, convCMSIS);
	c_conv(testVector, outputArrayC, convC, 2*length-1);
	is_valid(convC, convCMSIS, 2*length-1, errorTolerance, "convolution"); 

	return 0;
}

