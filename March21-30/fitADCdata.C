{


Float_t x[7], y[7];

for (int i = 0; i < 7; ++i){
	y[i] = eSimData.digitalReadoutValues[i];
	x[i] = eSimData.measurementTimes[i];
}

TGraph *graph = new TGraph(7,x, y);
graph->Draw("A*"); 

ElectronSimulation esim;

func = esim.computeFunctionFit(eSimData);
func->Draw("same");

}