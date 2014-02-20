#include "TTree.h"
#include "TF1.h"
#include "TMath.h"
#include "TRandom1.h"

// Struct which contains all data recorded in simulation
struct TrialDataSet
{
    float startingTime;
    float shapingPower;
    float shapingTime;
    float spreadValue;
    float chargeValues[10];
    float currentValues[10];
    float measurementTimes[10];
    float voltageValuesPreNoise[10];
    float voltageValuesPostNoise[10];
    int digitalReadoutValues[10];

    TrialDataSet() : startingTime(0.0), shapingPower(0.0), shapingTime(0.0), spreadValue(0.0)
    {
        for(int i=0;i<10;++i){
            chargeValues[i] = 0.0;
            currentValues[i] = 0.0;
            measurementTimes[i] = 0.0;
            voltageValuesPreNoise[i] = 0.0;
            voltageValuesPostNoise[i] = 0.0;
            digitalReadoutValues[i] = 0;
        }
    }
};


class ElectronSimulation
{

private:

     // Take this away and input struct into simulate

    // Analysis functions will also be in this class
    TProfile




    float randomGauss(float mean=0.0, float spread=1.0)
    {
        TRandom1 *randomGenerator = new TRandom1();
        return randomGenerator->Gaus(mean, spread);
    }

    float randomUniform(float mean=0.5,float range=1.0)
    {
        TRandom1 *randomGenerator = new TRandom1();
        return (randomGenerator->Rndm()*range) + mean;
    }

    void setMeasurementTimes(TrialDataSet&);
    void setChargeValues(TrialDataSet&, const TF1*);
    void setCurrentValues(TrialDataSet&);
    void setVoltageValuesPreNoise(TrialDataSet&);
    void setVoltageValuesPostNoise(TrialDataSet&);
    void setDigitalReadoutValues(TrialDataSet&);

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

    ElectronSimulation() {}

    // Simulation
    void simulate(TrialDataSet& eSimData, float shapingTime, float shapingPower)
    {
        // Create modelling function
        TF1 * func = new TF1("currentFunction", this->currentFunction, 0, 200, 2);
        func->SetParameters(shapingTime, shapingPower);

        eSimData.shapingTime = shapingTime;
        eSimData.shapingPower = shapingPower; 

        // Set uniform random starting time
        eSimData.startingTime = randomUniform(0.5, period);

        setMeasurementTimes(eSimData);
        setChargeValues(eSimData, func);
        setCurrentValues(eSimData);
        setVoltageValuesPreNoise(eSimData);
        setVoltageValuesPostNoise(eSimData);
        setDigitalReadoutValues(eSimData);
    }


    // Other methods
    static float currentFunction(double*, double*);
};

// Current function used to calculate current
float ElectronSimulation::currentFunction(double *x, double *par)
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

const float ElectronSimulation::frequency = 50.0e6;
const float ElectronSimulation::period = 1.0e9 / 50.0e6;
const float ElectronSimulation::subtractedTime = 30.0;
const float ElectronSimulation::averageTotalCharge = 0.02;
const float ElectronSimulation::currentChargeConversion = 1.0e3;
const float ElectronSimulation::currentValueConversion = 0.02 * 1.0e3;
const float ElectronSimulation::transimpedanceGain = 200.0;
const float ElectronSimulation::countVoltageConversion = 4.0;
const float ElectronSimulation::pedestal = 64.0;
const float ElectronSimulation::electronicNoiseRMS = 0.5;

void ElectronSimulation::setMeasurementTimes(TrialDataSet& eSimData)
{
    for (int i = 0; i < 10; i++)
    {
        eSimData.measurementTimes[i] = eSimData.startingTime + (i * period);
    }
}

void ElectronSimulation::setChargeValues(TrialDataSet& eSimData, const TF1* func)
{
    for (int i = 0; i < 10; i++)
    {
        eSimData.measurementTimes[i] = func->Eval(eSimData.measurementTimes[i]) * averageTotalCharge;
    }
}

void ElectronSimulation::setCurrentValues(TrialDataSet& eSimData)
{
    for (int i = 0; i < 10; i++)
    {
        eSimData.currentValues[i] = eSimData.chargeValues[i] * currentChargeConversion;
    }
}

void ElectronSimulation::setVoltageValuesPreNoise(TrialDataSet& eSimData)
{
    for (int i = 0; i < 10; i++)
    {
        eSimData.voltageValuesPreNoise[i] =  eSimData.currentValues[i] * transimpedanceGain;
    }
}

void ElectronSimulation::setVoltageValuesPostNoise(TrialDataSet& eSimData)
{
    for (int i = 0; i < 10; i++)
    {
        eSimData.voltageValuesPostNoise[i] =  eSimData.voltageValuesPreNoise[i] 
                                            + randomGauss(0.0, electronicNoiseRMS);
    }
}

void ElectronSimulation::setDigitalReadoutValues(TrialDataSet& eSimData)
{
    for (int i = 0; i < 10; i++)
    {
        eSimData.digitalReadoutValues[i] = (int) (eSimData.voltageValuesPostNoise[i] * countVoltageConversion + pedestal);
    }
}


float sumArray(const float * array)
{
    float sum = 0.0;
    // Get length
    int arrayLength = sizeof(array)/sizeof(*array);

    for (int i = 0; i < arrayLength; i++){sum += array[i];}

    return sum;
}




void execute()
{
    TTree * dataTree = new TTree() 

    const int numTrials = 1000; // Set number of trials
    const float shapingTime = 5.0;
    const float shapingPower = 40.0;

    ElectronSimulation * eSim = new ElectronSimulation();

    for (int i = 0; i < numTrials; i++)
    {
        TrialDataSet eSimData;
        eSim->simulate(eSimData, shapingTime, shapingPower);
        float totalCharge = sumArray(eSimData.chargeValues);
    }




}








