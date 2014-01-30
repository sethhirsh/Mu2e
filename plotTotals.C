// Only run this script after running simulate.C
{

// Create Canvas
//TCanvas* ncan = new TCanvas("ncan","Shaper",800,800);

//TLegend* legn = new TLegend(0.6,0.6,0.9,0.9);

// Create array of histograms (one for each function)
TH1D* functionHistArray[numFunctions];

// Loop through each function
for (int i = 0; i < numFunctions; i++)
{
	// Leave 50 characters for name of function
    char sname[50];
    snprintf(sname,50,"shape_%i",i);

//0.04 to 0.043
	// Create histogram 
	functionHistArray[i] = new TH1D(sname, "histTitle",40, 0.03, 0.043); 

	// Loop through each trial and plot graph for each function
	for (int j = 0; j < numTrials; j++)
	{
		functionHistArray[i]->Fill(totalData[i][j]);	
	}

	if (i == 0)
	{
		functionHistArray[i]->Draw();
	}
	else
	{
		functionHistArray[i]->Draw("same");
	}

	//functionHistArray[i]->Draw("same");
}

}