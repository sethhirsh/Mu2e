{

	const float shapingTime = 100.0;
	const float shapingPower = 1.0;
	const int numTrials = 10000;

	ElectronSimulation * eSim = new ElectronSimulation();

	//TH1I *hist = new TH1I("totalBitsSumMethod", "Total Bits Using Sum Method");
	TTree *dataTree = new TTree("treeData", "treeData");
	int qDifferenceDigitalReadoutValue;

	dataTree->Branch("qDifferenceDigitalReadoutValue", &qDifferenceDigitalReadoutValue, "qDifferenceDigitalReadoutValue/I");

	for (int i = 0; i < numTrials; ++i)
	{
		TrialDataSet eSimData;
		eSim->simulate(eSimData, shapingTime, shapingPower);
		qDifferenceDigitalReadoutValue = maxArray(eSimData.digitalReadoutValues, 10) 
											- eSimData.digitalReadoutValues[0];
		dataTree->Fill();
	}
	

	dataTree->Draw("qDifferenceDigitalReadoutValue>>hist");
	TAxis* xax = hist->GetXaxis();
	xax->SetTitle("Max Difference Between Number of Bits Per Trial");
	hist->SetTitle("Max Difference Between Number of Bits");
	gStyle->SetOptStat("mer");
	hist->Draw();

}