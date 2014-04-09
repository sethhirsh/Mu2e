#include "TTree.h"
#include "TF1.h"
#include "TMath.h"
#include "TRandom1.h"
#include "TProfile.h"
#include "TGraph.h"

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
    float qIntegralReadoutValue;

    TrialDataSet() : startingTime(0.0), shapingPower(0.0), shapingTime(0.0), spreadValue(0.0)
    {
        for(int i = 0 ;i < 10; ++i ){
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

    float integrateFunc(float);

    void simulate(TrialDataSet&, float, float);

    void plotTProfile(TTree *);

    void plotHistogram(TTree* treeData ) {
        treeData->Draw("qChargeValues");}

    void plotCurrentFunction(float shapingTime, float shapingPower)
    {
        TF1 * func = new TF1("currentFunction", this->currentFunction, -20, 200, 2);
        func->SetParameters(shapingPower, shapingTime);
        func->Draw();
    }

    void plotFittingFunction(float shiftedTime = 0.0, float scalingFactor = 1.0)
    {
        TF1 * func = new TF1("fittingFunction", this->fittingFunction, -20, 200, 2);
        func->SetParameters(shiftedTime, scalingFactor);
        func->Draw();
    }

    TF1* computeFunctionFit(TrialDataSet& eSimData)
    {
         TF1 * func = new TF1("fittingFunction", this->fittingFunction, -20, 200, 2);
         //func->SetParameters(eSimData.startingTime - subtractedTime, 1.0);
         func->SetParameters(50.0, 35700.0);

         // Convert digital readout values to floats
         float floatReadoutValues[10];
         for (int i = 0; i < 10; ++i){floatReadoutValues[i] = eSimData.digitalReadoutValues[i];}


         TGraph *gr = new TGraph(10, eSimData.measurementTimes, floatReadoutValues);

         gr->Fit(func, "QN");
         return func;
         
      //  fitParameters[0] = fittedFunction->GetParameter(0);
        // fitParameters[1] = fittedFunction->GetParameter(1);
         
    }

    // Other methods
    static float currentFunction(double*, double*);
    static float fittingFunction(double*, double*);
};

 // Simulation
    void ElectronSimulation::simulate(TrialDataSet& eSimData, float shapingTime, float shapingPower)
    {
        // Create modelling function
        TF1 * func = new TF1("currentFunction", this->currentFunction, 0, 200, 2);
        func->SetParameters(shapingPower, shapingTime);

        eSimData.shapingTime = shapingTime;
        eSimData.shapingPower = shapingPower; 

        // Set uniform random starting time
        eSimData.startingTime = randomUniform(0.0, period);

        setMeasurementTimes(eSimData);
        setChargeValues(eSimData, func);
        setCurrentValues(eSimData);
        setVoltageValuesPreNoise(eSimData);
        setVoltageValuesPostNoise(eSimData);
        setDigitalReadoutValues(eSimData); 

    }


    void ElectronSimulation::plotTProfile(TTree* treeData )
    {
        TProfile* hprof = new TProfile("hprof", "hprof", 10, -0.5, 9.5);
      
        for (int i = 0; i < 10; ++i)
        { 
            char chargeValueString[100];
            snprintf(chargeValueString,100,"qChargeValues[%i]:%i",i,i);
            treeData->Project("+hprof",chargeValueString);
        }
        hprof->Draw();
    }


// Current function used to calculate current
float ElectronSimulation::currentFunction(double *x, double *par)
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
float ElectronSimulation::fittingFunction(double *x, double *par)
{
    // These values must be doubles here
    double shapingTime = 40.0;
    double shapingPower = 1.0;

    double currentX[1] = {x[0] - par[0]};
    double currentParameters[2] = {shapingPower, shapingTime};

    return par[1] * currentFunction(currentX, currentParameters);
}

// Computes integral from 0 to inf for current function
float ElectronSimulation::integrateFunc(float scalingFactor)
{

    // Since the function is already normalized the scaling factor will be the integral
    return scalingFactor;
}





const float ElectronSimulation::frequency = 50.0e6;
const float ElectronSimulation::period = 20.0;
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
    for (int i = 0; i < 10; ++i)
    {
        eSimData.measurementTimes[i] = eSimData.startingTime + (i * period) - subtractedTime;
    }
}

void ElectronSimulation::setChargeValues(TrialDataSet& eSimData, const TF1* func)
{
    for (int i = 0; i < 10; ++i)
    {
        eSimData.chargeValues[i] = func->Eval(eSimData.measurementTimes[i]) * averageTotalCharge;

        // Subtract initial charge value from all values
        eSimData.chargeValues[i] -= eSimData.chargeValues[0];
    }
}

void ElectronSimulation::setCurrentValues(TrialDataSet& eSimData)
{
    for (int i = 0; i < 10; ++i)
    {
        eSimData.currentValues[i] = eSimData.chargeValues[i] * currentChargeConversion;
    }
}

void ElectronSimulation::setVoltageValuesPreNoise(TrialDataSet& eSimData)
{
    for (int i = 0; i < 10; ++i)
    {
        eSimData.voltageValuesPreNoise[i] =  eSimData.currentValues[i] * transimpedanceGain;
    }
}

