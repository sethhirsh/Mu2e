{

	//float shapingTime = 100.0;
	const float shapingPower = 1.0;
	const int numTrials = 100000;

	ElectronSimulation * eSim = new ElectronSimulation();

	//TH1I *hist = new TH1I("totalBitsSumMethod", "Total Bits Using Sum Method");
	TTree *dataTree = new TTree("treeData", "treeData");
	int qDifferenceDigitalReadoutValue;

	dataTree->Branch("qDifferenceDigitalReadoutValue", 
					&qDifferenceDigitalReadoutValue, 
					"qDifferenceDigitalReadoutValue/I");

	for (int i = 0; i < numTrials; ++i)
	{
		TrialDataSet eSimData;
		eSim->simulate(eSimData, shapingTime, shapingPower);
		qDifferenceDigitalReadoutValue = maxArray(eSimData.digitalReadoutValues, 7) 
										- eSimData.digitalReadoutValues[0];
		dataTree->Fill();

	}

	TH1I *hist = new TH1I("hist","hist",40,qDifferenceDigitalReadoutValue,qDifferenceDigitalReadoutValue+1.0);
	hist->SetBit(TH1::kCanRebin);
	dataTree->Draw("qDifferenceDigitalReadoutValue>>hist");
	TAxis* xax = hist->GetXaxis();
	xax->SetTitle("Maximum Difference of Bits Per Trial");
	hist->SetTitle("Total Bit Values Using Difference Method");
	gStyle->SetOptStat("mer");
	hist->Draw();

}