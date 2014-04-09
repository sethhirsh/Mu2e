{
	ElectronSimulation esim;
	TrialDataSet set;
	float shapingTime = 40.0;
	float shapingPower = 1.0;
	esim.simulate(set, shapingTime, shapingPower);
	gStyle->SetOptFit();

	Float_t x[7], y[7];

	for (int i = 0; i < 7; ++i)
	{
		y[i] = set.digitalReadoutValues[i];
		x[i] = set.measurementTimes[i]; 
	}

	TGraph *graph = new TGraph(7,x, y);
	graph->Draw("A*"); 
 	graph->SetLineColor(2);
    graph->SetLineWidth(4);
    graph->SetMarkerColor(4);
    //graph->SetMarkerStyle(21);
    graph->SetTitle("Sample Trial of Bit Value vs. Measurement Time");
    graph->GetXaxis()->SetTitle("Measurement Time (ns)");
    graph->GetYaxis()->SetTitle("Bit Value (Number of Bits Measured)");



	//TPaveStats *st = (TPaveStats*)graph->GetListOfFunctions()->FindObject("stats");
	//graph->SetOptStat(111); 
	//TPaveStats* sb2=(TPaveStats*)(graph->GetListOfFunctions()->FindObject("stats"));
	//sb2->SetX1NDC(.2);
	graph->SetStats(1);
	//gStyle->SetOptFit(1011);
	func = esim.computeFunctionFit(set);
	func->Draw("same");

}