void ElectronSimulation::setVoltageValuesPostNoise(TrialDataSet& eSimData)
{
    for (int i = 0; i < 10; ++i)
    {
        eSimData.voltageValuesPostNoise[i] =  eSimData.voltageValuesPreNoise[i] 
                                            + randomGauss(0.0, electronicNoiseRMS);
    }
}

void ElectronSimulation::setDigitalReadoutValues(TrialDataSet& eSimData)
{
    for (int i = 0; i < 10; ++i)
    {

        // Took out pedestal
        eSimData.digitalReadoutValues[i] = (int) (eSimData.voltageValuesPostNoise[i] * countVoltageConversion);
    }
}

// This function essentially subtracts the first value from all other values
/**void ElectronSimulation::differenceTotalReadoutValue(int *array)
{
    for (int i = 0; i < 10; ++i)
    {

    }
} **/


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

int maxArray(const int * array, int arrayLength)
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
  ElectronSimulation esim;
  return esim.currentFunction(x, par);
}

// Fitting function not associated with Class
// Used for testing purposes
float fittingFunction(double *x, double *par)
{
  ElectronSimulation esim;
  return esim.fittingFunction(x, par);
}




TTree* execute()
{

    // q is simply a prefix for values which are a branch on the TTree
    TTree * dataTree = new TTree("treeData", "treeData");
    float qChargeValues[10];
    float qTotalCharge;
    float qMeasurementTimes[10];
    float qCurrentValues[10];
    float qVoltageValuesPreNoise[10];
    float qVoltageValuesPostNoise[10];
    int qDigitalReadoutValues[10];
    float qFitShiftingTime;
    float qFitScalingFactor;
    int qTotalDigitalReadoutValue;
    int qDifferenceDigitalReadoutValue;
    float qIntegralReadoutValue;

    dataTree->Branch("qChargeValues", qChargeValues, "qChargeValues[10]/F");
    dataTree->Branch("qTotalCharge", &qTotalCharge, "qTotalCharge/F");
    dataTree->Branch("qMeasurementTimes", qMeasurementTimes, "qMeasurementTimes[10]/F"); 
    dataTree->Branch("qCurrentValues", qCurrentValues, "qCurrentValues[10]/F"); 
    dataTree->Branch("qVoltageValuesPreNoise", qVoltageValuesPreNoise, "qVoltageValuesPreNoise[10]/F"); 
    dataTree->Branch("qVoltageValuesPostNoise", qVoltageValuesPostNoise, "qVoltageValuesPostNoise[10]/F"); 
    dataTree->Branch("qDigitalReadoutValues", qDigitalReadoutValues, "qDigitalReadoutValues[10]/I");
    dataTree->Branch("qFitShiftingTime", &qFitShiftingTime, "qFitShiftingTime/F");
    dataTree->Branch("qFitScalingFactor", &qFitScalingFactor, "qFitScalingFactor/F");
    dataTree->Branch("qTotalDigitalReadoutValue", &qTotalDigitalReadoutValue, "qTotalDigitalReadoutValue/I");
    dataTree->Branch("qDifferenceDigitalReadoutValue", &qDifferenceDigitalReadoutValue, "qDifferenceDigitalReadoutValue/I");
    dataTree->Branch("qIntegralReadoutValue", &qIntegralReadoutValue, "qIntegralReadoutValue/F");


    const int numTrials = 10000; // Set number of trials
    const float shapingTime = 40.0;
    const float shapingPower = 1.0;

    ElectronSimulation * eSim = new ElectronSimulation();

    for (int i = 0; i < numTrials; ++i)
    {
        TrialDataSet eSimData;
        eSim->simulate(eSimData, shapingTime, shapingPower);

        qTotalCharge = sumArray(eSimData.chargeValues, 10);
        qTotalDigitalReadoutValue = sumArray(eSimData.digitalReadoutValues, 10);
        qDifferenceDigitalReadoutValue = maxArray(eSimData.digitalReadoutValues, 10) - eSimData.digitalReadoutValues[0];




        TF1 * fittedFunction = eSim->computeFunctionFit(eSimData);
        qFitShiftingTime = fittedFunction->GetParameter(0);
        qFitScalingFactor = fittedFunction->GetParameter(1);
        qIntegralReadoutValue = eSim->integrateFunc(qFitScalingFactor);
        fittedFunction->Draw();

        for (int j = 0; j<10; ++j)
            {
                qChargeValues[j] = eSimData.chargeValues[j];
                qMeasurementTimes[j] = eSimData.measurementTimes[j];
                qCurrentValues[j] = eSimData.currentValues[j];
                qVoltageValuesPreNoise[j] = eSimData.voltageValuesPreNoise[j];
                qVoltageValuesPostNoise[j] = eSimData.voltageValuesPostNoise[j];
                qDigitalReadoutValues[j] = eSimData.digitalReadoutValues[j];
            }
        dataTree->Fill();
    }

    return dataTree;


}







