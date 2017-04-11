

#include"backProp.h"
#include"data.h"

#include<vector>
#include<string>
#include<iostream>
#include<fstream>

using namespace std;

class experiment{
private:

	backprop *network;	//**make this templated to work with any type of network.
	data * info;

//training settings
	int epochSize;
	int nIters; //in case of backprop, this is the number of epochs training is ran for.
	double learnRate;

//data settings
	vector<int> type; //-1 for output.  0 for ignore.  1 for input.

//training record keeping.
//	vector<bool> classError;  //???
	vector<vector<double> > Y;
	vector<double> rmsErrorBad;	//each entry represents one epoch.
//oficially, "tracking"

	void track(int currentCycle);
	void printTrackFile(char * filename);
	int trackEvery;
	vector<int> cycle;
	vector<double> rmsErrorTrain;
	vector<double> rmsErrorTest;
	vector<double> cRateTrain;
	vector<double> cRateTest;


	double testAll(void);				//for testing on all trials.
	void printTestResults(bool unScale);
	void printTestResults(string filename, bool unScale);
	void printLearningCurve(void);
	void printLearningCurve(string filename);
//	vector<double> runNet(vector<int> trial);	//for running only specific trials
	void train(int nEpochs);


	bool grabLine(ifstream &fin, string*lineStr, char*word[100]);
	double calcRMS(vector<double> *actualOut,vector<double> *targetOut);


	//classification:
	int defaultCStrict;
	int classify(vector<double> *toClassify);
	int classify(vector<double> *toClassify, int strict);

	//variable reduction experiment:
	void singleVarTest(string fileStr);

public:
	experiment(string filename);	//load the .exp
	experiment( void ); 			//run the UI - *pending.
	~experiment(void);	//destructor.


};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


