#include "TTree.h"
#include "TF1.h"
#include "TMath.h"
#include "TRandom1.h"
#include "TProfile.h"
#include "TGraph.h"
#include "TGraphErrors.h"

// Struct which contains all data recorded in simulation
struct TrialDataSet
{
    float shapingTime;
    float spreadValue;
    float measurementTimes[7];
    int digitalReadoutValues[7];

    TrialDataSet() : shapingTime(0.0), spreadValue(0.0)
    {
        for(int i = 0 ;i < 7; ++i ){
            measurementTimes[i] = 0.0;
            digitalReadoutValues[i] = 0;
        }
    }
};


class ElectronSimAnalysis
{

private:

    // Analysis functions will also be in this class

    float randomGauss(float mean=0.0, float spread=1.0)
    {
        TRandom1 *randomGenerator = new TRandom1();
        return randomGenerator->Gaus(mean, spread);
    }

    // Note: Mean is not really mean
    float randomUniform(float lowestValue,float range=1.0)
    {
        TRandom1 *randomGenerator = new TRandom1();
        return (randomGenerator->Rndm()*range) + lowestValue;
    }


public:

    // Set Frequency for recording data (Hz)
    static const float frequency;

    // Compute period (nanoseconds)
    static const float period;

    // Time which should be subtracted before beginning to take data (ns)
    static const float subtractedTime;

    // Total charge for each trial (pC)
    static const float averageTotalCharge;

    // Conversion factor from charge to current (microAmps)
    static const float currentValueConversion;

    // Conversion factor from current to voltage (mV / microAmps)
    static const float transimpedanceGain;

    // Conversion factor from voltage to bits
    static const float countVoltageConversion;

    // Pedestal value (bits)
    static const float pedestal;

    // Electronic Noise RMS value (mV)
    static const float electronicNoiseRMS;

    // Conversion factor from charge to current
    static const float currentChargeConversion;

    ElectronSimAnalysis() {}


    void plotCurrentFunction(float shapingTime, float shapingPower)
    {
        TF1 * func = new TF1("currentFunction", this->currentFunction, -20, 200, 2);
        func->SetParameters(shapingPower, shapingTime);
        func->Draw();
    }

    void plotFittingFunction(float shiftedTime = 0.0, float scalingFactor = 1.0, float bitShift = 0.0)
    {
        TF1 * func = new TF1("fittingFunction", this->fittingFunction, -39, 200, 3);
        func->SetParameters(shiftedTime, scalingFactor, bitShift);
        func->Draw();
    }

    void computeFunctionFit(TrialDataSet& eSimData, TF1 * func)
    {
        Float_t errorY[7];
        Float_t errorX[7];
        for (int i = 0; i < 7; ++i){
            errorX[i] = 0.0;
            errorY[i] = 12.0;
        }

         // Convert digital readout values to floats
         float floatReadoutValues[7];
         for (int i = 0; i < 7; ++i){floatReadoutValues[i] = eSimData.digitalReadoutValues[i];}

         TGraphErrors *gr = new TGraphErrors(7, eSimData.measurementTimes, floatReadoutValues,errorX,errorY);
         gr->Fit(func, "QN");

    }

    // Other methods
    static float currentFunction(double*, double*);
    static float fittingFunction(double*, double*);
    static float fittingFunction2(double*, double*);
    static float fittingFunction3(double*, double*);
};


// Current function used to calculate current
float ElectronSimAnalysis::currentFunction(double *x, double *par)
{
  // Initial return value
  float returnValue = 0.0;
  // Set x value
  float xValue = x[0];
  
  if (xValue > 0.0)
  {
    returnValue = (pow(par[0]*xValue/par[1],par[0])
                  /(par[1]*TMath::Gamma(par[0])))
                    *exp(-par[0]*xValue/par[1]);
  }
  return returnValue;
}


// Fitting funtion used for fitting
// par[0] is shifted time
float ElectronSimAnalysis::fittingFunction(double *x, double *par)
{
    // These values must be doubles here
    double shapingTime = 25.0;
    double shapingPower = 1.0;

    double currentX[1] = {x[0] - par[0]};
    double currentParameters[2] = {shapingPower, shapingTime};

    return (par[1] * currentFunction(currentX, currentParameters)) + par[2];
}


float ElectronSimAnalysis::fittingFunction2(double *x, double *par)
{

    double firstParam[3] = {par[0], par[1], par[2]};
    double secondParam[3] = {par[3], par[4], par[5]};

    //double firstX[1] = {x[0]};
   // double secondX[1] = {x[1]};

    return fittingFunction(x,firstParam)
          +fittingFunction(x, secondParam);
}

