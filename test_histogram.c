/* test_histogram.c : make histograms and analyze data

*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "zignor.h"

#define PREPRINT 10
#define NUM_RAW_MOMENTS 8

#define H_NBINS 200
#define H_XW 5.0d
#define H_FILE "histogram.bin"


// utility functions

int double_factorial(int n) {
    if (n<=1) return(1);
    else n=n*double_factorial(n-2);
    return(n);
}


int main(int argc, char *argv[])
{
	long long int i, Nsamples;
	int j;
	
	int zigseed;
	double val;
	
	double X[NUM_RAW_MOMENTS], x_j;
	int expected;
	
	long long int H[H_NBINS];
	double HV[H_NBINS];
	int hi; // not unsigned!
	double hdx;
	double voff;
	
	FILE *fp;
	unsigned int Nbins;
	
	switch(argc)
	{
		case 1:
			Nsamples = 10000000;
			zigseed = 0;
			break;
		case 3:
			Nsamples = atoll(argv[1]);
			zigseed = atoi(argv[2]);
			break;
		default:
			printf("ERROR. Unexpected number of arguments.\n");
			printf("> test_histogram.exe <Nsamples> <seed>\n");
			return(1);
	}
	
	RanNormalSetSeedZig(&zigseed, 1);
	
	// Print the first numbers generated, for visual inspection
	for (i = 0; i < PREPRINT; i++)
	{
		val = DRanNormalZig();
		printf("%10.6f\n", val);
	}
	
	// Initialize raw moment accumulators
	for (j = 0; j < NUM_RAW_MOMENTS; j++) {
		X[j] = 0.0;
	}
	
	// Initialize histogram
	hdx = (2.0d*H_XW) / H_NBINS;
	for (hi = 0; hi < H_NBINS; hi++)
	{
		H[hi] = 0;
		HV[hi] = (hi * hdx) - H_XW;
	}


	for (i = 0; i < Nsamples; i++) {
		val = DRanNormalZig();
		
		// fill histogram
		voff = val + H_XW;
		hi = (int) lround(voff / hdx);
		if ((hi >= 0) && (hi < H_NBINS))
		{
			H[hi]++;
		}
		
		// Calculate raw moments of the generated random numbers
		for (j=0, x_j=val; j<NUM_RAW_MOMENTS; j++, x_j*=val) 
		{
			X[j] += x_j;
		}
	}

	printf("Created %lld normally distributed pseudo-random numbers...\n", Nsamples);
	
	//Output moments
	for (j=0; j < NUM_RAW_MOMENTS; j++) {
		expected = ( (j+1)%2 == 0 ? double_factorial(j) : 0 );
		printf("X%d: %f (Expected %i)\n", j+1, X[j]/Nsamples, expected);	
	}

	fp = fopen(H_FILE, "wb");
	Nbins = H_NBINS;
	fwrite(&Nbins, sizeof(Nbins), 1, fp);
	fwrite(HV, sizeof(HV[0]), H_NBINS, fp); 
	fwrite(H, sizeof(H[0]), H_NBINS, fp);	
	fclose(fp);
	
	// quick output histogram
	for (hi = 0; hi < H_NBINS; hi++)
	{
		printf("%10.6f\t%lld\n", HV[hi], H[hi]);
	}
	
	return 0;
}