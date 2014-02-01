// This code is responsible for simulat
{
  // Set Frequency for recording data (Hz)
  const float frequency = 50.0e6;

  // Compute period (nanoseconds)
  const float period = 1.0e9 / frequency;

  // Set number of trials 
  const int numTrials = 1000;

  // Time which should be subtracted before beginning to take data (ns)
  const float subtractedTime = 30.0;

  // Create array to store random values for initial data recording
  float startingTimes[numTrials];
  TRandom *R = new TRandom(time(0));
  for (int i = 0; i < numTrials; i++)
  {
    // Store random values in vector
    startingTimes[i] = R->Rndm() * period;
  }

  // Create 2d array to store functions
  TF1* shapesn[5][5];

  // Define possibilities for parameters for functions
  const float sp[5] = {5.0,4.0,3.0,2.0,1.0};

  const float st[5] = {15.0,20.0,25.0,30.0,40.0};

  // Define struct to hold event data
  typedef struct
  {
    float voltageValues[10]; 
    float totalVoltage;
    float startingTime;
    float shapingPower;
    float shapingTime;
  } TrialDataSet;

  // Create file to store TTree of data
  TFile f("totalData.root","recreate");
  TTree * totalData = new TTree("total data", "All of Data");
  TrialDataSet trialData;
  totalData->Branch("voltageValues", trialData.voltageValues, "voltageValues[10]/F");
  totalData->Branch("totalVoltage", &trialData.totalVoltage, "totalVoltage/F");
  totalData->Branch("startingTime", &trialData.startingTime, "startingTime/F");
  totalData->Branch("shapingPower", &trialData.shapingPower, "shapingPower/F");
  totalData->Branch("shapingTime", &trialData.shapingTime, "shapingTime/F");

  // Loop through sp array
  for (int spIter = 0; spIter < 5; spIter++)
  {
    // Loop through st array
    for (int stIter = 0; stIter < 5; stIter++)
    {
      cout << spIter << endl;
      trialData.shapingPower = sp[spIter];
      trialData.shapingTime = st[stIter];

    //Leave 50 characters for name of function
    char sname[50];
    snprintf(sname,50,"shape_[%i][%i]",spIter,stIter);

    // Leave 50 characters for name of title
    char stitle[50];
    snprintf(stitle,50,"Shaping Power = %f, Shaping Time = %f",sp[spIter],st[stIter]);

    // Add each function to shapesn array
    shapesn[spIter][stIter] = new TF1(sname,"(pow([0]*x/[1],[0])/([1]*TMath::Gamma([0])))*exp(-[0]*x/[1])",-200,200);

    // Set parameters for function
    shapesn[spIter][stIter]->SetParameters(sp[spIter],st[stIter]);

    // Loop through trials
    for (int i = 0; i < numTrials; i++)
      {        

        // Initial variable to hold total trial value
        trialData.totalVoltage = 0.0;

        // Loop through times and store data in vectors
        for (int j = 0; j < 10; j++)
        {
          trialData.startingTime = startingTimes[i];

          // Compute current time at which data is being recorded
          // Start data recording at random time minus some constant specified tme
          float currentTime = startingTimes[i] + j * period - subtractedTime;

          // Evaluate function at current time and store in trial values
          trialData.voltageValues[j] = shapesn[spIter][stIter]->Eval(currentTime);

          // Add trial value to total trial value
          trialData.totalVoltage += trialData.voltageValues[j];
        }

      // Add event to total data
      totalData->Fill();
      }
    // Store total data in root file
    totalData->Write();

    }
  }


// Plot data where shaping power is one

// Create canvas
TCanvas* ncan = new TCanvas("ncan","Shaper",800,800);

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
}



}
