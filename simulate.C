#include "TTree.h"
#include "TF1.h"
#include "TMath.h"

// Define struct to hold event data
typedef struct
{
  float voltageValues[10]; 
  float countValues[10]; 
//  float totalVoltage;
 float totalCount;
  float startingTime;
  float shapingPower;
  float shapingTime;
  int trialNumber;
} TrialDataSet;

// Writes and returns all of data for simulation
TTree * simulation()
{
  // Set Frequency for recording data (Hz)
  const float frequency = 50.0e6;

  // Compute period (nanoseconds)
  const float period = 1.0e9 / frequency;

  // Set number of trials 
  const int numTrials = 1000;

  // Time which should be subtracted before beginning to take data (ns)
  const float subtractedTime = 30.0;

  // Total charge for each trial (pC)
  const float averageTotalCharge = 0.02;

  // Conversion factor from charge to current (microAmps)
  const float currentValueConversion = averageTotalCharge * 1.0e3;

  // Conversion factor from current to voltage (mV / microAmps)
  const float transimpedanceGain = 200.0;

  // Conversion factor from voltage to bits
  const float countVoltageConversion = 4.0;

  // Pedestal value (bits)
  const float pedestal = 64.0;

  // Electronic Noise RMS value (mV)
  const float eletronicNoiseRMS = 0.5;

  // Create array to store random values for initial data recording
  float startingTimes[numTrials];
  TRandom *R = new TRandom(time(0));
  for (int i = 0; i < numTrials; i++)
  {
    // Store random values in vector
    startingTimes[i] = R->Rndm() * period;
  }

  // Create array to store random values from Gaussian distribution to 
  // be added to voltage values for spread
  float spreadValues[numTrials][10];

  // Create Trandom object to get spread values in for loop
  TRandom1 *spreadGenerator = new TRandom1();
  for (int i = 0; i < numTrials; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      spreadValues[i][j] = (spreadGenerator->Gaus(0.0, eletronicNoiseRMS));
    }
  }

  // Create 2d array to store functions
  TF1* shapesn[5][5];

  // Define possibilities for parameters for functions
  const float sp[5] = {5.0,4.0,3.0,2.0,1.0};

  const float st[5] = {15.0,20.0,25.0,30.0,40.0};

  // Create file to store TTree of data
  TFile f("totalData.root","recreate");
  TTree * totalData = new TTree("totalData", "All of Data");
  TrialDataSet trialData;
  totalData->Branch("voltageValues", trialData.voltageValues, "voltageValues[10]/F");
  totalData->Branch("countValues", trialData.countValues, "countValues[10]/F");
//  totalData->Branch("totalVoltage", &trialData.totalVoltage, "totalVoltage/F");
  totalData->Branch("totalCount", &trialData.totalCount, "totalCount/F");  
  totalData->Branch("startingTime", &trialData.startingTime, "startingTime/F");
  totalData->Branch("shapingPower", &trialData.shapingPower, "shapingPower/F");
  totalData->Branch("shapingTime", &trialData.shapingTime, "shapingTime/F");
  totalData->Branch("trialNumber", &trialData.trialNumber, "trialNumber/I");

  // Loop through sp array
  for (int spIter = 0; spIter < 1; spIter++)
  {
    // Loop through st array
    for (int stIter = 4; stIter < 5; stIter++)
    {
      //cout << spIter << endl;
      trialData.shapingPower = sp[spIter];
      trialData.shapingTime = st[stIter];

    //Leave 50 characters for name of function
    char sname[50];
    snprintf(sname,50,"shape_[%i][%i]",spIter,stIter);

    // Leave 50 characters for name of title
    char stitle[50];
    snprintf(stitle,50,"Shaping Power = %f, Shaping Time = %f",sp[spIter],st[stIter]);

    // Add each function to shapesn array
    shapesn[spIter][stIter] = new TF1(sname,voltageFunction,-200,200,2);

    // Set parameters for function
    shapesn[spIter][stIter]->SetParameters(sp[spIter],st[stIter]);

    // Loop through trials
    for (int i = 0; i < numTrials; i++)
      {        

        // Initial variable to hold total trial value
  //     trialData.totalVoltage = 0.0;
        trialData.totalCount = 0.0;

        // Loop through times and store data in vectors
        for (int j = 0; j < 10; j++)
        {
          trialData.startingTime = startingTimes[i];

          // Compute current time at which data is being recorded
          // Start data recording at random time minus some constant specified tme
          float currentTime = startingTimes[i] + j * period - subtractedTime;

          // Evaluate function at current time and store in trial values
          const float evaluatedValue = (shapesn[spIter][stIter]->Eval(currentTime));

          // Add Gaussian spread evaluated value and store it in voltage value
          trialData.voltageValues[j] = evaluatedValue;

          // Convert voltage value to count value through scaling
          // Note spread values is added to add error in voltage
          trialData.countValues[j] = (countVoltageConversion 
                                   * ((transimpedanceGain
                                   * currentValueConversion 
                                   * trialData.voltageValues[j]) + spreadValues[i][j]))+pedestal;           

          // Add trial value to total trial values
    //      trialData.totalVoltage += trialData.voltageValues[j];
          trialData.totalCount += trialData.countValues[j];
        }

        //Record trial number
        trialData.trialNumber = i;

      // Add event to total data
      totalData->Fill();
      }
    }
  }
  // Store total data in root file
  totalData->Write();

  // return total data
  return totalData;
}


