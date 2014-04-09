{

//13285
//15580
//17186

	treeData->GetEntry(4);

	for (int j = 0; j < 7; ++j){
		adc0 = adc[0][0];
		adcj = adc[0][j];
		if (mcenergy != 0.0)
		{
			eSimData.digitalReadoutValues[j] = (adcj- adc0)/mcenergy;
			eSimData.measurementTimes[j] = 20.0 * j;
		}

	}

}