float ElectronSimAnalysis::fittingFunction3(double *x, double *par)
{
    // Par0 - shift in X 1st peak
    // Par1 - scalingFactor 1st peak
    // Par2 - vertical shift 1st peak
    // Par3 - shift in 2nd peak minus shift in 1st peak
    // Par4 - scaling factor 2nd peak
    // Par5 - vertical shift 2nd peak
    double firstParam[3] = {par[0], par[1], par[2]};
    double secondParam[3] = {par[3] + par[0], par[4], par[5]};

    //double firstX[1] = {x[0]};
   // double secondX[1] = {x[1]};

    return fittingFunction(x,firstParam)
          +fittingFunction(x, secondParam);

}



const float ElectronSimAnalysis::frequency = 50.0e6;
const float ElectronSimAnalysis::period = 20.0;
const float ElectronSimAnalysis::subtractedTime = 39.0;
const float ElectronSimAnalysis::averageTotalCharge = 0.02;
const float ElectronSimAnalysis::currentChargeConversion = 1.0e3;
const float ElectronSimAnalysis::currentValueConversion = 0.02 * 1.0e3;
const float ElectronSimAnalysis::transimpedanceGain = 200.0;
const float ElectronSimAnalysis::countVoltageConversion = 4.0;
const float ElectronSimAnalysis::pedestal = 64.0;
const float ElectronSimAnalysis::electronicNoiseRMS = 0.5;


float sumArray(const float * array, int arrayLength)
{
    float sum = 0.0;
    // For some reason sizeof(array) / sizeof(*array) has weird output
    for (int i = 0; i < arrayLength; ++i){sum = sum + array[i];}
    return sum;
}

int sumArray(const int * array, int arrayLength)
{
    int sum = 0;
    // For some reason sizeof(array) / sizeof(*array) has weird output
    for (int i = 0; i < arrayLength; ++i){sum = sum + array[i];}
    return sum;
}

int sumArray(const unsigned int * array, int arrayLength)
{
    int sum = 0;
    // For some reason sizeof(array) / sizeof(*array) has weird output
    for (int i = 0; i < arrayLength; ++i){sum = sum + array[i];}
    return sum;
}


int maxArray(int * array, int arrayLength)
{
    int maxValue = array[0];
    for (int i = 1; i < arrayLength; ++i){
        if (array[i] > maxValue) {maxValue = array[i];}
    }
    return maxValue;
}


// Current function not associated with Class
// Used for testing purposes
float currentFunction(double *x, double *par)
{
  ElectronSimAnalysis esim;
  return esim.currentFunction(x, par);
}

// Fitting function not associated with Class
// Used for testing purposes
float fittingFunction(double *x, double *par)
{
  ElectronSimAnalysis esim;
  return esim.fittingFunction(x, par);
}


float fittingFunction2(double *x, double *par)
{
    ElectronSimAnalysis esim;
    return esim.fittingFunction2(x, par);
}

float fittingFunction3(double *x, double *par)
{
    ElectronSimAnalysis esim;
    return esim.fittingFunction3(x, par);
}


// Compute probability for a certain Chi-squared
double chi2Test(double chi2, int degreesOfFreedom)
{
    return TMath::Prob(chi2, degreesOfFreedom);
}

void listElemArray(double * array, int arrayLength)
{
    for (int i = 0; i < arrayLength; ++i)
    {
        cout << i << " : " << array[i] << endl;
    }
}

bool hasElem(int * array, int elem, int arrayLength)
{
    for (int i = 0; i < arrayLength; ++i)
    {
        if (array[i] == elem)
        {
            return true;
        }
    }
    return false;
}

bool hasElemGreaterThan(int * array, int elem, int arrayLength)
{
    for (int i = 0; i < arrayLength; ++i)
    {
        if (array[i] > elem)
        {
            return true;
        }
    }
    return false;
}



int computeSumTrial(TrialDataSet &set)
{
    return sumArray(set.digitalReadoutValues, 7);
}

int computeMaxTrial(TrialDataSet &set)
{
    return maxArray(set.digitalReadoutValues, 7);
}

bool hasTwinPeaks(TrialDataSet &set)
{
    int numPeaks = 0;

    for (int i = 2; i < 7; ++i)
    {
        if (set.digitalReadoutValues[i] > set.digitalReadoutValues[i-1] && set.digitalReadoutValues[i+1] < set.digitalReadoutValues[i-1])
        {
            numPeaks++;
        }
    }
    if (numPeaks > 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

