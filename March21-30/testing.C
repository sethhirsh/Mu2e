{
	ElectronSimulation esim;
	TrialDataSet set;
	float shapingTime = 40.0;
	float shapingPower = 1.0;
	esim.simulate(set, shapingTime, shapingPower);

	int numTrials = 10000;

	Float_t x[numTrials], y[numTrials];

	for (int i = 0; i < numTrials; ++i)
	{
		esim.simulate(set, shapingTime, shapingPower);
		//TF1 * fittedFunction = esim->computeFunctionFit(set);
		//float FitScalingFactor = fittedFunction->GetParameter(1);
		//Float_t qIntegralReadoutValue = esim->integrateFunc(FitScalingFactor);
		y[i] = sumArray(set.digitalReadoutValues,10);
		//y[i] = qIntegralReadoutValue;
		x[i] = set.startingTime;
	}

	TGraph *graph = new TGraph(numTrials,x, y);
	graph->Draw("AP");
 	graph->SetLineColor(2);
    graph->SetLineWidth(4);
    graph->SetMarkerColor(4);
    //graph->SetMarkerStyle(21);
    graph->SetTitle("Sample Trial of Bit Value vs. Measurement Time");
    graph->GetXaxis()->SetTitle("startingTime (ns)");
    graph->GetYaxis()->SetTitle("initial readout value");




	//func = esim.computeFunctionFit(set);
	//func->Draw("same");

}