{
/**	ElectronSimulation esim;
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
    graph->GetYaxis()->SetTitle("initial readout value");**/

  /**  ElectronSimAnalysis esim;
    TF1 *func = new TF1("func",esim.fittingFunction2,0,140,3);
    func->SetParameters(45.0, 2.75e7, 0.0); **/

   // for (int i = 0; i < treeData->GetEntries(); ++i){ if (scalingFact[i] < 1.0e3 || scalingFact[i] > 1.0e6){cout << i << endl;} }
   for (int i = 0; i < treeData->GetEntries(); ++i)
   {
   	if (scalingFact[i] < 100.0e3 && mcenergies[i] > 0.005)
   	{
   		cout << i << " : " << scalingFact[i] << " : " << mcenergies[i] << endl;
   	}
   }



	//func = esim.computeFunctionFit(set);
	//func->Draw("same");

}