experiment::experiment(string filename){try{
	network = NULL;
	info = NULL;
	char line[100];	char fileChArray[100]; char chArray[100];	char *word; word = new char[100];
	string lineStr, wordStr, fileStr, str, typeFile;
	int a, b, c, d, i;	double f1,f2;	int nR;  //number read (for sscanf).
	char rmsFilename[100];
	char trainedNetFilename[100];
	char outputFilename[100];	
	char trackFilename[100];

	//defaults:
	int seed = 999;
	learnRate=0.05;
	trackEvery = 0;
	defaultCStrict = 1;




	ifstream fin;
	fin.open(filename.c_str());
	
//	while(fin.good()){	if(lineStr.size()>0){

		//first read type of network.//////////////////////////////////////////////
		
		grabLine(fin, &lineStr, &word);
		if(!strcmp(word,"backprop")){


			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"seed")){
				sscanf(lineStr.c_str(),"%d",&seed);
			}else throw("Expected \"seed\" under backprop's settings.");


			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"topology3")){
				nR=sscanf(lineStr.c_str(),"%d %d %d",&a,&b,&c);
				if(nR!=3) throw("exp:exp() | invalid number of parameters following \"topology3\" in backprop settings");
				network = new backprop(a, b, c, seed);	//build network
			}else if(!strcmp(word,"topology4")){
				nR=sscanf(lineStr.c_str(),"%d %d %d %d",&a,&b,&c,&d);
				if(nR!=3) throw("exp:exp() | invalid number of parameters following \"topology4\" in backprop settings");
				network = new backprop(a, b, c, d, seed);	//build network
			}else throw("Invalid backprop topology.");


		}else  throw("Invalid network type given in experiment file.");


		//then read the data's format/settings/////////////////////////////////////
		grabLine(fin, &lineStr, &word);
		if(!strcmp(word,"data")){
			int nTrials;
			grabLine(fin, &lineStr, &word);
			if(strcmp(word,"format") == 0){
				sscanf(lineStr.c_str(),"%d %d",&a, &b);	
				for(i=0;i<a;i++) type.push_back(1);  for(i=0;i<b;i++) type.push_back(-1); //construct "type" vector
			}else if(strcmp(word,"typeFile") == 0){//this is a hack.  should just pass the filename into data class to handle.
				sscanf(lineStr.c_str(),"%s",fileChArray);	typeFile = fileChArray;
				int N, tempInt;
				ifstream fin;
				fin.open(typeFile.c_str());
				fin >> N;//read N
				for(int n=0;n<N;n++){
					fin >> tempInt;
					type.push_back(tempInt);
				}
				fin.close();
			}else throw("Expected to read format type under data's settings.");
        

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"nTrials")){
				sscanf(lineStr.c_str(),"%d",&nTrials);
			}else throw("Expected to read nTrials under data's settings.");

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"datFile")){
				sscanf(lineStr.c_str(),"%s",fileChArray);	fileStr = fileChArray;
			}else throw("Expected to read format type under data's settings.");

			//build info:
			info = new data(fileStr,type,nTrials);

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"normalize")){
				sscanf(lineStr.c_str(),"%s",chArray);	str = chArray;
				if(!strcmp(chArray,"none")){}		//if "none" is said, then do nothing.
				else if(!strcmp(chArray,"unit")){
					info->normalize(0,1);  //call normalization function for unit interval
				}else if(!strcmp(chArray,"custom")){
					sscanf(lineStr.c_str(),"%*s %lf %lf",&f1,&f2);
					info->normalize(f1,f2);  //call normalization function for unit interval
				}else
					throw("Invalid normalization parameter given in .exp file.");
			}else throw("Expected to read \"normalize\" under data's settings.");




		}else  throw("Invalid experiment file section.  Expected to read \"data\".");


		//then read the training settings//////////////////////////////////////////
		grabLine(fin, &lineStr, &word);
		if(!strcmp(word,"train")){

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"trainSet")){
				nR=sscanf(lineStr.c_str(),"%s",chArray);	str = chArray;
				if(nR!=1) throw("AAA");
				if(!strcmp(chArray,"default")){throw("Setting not yet written");}		//if "default" is said, then do nothing.
				else if(!strcmp(chArray,"random")){throw("Setting not yet written");}	//if "random, then .... for now, do nothing.
				else								//otherwise, use this string as a filename
					info->readTrainFile(str);
			}else throw("Expected \"trainSet\" under train settings.");

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"testSet")){
				nR=sscanf(lineStr.c_str(),"%s",chArray);	str = chArray;
				if(nR!=1) throw("AAB");
				if(!strcmp(chArray,"default")){throw("Setting not yet written");}		//if "default" is said, then do nothing.
				else if(!strcmp(chArray,"random")){throw("Setting not yet written");}	//if "random, then .... for now, do nothing.
				else								//otherwise, use this string as a filename
					info->readTestFile(str);
			}else throw("Expected \"testSet\" under train settings.");

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"nTrainIters")){
				nR=sscanf(lineStr.c_str(),"%d",&nIters);
				if(nR!=1) throw("AAC");
			}else throw("Expected \"nTrainIters\" under train settings.");

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"epochSize")){
				sscanf(lineStr.c_str(),"%d",&epochSize);
				if(epochSize<1) throw("exp::exp() | invalid epochSize");
			}else throw("Expected \"epochSize\" under train settings.");

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"learnRate")){
				sscanf(lineStr.c_str(),"%lf",&learnRate);
				learnRate = learnRate/epochSize;
			}else throw("Expected to read \"epochSize\" under train settings.");

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"track")){
				sscanf(lineStr.c_str(),"%s",chArray);
				if(!strcmp(chArray,"none")){
					trackEvery = 0;
				}
				else{
					nR = sscanf(lineStr.c_str(),"%s %d",trackFilename, &trackEvery);
					if(nR!=2) throw("exp::exp | invalid number of params passed after \"track\"");
				}
			}else throw("Expected to read \"track\" under train settings.");

			//run training.
			train(nIters);			
			if(trackEvery>0)	printTrackFile(trackFilename);

		}else  throw("Invalid experiment file section.  Expected to read \"train\".");



		//then read the save settings//////////////////////////////////////////
		grabLine(fin, &lineStr, &word);
		if(!strcmp(word,"save")){

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"rms")){
				sscanf(lineStr.c_str(),"%s",fileChArray);	fileStr = fileChArray;
				if(strcmp(fileChArray,"none"))
					printLearningCurve(fileStr);
			}else throw("Expected to read \"rms\" type under data's settings.");

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"network")){
				sscanf(lineStr.c_str(),"%s",fileChArray);	fileStr = fileChArray;
				if(strcmp(fileChArray,"none"))
					network->print(fileStr);
			}else throw("Expected to read \"network\" type under data's settings.");

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"output")){
				sscanf(lineStr.c_str(),"%s",fileChArray);	fileStr = fileChArray;
				if(strcmp(fileChArray,"none")){
					testAll();
					printTestResults(fileStr,true);	//info->print(fileStr);
				}
			}else throw("Expected to read \"output\" type under data's settings.");

		}else  throw("Invalid experiment file section.  Expected to read \"train\".");


		//then read the save settings//////////////////////////////////////////
		grabLine(fin, &lineStr, &word);
		if(!strcmp(word,"varReduction")){

			grabLine(fin, &lineStr, &word);
			if(!strcmp(word,"single")){
				sscanf(lineStr.c_str(),"%s",fileChArray);	fileStr = fileChArray;
				if(strcmp(fileChArray,"none"))
					singleVarTest(fileStr);
			}else throw("Expected to read \"rms\" type under data's settings.");

		}else  throw("Invalid experiment file section.  Expected to read \"varReduction\".");




	//retest network.
	c = cycle.size()-1;
	if(c<0)
		cout << "no recorded data."<<endl;
	else
		cout << "cycle "<<cycle[c]<<" rmsTrain "<<rmsErrorTrain[c]<<" rmsTest "<<rmsErrorTest[c]<<" cRTrain "<<cRateTrain[c]<<" cRTest "<<cRateTest[c]<< endl;


	delete [] word;
}catch(char const *message){
	cout << "error: " << message << endl;
}}


