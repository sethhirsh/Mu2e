{


Float_t x[7], y[7];
Float_t errorY[7] = {12.0, 12.0, 12.0, 12.0, 12.0, 12.0, 12.0};
Float_t errorX[7] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0};

for (int j = 0; j < 7; ++j){
		adc0 = adc[0][0];
		adcj = adc[0][j];
if (mcenergy != 0.0)
	{
			eSimData.digitalReadoutValues[j] = (adcj- adc0);
			eSimData.measurementTimes[j] = 20.0 * j;
		}

		}


for (int i = 0; i < 7; ++i){
	y[i] = eSimData.digitalReadoutValues[i];
	x[i] = eSimData.measurementTimes[i];
}

TGraphErrors *graph = new TGraphErrors(7,x, y,errorX, errorY);
graph->Draw("A*"); 

ElectronSimAnalysis esim;

/**TF1 *func = new TF1("func",esim.fittingFunction2,0,140,6);
func->SetParameters(45.0, computeMaxTrial(eSimData)/0.015, 0.0, 85.0, computeMaxTrial(eSimData)/0.015, 0.0);
func->SetParLimits(1, 0.1e5, 2.0e9);
func->SetParLimits(4, 0.5e5, 2.0e9);
func->SetParLimits(0, 0.0, 100.0);
func->SetParLimits(3, 30, 100.0);**/

TF1 * func = new TF1("fittingFunction", fittingFunction3, 0, 140, 6);
func->SetParameters(35.0, computeMaxTrial(eSimData)/0.015, 0.0, 5.0, computeMaxTrial(eSimData)/0.015, 0.0);
func->SetParLimits(3, 5.0, 200.0);
func->SetParLimits(1, 1.0, 2.0e9);

esim.computeFunctionFit(eSimData, func);
func->Draw("same");




}