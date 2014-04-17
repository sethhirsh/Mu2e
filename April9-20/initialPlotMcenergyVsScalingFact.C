{
	// First run getADCdata.C

	gr = TGraph(24714, scalingFact, mcenergies);
	gr.SetTitle("Initial Plot Mcenergy vs Scaling Fit Factor"); 
	gr.Draw("A*");

}