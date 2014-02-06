#include "TMath.h"

float voltageFunction(float *x, float *par)
{
	// Initial return value
	float returnValue = 0.0;
	// Set x value
	float xValue = x[0];
	
	if (xValue > 0.0)
	{
	 returnValue = (pow(par[0]*xValue/par[1],par[0])/(par[1]*TMath::Gamma(par[0])))*exp(-par[0]*xValue/par[1]);
	}
	return returnValue;
}