// Current function used to calculate voltage
float voltageFunction(double *x, double *par)
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

// Plot TProfile of voltage values
TProfile * plotTProfileVoltageValues()
{
// Load data file
 TFile *f = new TFile("totalData.root");

 // Convert data from file into TTree
 TTree *tData = (TTree *)f->Get("totalData");

 // Create canvas
 //TCanvas *c1 = new TCanvas("c1","Histogram Of Voltages",200,10,700,500);

 // Create TProfile
  TProfile* hprof  = new TProfile("hprof","Profile of voltage",10,-0.5,9.5);

  for (int i = 0; i < 10; i++)
  { 
    char voltageValueString[50];
    snprintf(voltageValueString,50,"voltageValues[%i]:%i",i,i);
    tData->Project("+hprof",voltageValueString,"(shapingPower==5.0)&&(shapingTime==40.0)");
  }
  return hprof;
}


// Plot TProfile of count values
TProfile * plotTProfileCountValues()
{
// Load data file
 TFile *f = new TFile("totalData.root");

 // Convert data from file into TTree
 TTree *tData = (TTree *)f->Get("totalData");

 // Create canvas
 //TCanvas *c1 = new TCanvas("c1","Histogram Of Voltages",200,10,700,500);

 // Create TProfile
  TProfile* hprof  = new TProfile("hprof","Profile of Count Values",10,-0.5,9.5);

  for (int i = 0; i < 10; i++)
  { 
    char voltageValueString[50];
    snprintf(voltageValueString,50,"countValues[%i]:%i",i,i);
    tData->Project("+hprof",voltageValueString,"(shapingPower==5.0)&&(shapingTime==40.0)");
  }
  return hprof;
}

// Creates projection for data and returns it as histogram
TH1F* createProjectionTotalVoltage(TTree * data)
{
  data->Draw("totalVoltage>>hist","(shapingPower==5.0)&&(shapingTime==40.0)");
  return hist;

}

// Fits function to a histogram
void fitHistogramWithFunction(TH1D * hist, float (* function)(double *, double *) )
{
  hist->Fit(function);
  hist->Draw();  
}


double getTotalBitsSingleTrial(TTree * data, int trialNumber, int pedestal)
{
  char trialNum[100];
  snprintf(trialNum,100,"(shapingPower==5.0)&&(shapingTime==40.0)&&(trialNumber==%i)",trialNumber);

  data->Draw("countValues>>hist",trialNum);

  return ((hist->GetMean() - pedestal) * hist->GetEntries());

}

double getTotalChargeSingleTrial(TTree * data, int trialNumber, int pedestal)
{
  getTotalBitsSingleTrial(data, trialNumber, pedestal);
}

TH1D* getTotalBitsEachTrial(TTree *data, numTrials, pedestal)
{

 // double * pointer;
  TH1D * totalBits = new TH1D("totalBits","totalBits",800, 0 , 800);
  totalBits->SetBit(TH1::kCanRebin); 
  for (int i = 0; i < numTrials; i++)
  {
    totalBits->Fill(getTotalBitsSingleTrial(data, i, pedestal));
  }

  return totalBits;
}

//TH1D* getTotalChargeEachTrial(TTree *data, int numTrial)



void readTTree()
{
  TFile *f = new TFile("totalData.root");
  TTree *tData = (TTree *)f->Get("totalData");
}



// Used to execute functions
void run()
{

  simulation();
  TFile *f = new TFile("totalData.root");
  TTree *tData = (TTree *)f->Get("totalData");

//TH1D *intHist = new TH1D("intHist", "intHist", -5, 0, 800);

  tData->Draw("countValues>>intHist","(shapingPower==5.0)&&(shapingTime==40.0)");


  // Create TF1 object from C function
  //TF1 * func = new TF1("voltageFunction",voltageFunction, 0,200,2);
  //func->SetParameters(5.0,40.0);
  //createProjection(tData);

  // Fit function to histogram
  //hist->Fit("voltageFunction");
  //hist->Draw();

  //hist->Draw();
}



//tData->Draw("totalVoltage:startingTime","(shapingPower==5.0)&&(shapingTime==40.0)");


// Plot data where shaping power is one

// Create canvas
/**TCanvas* ncan = new TCanvas("ncan","Shaper",800,800);

// Create Legend
TLegend* legn = new TLegend(0.6,0.6,0.9,0.9);

for (int i = 0; i < 5; i++)
{
  char selection[60];
  snprintf(selection,60,"(shapingPower==%f)&&(shapingTime==%f)",sp[4],st[i]);

  totalData->SetMarkerColor(i+1);

  if (i == 0)
    totalData->Draw("totalVoltage:startingTime",selection);
  else
    totalData->Draw("totalVoltage:startingTime",selection,"same");
} **/