experiment::~experiment(void){
	if(info!=NULL)
		delete info;
	if(network!=NULL)
		delete network;


}

////////////////////////////////////////////////////////////////////////////////////////
////////////////grabLine()/////////////////////////////////////////////////////////

bool experiment::grabLine(ifstream &fin, string*lineStr, char **word){
	char line[100];//, word[100];

	//grab line
	fin.getline(line,100);  *lineStr = line;	
	//remove initial whitespace (if any)
	while(((*lineStr)[0]==' ')||((*lineStr)[0]=='	'))lineStr->erase(0,1);

	//grab word.
	sscanf(lineStr->c_str(),"%s",*word);	

	// and cut word from line.
	lineStr->erase(0,strlen(*word));	//grab (and erase) word from line.

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////Train/test functions/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
////////////////testAll()//////////////////////////////////////////////////////////

double experiment::testAll(void){try{
	if(network==NULL) throw("in experiment object, testAll(): no network to test.");
	if(info==NULL)	throw("in experiment object, testAll(): no data to test with.");

	bool pF = false;
	double error,d;	//errror, difference
	int t,i;	//trial, index
	Y.clear();	//clear Y in case eval was already ran.

	error = 0;
	vector<double> in, targetOut, actualOut;
	for(t=0;t<info->getNTest();t++){
								if(pF) {	cout << "trial " << t << endl;	}
		//gather i/o data:
		in = info->getTestIn(t);
								if(pF){cout<<endl<<"  in= "; for(i=0;i<in.size();i++) cout<<in[i]<<' ';}
		targetOut = info->getTestOut(t);
								if(pF){cout<<endl<<"  tOut= "; for(i=0;i<targetOut.size();i++) cout<<targetOut[i]<<' ';}

		//run network to recover real output.
		actualOut = network->run(&in);
								if(pF){cout<<endl<<"  aOut= "; for(i=0;i<actualOut.size();i++) cout<<actualOut[i]<<' ';}
		Y.push_back(actualOut);

		//compare target and actual outputs.
		error += calcRMS(&actualOut,&targetOut);//and make "error" be the accumultion of each sample's rms error.
								if(pF){cout<<endl<<"  error = "<<calcRMS(&actualOut,&targetOut)<<endl;}
	}
	error /= info->getNTest();//make "error" be the _average_ of the sample errors.
	return error;
//return the rms error of training set and save the actual outputs for potential printing.
}catch(const char*m){cout<<m<<endl;exit(0);}}

////////////////////////////////////////////////////////////////////////////////////////
////////////////printTestResults()/////////////////////////////////////////////////

void experiment::printTestResults(bool unScale){try{	//*****pass parameter that says to scale or not???
	if(Y.empty())	throw("expriment.h printTestResults(void) | no output to print.");	//make sure Y is populated...

	int t,i;	vector<double> temp;
	//print actual and target outputs
	for(t=0;t<Y.size();t++){	//for each trial
		cout << "target: ";//give the target outputs
		temp = info->getTestOut(t);
		if(unScale)	temp = info->unScale(0,temp);
		for(i=0;i<temp.size();i++)
			cout << temp[i] << ' ';

		cout << "actual: ";//then give the actual outputs
		temp = Y[t];
		if(unScale)	temp = info->unScale(0,temp);
		for(i=0;i<temp.size();i++)
			cout << temp[i] << ' ';
		cout << endl;
	}
	//confusion matrix??

}catch(const char*m){cout<<m<<endl;exit(0);}}

void experiment::printTestResults(string filename, bool unScale){try{
	if(Y.empty())	throw("expriment.h printTestResults(string filename) | no output to print.");

	ofstream fout;	fout.open(filename.c_str());

	int t,i;	vector<double> tT,tA;//tempTarget, tempActual.
	//print actual and target outputs
	for(t=0;t<Y.size();t++){	//for each trial
		fout << "target: ";//give the target outputs
		tT = info->getTestOut(t);
		if(unScale)	tT = info->unScale(0,tT);
		for(i=0;i<tT.size();i++)
			fout << tT[i] << ' ';

		fout << "actual: ";//then give the actual outputs
		tA = Y[t];
		if(unScale)	tA = info->unScale(0,tA);
		for(i=0;i<tA.size();i++)
			fout << tA[i] << ' ';
		fout << endl;
//		fout << "  tClass: "<<classify(&tT)<<" aClass: "<<classify(&tA)<<endl;
	}
	//confusion matrix??

}catch(const char*m){cout<<m<<endl;exit(0);}}



////////////////////////////////////////////////////////////////////////////////////////
////////////////printLearningCurve()///////////////////////////////////////////////

void experiment::printLearningCurve(void){try{
	if(rmsErrorBad.size()==0)	throw("experiment.h printLearningCurve() | no learning curve to print");

	for(int i=0;i<rmsErrorBad.size();i++)
		cout << i << ' ' << rmsErrorBad[i] << endl;

}catch(const char*m){cout<<m<<endl;}}

void experiment::printLearningCurve(string filename){try{
	if(rmsErrorBad.size()==0)	throw("experiment.h printLearningCurve() | no learning curve to print");

	ofstream fout;
	fout.open(filename.c_str());

	for(int i=0;i<rmsErrorBad.size();i++)
		fout << i << ' ' << rmsErrorBad[i] << endl;

}catch(const char*m){cout<<m<<endl;}}

////////////////////////////////////////////////////////////////////////////////////////
////////////////train(int nEpochs)/////////////////////////////////////////////////

void experiment::train(int nEpochs){try{
//	if(network==NULL) throw("in experiment object, testAll(): no network to test.");
//	if(info==NULL)	throw("in experiment object, testAll(): no data to test with.");

//	double error,d;	//errror, difference
	double error;
	int e,c;	//epoch, cycle (training iteration within epoch).
//	Y.clear();	//clear Y in case eval was already ran.

	int trPtr = 0;	//trial pointer.
	vector<double> in, targetOut, actualOut;
	for(e=0;e<nEpochs;e++){

		error = 0;
		for(c=0;c<epochSize;c++){ //for each train cycle within the epoch
			//recover relevant (i/o) data:
			in = info->getTrainIn(trPtr);
			targetOut = info->getTrainOut(trPtr);

			//run backprop alg once:
			actualOut =  network->propagateError(&in, targetOut);
			error += calcRMS(&actualOut,&targetOut);

			//and tend to the trial pointer:
			trPtr++;
			if(trPtr>=info->getNTrain()) trPtr=0;  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		}
		error /= epochSize;	//take the average RMS error across the iterations in this epoch.
		network->updateWeights(learnRate);
//** re-run classification of the above trials??

		if( ( (e%trackEvery) == 0) && (trackEvery!=0) )
			track(e);
		rmsErrorBad.push_back(error);
		//save a different version of rms error where it's tested on the whole set?*

	}

	//** if any of the tracking variables are not emtpy,
	//print the tracking file.

}catch(const char*m){cout<<m<<endl;}}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////General Utilities////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


double experiment::calcRMS(vector<double> *actualOut,vector<double> *targetOut){try{
//cout << "actualOut = ";   for(int z=0;z<(*actualOut).size();z++) cout <<(*actualOut)[z] <<' ';
//cout << "  targetOut = "; for(int z=0;z<(*targetOut).size();z++) cout <<(*targetOut)[z] <<' ';

	if(actualOut->size()!=targetOut->size()) 
		throw("experiment.h calcRMS | non-matching vector size on passed parameters");

	int i;	double d;

	double sampleError = 0;
	for(i=0;i<targetOut->size();i++){
		d = actualOut->at(i) - targetOut->at(i);//difference = actual - target;   //calcualte error
		d *= d;	// sqaured error
		sampleError += d;	//sum of all error
	}

	sampleError /= targetOut->size();	//means squared error
	sampleError = sqrt(sampleError);	//root means squared (RMS) error
//cout << "error: "<< sampleError << endl;
	return sampleError;

}catch(const char*m){cout<<m<<endl;}}

//classification:
int experiment::classify(vector<double> *toClassify){	return classify(toClassify,defaultCStrict);	} //default strict is '3'.
int experiment::classify(vector<double> *toClassify, int strict){
	//1 - loose: return the highest as the classification. 
	//2 - medium: return class if one is above uConf.
	//3 - tight: only return a class is one is above uConf and all others are below lConf. ret -1 if classes are equal.
	int i;	//index,
	int C;	//class.

	double max = 1.0;
	double min = 0.0;
	double uConf = max - ( (max-min)/3 );//confidence limit for upper end.  - 1/3d down from top.
	double lConf = min + ( (max-min)/3 ); //confidence limit for lower end.  - 1/3rd up from bottom.

	if(strict==3){
//		vector<bool> confidentYes;	vector<bool> confidentNo;
		int nCNo  = 0;	int nCYes = 0;
		//determine which "bin" each piece falls into.
		for(i=0;i<toClassify->size();i++){
			if(toClassify->at(i) < uConf){}
//				confidentYes.push_back(false);
			else{
//				confidentyes.push_back(true);
				nCYes++;
				C=i;
			}
			if(toClassify->at(i) > lConf){}
//				confidentNo.push_back(false);
			else{
//				confidentNo.push_back(true);
				nCNo++;
			}
		}

		if(nCYes>1)	C = -1;//if nCYes is >1, then there is too much confusion.
		if( (nCYes+nCNo) != toClassify->size() )	C= -1;	//if something wasn't confidently classified.

	}else if(strict==2){
		vector<bool> confidentYes;	int nCYes =0;
		//determine which "bin" each piece falls into.
		for(i=0;i<toClassify->size();i++)
			if(toClassify->at(i) < uConf)
				confidentYes.push_back(false);
			else{
				confidentYes.push_back(false);
				nCYes++;
				C=i;
			}
		if(nCYes>1)	C = -1;//if nCYes is >1, then there is too much confusion.
	}else{  //assume it equals 1.
		C = 0;
		i=0;
		double val = toClassify->at(i);
		for(i=1;i<toClassify->size();i++)
			if(val < toClassify->at(i)){
				val=toClassify->at(i);
				C=i;
			}//else if(val==toClassify->at(i))
			//	C= -1;
	}

	return C;
}


void experiment::track(int currentCycle){
	double tempError, tempCR,d;	//errror, difference
	int t,i;	//trial, index
	int aC,tC; //actual class, target class.

	// record cycle
	cycle.push_back(currentCycle);

	tempError = 0;
	tempCR = 0;
	vector<double> in, targetOut, actualOut;

	// solve for and record info from train set trackers.
	for(t=0;t<info->getNTrain();t++){
		//gather i/o data:
		in = info->getTrainIn(t);			//recover input
		targetOut = info->getTrainOut(t);	//recover target out
		actualOut = network->run(&in);		//solve for actual out

		//compare target and actual outputs.
		tempError += calcRMS(&actualOut,&targetOut);//and make "error" be the accumultion of each sample's rms error.
		aC = classify(&actualOut);	tC = classify(&targetOut);
		if( (aC==tC) && (aC!=(-1)) )	tempCR++;	//if classes match and aren't ambiguous, increment. else, leave as is.
	}
	tempError /= info->getNTrain();//make "error" be the _average_ of the sample errors.
	tempCR    /= info->getNTrain();
	rmsErrorTrain.push_back(tempError);
	cRateTrain.push_back(tempCR);


	// solve for and record info from test set trackers.
	tempError = 0;
	tempCR = 0;
	for(t=0;t<info->getNTest();t++){
		//gather i/o data:
		in = info->getTestIn(t);			//recover input
		targetOut = info->getTestOut(t);	//recover target out
		actualOut = network->run(&in);		//solve for actual out

		//compare target and actual outputs.
		tempError += calcRMS(&actualOut,&targetOut);//and make "error" be the accumultion of each sample's rms error.
		aC = classify(&actualOut);	tC = classify(&targetOut);
		if( (aC==tC) && (aC!=(-1)) )	tempCR++;	//if classes match and aren't ambiguous, increment. else, leave as is.
	}
	tempError /= info->getNTest();//make "error" be the _average_ of the sample errors.
	tempCR    /= info->getNTest();
	rmsErrorTest.push_back(tempError);
	cRateTest.push_back(tempCR);


}
void experiment::printTrackFile(char * filename){
	ofstream fout;
	fout.open(filename);
	fout << "cycle	rmsErrorTrain	rmsErrorTest	cRateTrain	cRateTest"<<endl;
	for(int i=0;i<cycle.size();i++){
		fout << cycle[i] << '	';
		fout << rmsErrorTrain[i] << '	';
		fout << rmsErrorTest[i] << '	';
		fout << cRateTrain[i] << '	';
		fout << cRateTest[i] << '	';
		fout << endl;
	}
	fout.close();
}

void experiment::singleVarTest(string fileStr){
	double tempError, tempCR,d;	//errror, difference
	int t,i;	//trial, index
	int aC,tC; //actual class, target class.

	//storage for this test's data.
	vector<double> rmsTrainMissing;
	vector<double> cRTrainMissing;
	vector<double> rmsTestMissing;
	vector<double> cRTestMissing;

	vector<double> in, targetOut, actualOut;

//	in = info->getTrainIn(t);  // why was this ever here?  Seems it should be deleted... - commented out by AG 2017
	int nX=in.size();	int x;
	for(x=0;x<nX;x++){
		tempError = 0;
		tempCR = 0;
		// solve for and record info from train set trackers.
		for(int val=0;val<=1;val++){
			for(t=0;t<info->getNTrain();t++){
				//gather i/o data:
				in = info->getTrainIn(t);			//recover input
				in[x]=(float)val;					//corrupt x by planting "((float)val)" into spot x
				targetOut = info->getTrainOut(t);	//recover target out
				actualOut = network->run(&in);		//solve for actual out

				//compare target and actual outputs.
				tempError += calcRMS(&actualOut,&targetOut);
				aC = classify(&actualOut);	tC = classify(&targetOut);
				if( (aC==tC) && (aC!=(-1)) )	tempCR++;
			}
		}
		tempError /= (info->getNTrain()*2);//make "error" be the _average_ of the sample errors.
		tempCR    /= (info->getNTrain()*2);
		rmsTrainMissing.push_back(tempError);
		cRTrainMissing.push_back(tempCR);

		tempError = 0;
		tempCR = 0;
		for(int val=0;val<=1;val++){
			for(t=0;t<info->getNTest();t++){
				//gather i/o data:
				in = info->getTestIn(t);			//recover input
				in[x]=(float)val;					//corrupt x by planting "((float)val)" into spot x
				targetOut = info->getTestOut(t);	//recover target out
				actualOut = network->run(&in);		//solve for actual out

				//compare target and actual outputs.
				tempError += calcRMS(&actualOut,&targetOut);
				aC = classify(&actualOut);	tC = classify(&targetOut);
				if( (aC==tC) && (aC!=(-1)) )	tempCR++;
			}
		}
		tempError /= (info->getNTest()*2);//make "error" be the _average_ of the sample errors.
		tempCR    /= (info->getNTest()*2);
		rmsTestMissing.push_back(tempError);
		cRTestMissing.push_back(tempCR);

	}	//end for(eachInput)

	//append this data to specified file - for now it's the classification rate across the whole set.
	fstream fid;
	fid.open(fileStr.c_str(), fstream::out | fstream::app);
	for(x=0;x<nX;x++){
		d = ( (cRTestMissing[x] * info->getNTest()) + (cRTrainMissing[x] * info->getNTrain()) ) / (info->getNTest() + info->getNTrain() );
		fid << d << '	';
	}
	fid << endl;
	fid.close();
}

