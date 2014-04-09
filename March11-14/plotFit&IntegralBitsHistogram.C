{

	const float shapingTime = 40.0;
	const float shapingPower = 1.0;
	const int numTrials = 10000;
	

	ElectronSimulation * eSim = new ElectronSimulation();

	//TH1I *hist = new TH1I("totalBitsSumMethod", "Total Bits Using Sum Method");
	TTree *dataTree = new TTree("treeData", "treeData");
	int qIntegralReadoutValue;
	int qFitScalingFactor;

	//dataTree->Branch("qIntegralReadoutValue", 
					//&qIntegralReadoutValue, 
					//"qIntegralReadoutValue/I");

	TH1I *hist = new TH1I("hist","hist",40,16000.0,16001.0);
	hist->SetBit(TH1::kCanRebin);
	for (int i = 0; i < numTrials; ++i)
	{
		TrialDataSet eSimData;
		eSim->simulate(eSimData, shapingTime, shapingPower);
		TF1 * fittedFunction = eSim->computeFunctionFit(eSimData);
		qFitScalingFactor = fittedFunction->GetParameter(1);
		//cout << qFitScalingFactor << endl;
        qIntegralReadoutValue = eSim->integrateFunc(qFitScalingFactor);
		//dataTree->Fill();
		hist->Fill(qIntegralReadoutValue);



	}

	//dataTree->Draw("qIntegralReadoutValue>>hist");
	TAxis* xax = hist->GetXaxis();.
	xax->SetTitle("Integral of Bits Per Trial");
	hist->SetTitle("Total Bit Values Using Integral Method");
	gStyle->SetOptStat("mer");
	hist->Draw();

}