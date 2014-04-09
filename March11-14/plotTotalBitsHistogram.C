{

	float shapingTime = 100.0;
	const float shapingPower = 1.0;
	const int numTrials = 10000;

	ElectronSimulation * eSim = new ElectronSimulation();

	//TH1I *hist = new TH1I("totalBitsSumMethod", "Total Bits Using Sum Method");
	TTree *dataTree = new TTree("treeData", "treeData");
	int qTotalDigitalReadoutValue;

	dataTree->Branch("qTotalDigitalReadoutValue", &qTotalDigitalReadoutValue, "qTotalDigitalReadoutValue/I");

	for (int i = 0; i < numTrials; ++i)
	{
		TrialDataSet eSimData;
		eSim->simulate(eSimData, shapingTime, shapingPower);
		qTotalDigitalReadoutValue = sumArray(eSimData.digitalReadoutValues, 7);
		dataTree->Fill();

	}



	TH1I *hist = new TH1I("hist","hist",40,qTotalDigitalReadoutValue,qTotalDigitalReadoutValue+1.0);
	hist->SetBit(TH1::kCanRebin);

	dataTree->Draw("qTotalDigitalReadoutValue>>hist");
	TAxis* xax = hist->GetXaxis();
	xax->SetTitle("Total Number of Bits Per Trial");
	hist->SetTitle("Total Bit Values Using Sum Method");
	gStyle->SetOptStat("mer");
	hist->Draw();

}