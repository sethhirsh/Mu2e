
{

	TFile f("PC.root");
	//TBrowser  browser;
	gDirectory->cd("makeSD");
	treeData  = (TTree*) gDirectory->Get("sddiag;1"); 

	//Difference histogram
	TH1I *diffHist = new TH1I("diffHist", "diffHist", 500, 0, 100.0);

	//Sum histogram
	TH1I *sumHist = new TH1I("sumHist", "sumHist", 60, 0, 10000.0);

	//Integral Fit histogra,
	TH1F *fitHist = new TH1F("fitHist","fitHist",1000,0,1.0e6.0);

	TH1F *chiSquareValues = new TH1F("chiSquareValues", "chiSquareValues", 500, 0, 2.0e9);

	fitHist->SetBit(TH1::kCanRebin);
	sumHist->SetBit(TH1::kCanRebin);
	diffHist->SetBit(TH1::kCanRebin);

	//Get Adc Values
	vector<unsigned int> *adc = 0;
	float mcenergy = 0;

	//Used for graph
	Float_t x[7], y[7];
	float adcj = 0.0;
	float adc0 = 0.0;


	ElectronSimulation * eSim = new ElectronSimulation();
	TrialDataSet eSimData;
//	treeData->SetBranchAddress("adc",&adc);
	//treeData->GetEntries()
	treeData->SetBranchAddress("adc",&adc);
	treeData->SetBranchAddress("mcenergy",&mcenergy);
	for (int i = 0; i < treeData->GetEntries(); ++i)
	{
		treeData->GetEntry(i);
	//	int array[7];
		for (int j = 0; j < 7; ++j){
			adc0 = adc[0][0];
			adcj = adc[0][j];
		if (mcenergy != 0.0)
		{
			eSimData.digitalReadoutValues[j] = (adcj- adc0)/mcenergy;
			eSimData.measurementTimes[j] = 20.0 * j;
		}

		}

//	treeData->SetBranchAddress("mcenergy",&mcenergy);


	/**	for (int k = 0; k < 7; ++k)
		{
		y[k] = eSimData.digitalReadoutValues[k];
		x[k] = eSimData.measurementTimes[k];
		}
		TGraph *graph = new TGraph(7,x, y);
		func = eSim->computeFunctionFit(eSimData);
		if (func->GetParameter(1) > 30.0e3 && func->GetParameter(1) < 40.0e3 )
		{
			graph->Draw("A*"); 
			func->Draw("same");
		}**/



	if (mcenergy != 0 && adc0 <= adcj && i != 13285 && i != 15580 && i != 17186)
	{
	int diffElem = maxArray(eSimData.digitalReadoutValues,7) - eSimData.digitalReadoutValues[7];
	diffHist->Fill(diffElem);

	int sumElem = sumArray(eSimData.digitalReadoutValues,7);
	sumHist->Fill(sumElem);

	TF1 * fittedFunction = eSim->computeFunctionFit(eSimData);
	float scalingFactor = fittedFunction->GetParameter(1);
	if (scalingFactor < 0.0)
	{
	//	cout << scalingFactor;
		cout << i << endl;
	}
	fitHist->Fill(scalingFactor);
	chiSquareValues->Fill(fittedFunction->GetChisquare());
}


	}


	//treeData->BuildIndex("adc"); 

	//treeData->Draw("adc>>hist");
	//numEntries = treeData->GetEntries();

	//for (int i = 0; i < numEntries)

}

