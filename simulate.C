// This code is responsible for simulat
{
  // Set Frequency for recording data (Hz)
  const float frequency = 50.0e6;

  // Compute period (nanoseconds)
  const float period = 1.0e9 / frequency;

  // Set number of trials 
  const int numTrials = 10000;

  // Set number of functions to use
  const int numFunctions = 1;

  const int is = 0;

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

  TF1* shapesn[5];
  TF1* shapest[5];


  // Define possibilities for parameters for functions
  double sd[5] = {5.0,4.0,3.0,2.0,1.0};

  double st[5] = {15.0,20.0,25.0,30.0,40.0};

  // Set shaping time
  double shapingTime = 40.0;

  // set shaping power
  double shapingPower = 1.0;

  // Define struct to hold event data
  typedef struct
  {
    float voltageValues[10]; 
    float totalVoltage;
    float startingTime;
  } TrialDataSet;

  // Create file to store TTree of data
  TFile f("totalData.root","recreate");
  TTree * totalData = new TTree("total data", "All of Data");
  TrialDataSet trialData;
  totalData->Branch("voltageValues", trialData.voltageValues, "voltageValues[10]/F");
  totalData->Branch("totalVoltage", &trialData.totalVoltage, "totalVoltage/F");
  totalData->Branch("startingTime", &trialData.startingTime, "startingTime/F");

  //Leave 50 characters for name of function
  char sname[50];
  snprintf(sname,50,"shape_%i",is);

  // Leave 50 characters for name of title
  char stitle[50];
  snprintf(stitle,50,"Shaping power = %f",sd[is]);

  // Add each function to shapesn array
  shapesn[is]  = new TF1(sname,"(pow([0]*x/[1],[0])/([1]*TMath::Gamma([0])))*exp(-[0]*x/[1])",0,2000);
    
  // Set parameters
  shapesn[is]->SetParameters(sd[is],shapingTime);

  // Loop through trials
  for (int i = 0; i < numTrials; i++)
    {        
        // Leave the first 2 data points as zero for now
        trialData.voltageValues[0] = 0.0;
        trialData.voltageValues[1] = 0.0;

        // Initial variable to hold total trial value
        trialData.totalVoltage = 0.0;

        // Loop through times and store data in vectors
        for (int j = 2; j < 10; j++)
        {
          trialData.startingTime = startingTimes[i];

          // Compute current time at which data is being recorded
          float currentTime = startingTimes[i] + j * period - subtractedTime;

          // Evaluate function at current time and store in trial values
          trialData.voltageValues[j] = shapesn[is]->Eval(currentTime);

          // Add trial value to total trial value
          trialData.totalVoltage += trialData.voltageValues[j];
        }

      // Add event to total data
      totalData->Fill();
      }
    // Store total data in root file
    totalData->Write();
}


