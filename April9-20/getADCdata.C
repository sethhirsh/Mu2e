
{

	TFile f("PC.root");
	//TBrowser  browser;
	gDirectory->cd("makeSD");
	treeData  = (TTree*) gDirectory->Get("sddiag;1"); 

	float qMcenergy,qChiF1,qChiF2;
	int qAdc[7];
	float qF1Param0,qF1Param1,qF1Param2;
	float qF2Param0,qF2Param1,qF2Param2,qF2Param3,qF2Param4,qF2Param5;

	TTree* experimentTree = new TTree("experimentTree","experimentTree");
	experimentTree->Branch("qMcenergy",&qMcenergy,"qMcenergy/F");
	experimentTree->Branch("qAdc", qAdc, "qAdc[7]/I");
	experimentTree->Branch("qChiF1",&qChiF1,"qChiF1/F");
	experimentTree->Branch("qChiF2",&qChiF2,"qChiF2/F");
	experimentTree->Branch("qF2Param0",&qF2Param0,"qF2Param0/F");
	experimentTree->Branch("qF2Param1",&qF2Param1,"qF2Param1/F");
	experimentTree->Branch("qF2Param2",&qF2Param2,"qF2Param2/F");
	experimentTree->Branch("qF2Param3",&qF2Param3,"qF2Param3/F");
	experimentTree->Branch("qF2Param4",&qF2Param4,"qF2Param4/F");
	experimentTree->Branch("qF2Param5",&qF2Param5,"qF2Param5/F");
	experimentTree->Branch("qF1Param0",&qF1Param0,"qF2Param0/F");
	experimentTree->Branch("qF1Param1",&qF1Param1,"qF2Param1/F");
	experimentTree->Branch("qF1Param2",&qF1Param2,"qF2Param2/F");

	//Difference histogram
	TH1I *diffHist = new TH1I("diffHist", "diffHist", 500, 0, 100.0);

	//Sum histogram
	TH1I *sumHist = new TH1I("sumHist", "sumHist", 60, 0, 10000.0);

	//Integral Fit histogram
	TH1F *fitHist = new TH1F("fitHist","fitHist",10000,0.0,1.0);

	TH1F *chiSquareValues = new TH1F("chiSquareValues", "chiSquareValues", 1600, 0, 800.0);

	TH1F *probHist = new TH1F("probHist","Probability from Chi2",10,0.0,1.0);

	fitHist->SetBit(TH1::kCanRebin);
	sumHist->SetBit(TH1::kCanRebin);
	diffHist->SetBit(TH1::kCanRebin);
	//chiSquareValues->SetBit(TH1::kCanRebin);

	//Get Adc Values
	vector<unsigned int> * adc = 0;
	float mcenergy = 0;
	Float_t mcenergies[treeData->GetEntries()];
	Float_t chiSquareVals[treeData->GetEntries()];

	//Used for graph
	Float_t x[7], y[7];
	float adcj = 0.0;
	float adc0 = 0.0;

	Float_t scalingFact[treeData->GetEntries()];




	ElectronSimAnalysis * eSim = new ElectronSimAnalysis();
	TrialDataSet eSimData;
	treeData->SetBranchAddress("adc",&adc);
	treeData->SetBranchAddress("mcenergy",&mcenergy);
	TF1 * func = new TF1("fittingFunction", fittingFunction2, 0, 140, 6);
	TF1 * func2 = new TF1("fittingFunction", fittingFunction, 0, 140, 3);

//	func->SetParameters(45.0, 2.75e7, 0.0);

	for (int i = 0; i < treeData->GetEntries(); ++i) 
	//for (int i = 5002; i < 5008; ++i)
	{
		treeData->GetEntry(i);
	//	int array[7];
		for (int j = 0; j < 7; ++j){
			adc0 = adc[0][0];
			adcj = adc[0][j];
		if (mcenergy != 0.0)
		{
			eSimData.digitalReadoutValues[j] = (adcj- adc0);  ///mcenergy;
			eSimData.measurementTimes[j] = 20.0 * j;
		}

		}




if (mcenergy != 0 && adc0 <= adcj && !hasElemGreaterThan(eSimData.digitalReadoutValues,3900,7)) //&& i != 13285 && i != 15580 && i != 17186)
	{
	TF1 * func3 = new TF1("fittingFunction3", fittingFunction3, 0, 140, 6);
//	TF1 * func2 = new TF1("fittingFunction", fittingFunction, 0, 140, 3);
	/**int diffElem = maxArray(eSimData.digitalReadoutValues,7) - eSimData.digitalReadoutValues[7];
	diffHist->Fill(diffElem);

	int sumElem = sumArray(eSimData.digitalReadoutValues,7);
	sumHist->Fill(sumElem); **/
	
/**	if (hasTwoPeaks(eSimData))
	{
		func->SetParameters(45.0, computeMaxTrial(eSimData)/0.015, 0.0, 85.0, computeMaxTrial(eSimData)/0.015, 0.0);
		func->SetParLimits(1, 0.1e5, 2.0e9);
		func->SetParLimits(4, 0.5e5, 2.0e9);
		func->SetParLimits(0, 0.0, 100.0);
		func->SetParLimits(3, 30, 100.0);
		eSim->computeFunctionFit(eSimData, func);
		chiSquareValues->Fill(func->GetChisquare());
		chiSquareVals[i] = func->GetChisquare();
	}
	else
	{
		func2->SetParameters(45.0, computeMaxTrial(eSimData)/0.015, 0.0);
		func2->SetParLimits(1, 1.0, 2.0e9);
		eSim->computeFunctionFit(eSimData, func2);

		chiSquareValues->Fill(func2->GetChisquare());
		chiSquareVals[i] = func2->GetChisquare();
		float scalingFactor = func2->GetParameter(1);
	}*/


/**	func2->SetParameters(43.0, computeMaxTrial(eSimData)/0.015, 0.0);
	func2->SetParLimits(1, 1.0, 2.0e9);
	eSim->computeFunctionFit(eSimData, func2);

	chiSquareValues->Fill(func2->GetChisquare());
	chiSquareVals[i] = func2->GetChisquare();
	float scalingFactor = func2->GetParameter(1); **/

	func3->SetParameters(35.0, computeMaxTrial(eSimData)/0.015, 0.0, 5.0, computeMaxTrial(eSimData)/0.015, 0.0);
	func3->SetParLimits(3,5.0, 200.0);
	func3->SetParLimits(1, 1.0, 2.0e9);
	eSim->computeFunctionFit(eSimData, func3);
	float scalingFactor = func3->GetParameter(1);
	fitHist->Fill(scalingFactor);
	scalingFact[i] = scalingFactor;

	//if (scalingFactor >= 0.0)
	chiSquareValues->Fill(func3->GetChisquare());
	chiSquareVals[i] = func3->GetChisquare();
	//float prob  = chi2Test(fittedFunction->GetChisquare(),6);
	//probHist->Fill(prob);
	mcenergies[i] = mcenergy;
	delete func3;
}



	}


}

