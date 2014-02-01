/* Objectives:
Construct vector -- add 2 zeros at the begining for the "prepoints"
Obtain values of function at 8 areas
Set random initial starting time for recording (range of random value is 0:1st period)
*/

{

  // Set Frequency for recording data (Hz)
  const double frequency = 5.0e6;

  // Compute period
  const double period = 1.0 / frequency;

  // Set number of trials 
  const double numTrials = 100;

  // Create array to store random values for initial data recording
  vector<double> startingTimes(numTrials);
  TRandom *R = new TRandom(time(0));
  for (int i = 0; i < numTrials; i++)
  {
    // Store random values in vector
    startingTimes[i] = R->Rndm()*20.0;
  }

  TF1* shapesn[5];
  TF1* shapest[5];

  double sd[5] = {5.0,4.0,3.0,2.0,1.0};

  double st[5] = {15.0,20.0,25.0,30.0,40.0};

  // Set shaping time
  double shapingTime(40.0);

  // set shaping power
  double shapingPower(1.0);

  // Set color scheme for functions
  int colors[5] = {1,2,3,4,5};
  TCanvas* ncan = new TCanvas("ncan","Shaper",800,800);

  //Create plot with varying shaping power

  // Create canvas
  TCanvas* ncan = new TCanvas("ncan","Shaper",800,800);

  // Create Legend
  TLegend* legn = new TLegend(0.6,0.6,0.9,0.9);

  //
  char head[20];
  snprintf(head,20,"Shaping Time = %f",shapingTime);
  legn->SetHeader(head);
  TH1F* shapern = new TH1F("shapern","Shaper Response;nsec;Normalized Response",100,-50,200);

  // Set range of plot
  shapern->SetMaximum(0.025);
  shapern->SetStats(0);

  // Draw histogram 
  shapern->Draw();

  // Loop to create and plot functions
  for(int is=0;is<1;++is)
  {
    // Leave 50 characters for name of function
    char sname[50];
    snprintf(sname,50,"shape_%i",is);

    // Leave 50 characters for name of title
    char stitle[50];
    snprintf(stitle,50,"Shaping power = %f",sd[is]);

    // Add each function to shapesn array
    shapesn[is]  = new TF1(sname,"(pow([0]*x/[1],[0])/([1]*TMath::Gamma([0])))*exp(-[0]*x/[1])",-30,10);
    
    // Set parameters
    shapesn[is]->SetParameters(sd[is],40.0);

    // Create vector to contain vectors of simulated data
    vector< vector<double> > totalData(numTrials);

    // Loop through trials
    for (int i = 0; i < numTrials; i++)
      {        
          // Create vector to store values from each trial
          vector<double> trialValues(10);

          // Leave the first 2 data points as zero for now
          trialValues[0] = 0.0;
          trialValues[1] = 0.0;

          // Loop through times and store data in vectors
          for (int j = 2; j < 10; j++)
          {
            // Compute current time at which data is being recorded
            double currentTime = startingTimes[i] + j * period;

            // Evaluate function at current time and store in trial values
            trialValues[j] = shapesn[is]->Eval(currentTime); //shapesn[is]->Eval(currentTime);
          }

          // Store data from trial in totalData
          totalData[i] = trialValues;
      }

    // Set line color
    shapesn[is]->SetLineColor(colors[is]);

    // Display graph of function
    shapesn[is]->Draw("same");

    // Add function to legend
    legn->AddEntry(shapesn[is],stitle,"L");
  }

  // Display legend
  legn->Draw();

/**
  // Create plot with varying shaping time 
  TCanvas* tcan = new TCanvas("tcan","Shaper",800,800);

  // Create histogram object
  TH1F* shapert = new TH1F("shapert","Shaper Response;nsec;Normalized Response",100,0,200);

  // Create legend
  TLegend* legt = new TLegend(0.6,0.6,0.9,0.9);
  snprintf(head,20,"Shaping Power = %f",shapingPower);
  legt->SetHeader(head);

  // Set range for plot 
  shapert->SetMaximum(0.025);

  // Turn stats box off
  shapert->SetStats(0);

  // Draw histogram 
  shapert->Draw();
 for(int is=0;is<5;++is){

    // Create string to hold 
    char sname[50];
    snprintf(sname,50,"shape_%i",is);
    char stitle[50];
    snprintf(stitle,50,"Shaping time = %f",st[is]);
    shapest[is]  = new TF1(sname,"(pow([0]*x/[1],[0])/([1]*TMath::Gamma([0])))*exp(-[0]*x/[1])",0,200);

    // Set parameters
    shapest[is]->SetParameters(1.0,st[is]);

    // Set line color
    shapest[is]->SetLineColor(colors[is]);

    // Draw function and keep all previous functions on plot
    shapest[is]->Draw("same");

    // Add function to legend
    legt->AddEntry(shapest[is],stitle,"L");
  }
  legt->Draw();
  **/
}
