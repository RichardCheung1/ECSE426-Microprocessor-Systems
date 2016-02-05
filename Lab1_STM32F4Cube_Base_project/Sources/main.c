#include <stdio.h>
#include "arm_math.h"

typedef struct stateInfo{
	float q, r, x, p, k;
}kalman_state;


extern int Kalmanfilter_asm (float* InputArray, float* OutputArray, kalman_state* kstate, int length);
int Kalmanfilter_C(float* InputArray, float* OutputArray, kalman_state* kstate, int Length);


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

int isValid(float *ScrA, float *ScrB, int length, float tol, char *errorCode)
{
	
	for(int i = 0; i < length; i++)
	{
		if(ScrA[i] - ScrB[i] > tol || ScrA[i] - ScrB[i] < -tol)
		{
			printf("Tests fails at %s\n", errorCode);
			return 0;
		}
	}
	printf("Tests Succeed at %s\n", errorCode);
	return 1;
}

int main()
{
	//initialize testing array
	float test_vector[] = {0.1f,0.2f,0.3f,0.4f,0.5f};
	//get the size of the array
	int length = sizeof(test_vector)/sizeof(float);
	//initiate empty output array of size length
	float output_array_C[length];
	//initialize the struct at p=r=q 0.1 and x=k=0 
	kalman_state currentState = {0.1f, 0.1f, 0.0f , 0.1f, 0.0f};
	//call function Kalmanfilter_C
	Kalmanfilter_C(test_vector, output_array_C, &currentState, length);
	
	//initiate empty output array of size length
	float output_array_asm[length];
	
	//reinitialize the struct at p=r=q 0.1 and x=k=0 
	currentState.p = 0.1f ; 
	currentState.r = 0.1f ; 	
	currentState.k = 0.0f;
	currentState.q = 0.1f ; 
	currentState.x = 0.0f ; 
	
	//call subroutine Kalmanfilter_asm
	Kalmanfilter_asm(test_vector, output_array_asm, &currentState, length );
	//Check for correctness with a tolerance of 0.000001
	isValid(output_array_C,output_array_asm,length, 0.000001, "c vs asm");
	
	
	//float state_CMSIS[4];
	
	//Difference
	//arm_sub_f32 (float32_t *pSrcA, float32_t *pSrcB, float32_t *pDst, uint32_t blockSize);
	
	//Mean
	//arm_mean_f32 (float32_t *pSrc, uint32_t blockSize, float32_t *pResult);
	
	//STD
	//arm_std_f32 (float32_t *pSrc, uint32_t blockSize, float32_t *pResult);
	
	//correlation;
	//arm_correlate_f32 (float32_t *pSrcA, uint32_t srcALen, float32_t *pSrcB, uint32_t srcBLen, float32_t *pDst);
	
	//convolution
	//arm_conv_f32 (float32_t *pSrcA, uint32_t srcALen, float32_t *pSrcB, uint32_t srcBLen, float32_t *pDst);
	
	return 0;
}

