// This code is responsible for simulat
{
  // Set Frequency for recording data (Hz)
  const double frequency = 5.0e6;

  // Compute period
  const double period = 1.0 / frequency;

  // Set number of trials 
  const int numTrials = 20;

  // Set number of functions to use
  const int numFunctions = 5;

  // Create array to store random values for initial data recording
  vector<double> startingTimes(numTrials);
  TRandom *R = new TRandom(time(0));
  for (int i = 0; i < numTrials; i++)
  {
    // Store random values in vector
    startingTimes[i] = R->Rndm();
  }

  TF1* shapesn[5];
  TF1* shapest[5];

  // Set standard deviations for each function????
  double sd[5] = {5.0,4.0,3.0,2.0,1.0};

  // Set shaping times for each function????
  double st[5] = {15.0,20.0,25.0,30.0,40.0};

  // Set shaping time
  double shapingTime(40.0);

  // set shaping power
  double shapingPower(1.0);

  // Create array to hold all of data
  vector < vector <double> > totalData(numFunctions);

 // Loop to through each function to record data
  for(int is=0;is<numFunctions;is++)
  {
    // Leave 50 characters for name of function
    char sname[50];
    snprintf(sname,50,"shape_%i",is);

    // Leave 50 characters for name of title
    char stitle[50];
    snprintf(stitle,50,"Shaping power = %f",sd[is]);

    // Add each function to shapesn array
    shapesn[is]  = new TF1(sname,"(pow([0]*x/[1],[0])/([1]*TMath::Gamma([0])))*exp(-[0]*x/[1])",0,200);
    
    // Set parameters
    shapesn[is]->SetParameters(sd[is],shapingTime);

    //Create vector to contain TGraphs of simulated data
    vector < double > functionData(numTrials);

    // Loop through trials
    for (int i = 0; i < numTrials; i++)
      {        
          // Create vector to store values from each trial
          double trialValues[10];
          double recordingTimes[10];

          // Leave the first 2 data points as zero for now
          trialValues[0] = 0.0;
          trialValues[1] = 0.0;
          recordingTimes[0] = 0.0;
          recordingTimes[1] = 0.0;

          // Initial variable to hold total trial value
          double totalTrialValue = 0.0;

          // Loop through times and store data in vectors
          for (int j = 2; j < 10; j++)
          {
            // Compute current time at which data is being recorded
            double currentTime = startingTimes[i] + j * period;

            // Add current time to recording times
            recordingTimes[j] = currentTime;

            // Evaluate function at current time and store in trial values
            trialValues[j] = shapesn[is]->Eval(currentTime);

            // Add trial value to total trial value
            totalTrialValue += trialValues[j];
          }

          // Store total value for trial in function data vector
          functionData[i] = totalTrialValue;

          // Create graph to hold trial data
         // TGraph *trialGraph = new TGraph(10, recordingTimes, trialValues);


      }
      totalData[is] = functionData;

      cout << is <<endl;
      // Store function data in total data
      //totalData[is] = functionData;
     // cout << totalData[0][0][9] << endl;

    }
}
