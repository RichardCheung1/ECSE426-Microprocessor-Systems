#include <stdio.h>
#include "arm_math.h"

typedef struct stateInfo{
	float q, r, x, p, k;
}kalman_state;

//int Kalmanfilter_C(float* InputArray, float* OutputArray, kalman_state* kstate, int Length);
int Kalmanfilter_C(float* InputArray, float* OutputArray, kalman_state* kstate, int Length)
{	
	int i;
	for(i = 0; i<Length; i++) {
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (InputArray[i] - kstate->x);
		kstate->p = (1 - kstate->k) * kstate->p;

		OutputArray[i] = kstate->x;

		printf("InputValue: %f\t Output: %f\t\n", InputArray[i], OutputArray[i]);
	}
	return kstate->x; 
}
	



float test_vector[] = {0.1,0.2,0.3,0.4,0.5};

int main()
{
	
	int Length = sizeof(test_vector)/sizeof(float);

	float outputArray[Length];
	
	printf("%d\n", Length);
	kalman_state currentState = {1.0f, 1.0f, 0.0f , 1.0f, 0.0f};
	Kalmanfilter_C(test_vector, outputArray, &currentState, Length);

	/*
	//This following module is to check that the output array is being updated correctly
	printf("The following values were printed from the main method:\n");
	for(int i = 0; i<Length; i++) {
		printf("InputValue: %f\t Output: %f\t\n", inputArray[i], outputArray[i]);
	}
	*/
	
	//float state_CMSIS[4];
	
	//Difference
	//arm_sub_f32 (float32_t *pSrcA, float32_t *pSrcB, float32_t *pDst, uint32_t blockSize);
	
	//Mean
	//arm_mean_f32 (float32_t *pSrc, uint32_t blockSize, float32_t *pResult);
	
	//STD
	//arm_std_f32 (float32_t *pSrc, uint32_t blockSize, float32_t *pResult);
	
	//correlation
	//arm_correlate_f32 (float32_t *pSrcA, uint32_t srcALen, float32_t *pSrcB, uint32_t srcBLen, float32_t *pDst)
	
	//convolution
	//arm_conv_f32 (float32_t *pSrcA, uint32_t srcALen, float32_t *pSrcB, uint32_t srcBLen, float32_t *pDst)
	return 0;
}


