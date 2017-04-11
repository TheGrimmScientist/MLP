
#include"trial.h"

#include<vector>
#include<string>

using namespace std;

//lots of potential in how to cut this.  the orignal goal is to make a "layer" just an organizer for a vector of nodes.  I would like to pull in the links too, but that would make NNS too specialized.  Right?

class data{
private:
	vector<int> type;	//-1 for output.  0 for ignore.  1 for input.
	vector<trial> info;

	//pointers marking the different data subsets:
	vector<int> train;	//on default, all data is used in train and test... althought this is traditionally not so.
	vector<int> test;
//	vector<int> validation;

	vector<int> readVecIntFile(string filename);
	void writeVecIntFile(vector<int>* toPrint, string filename);
	void read(ifstream&fin,int nTrials);
	bool grabPiece(char**line, double p);

	//normalization structures:
	void defaultNormalization(void);
	vector<double> xScaler, yScaler;
	vector<double> xOffset, yOffset;
	vector<double> xMin, yMin;

public:
	data(string filename, vector<int> typeVec, int nTrials);

	//print full dataset:
	void printScreenFormatted(void);
	void print(string filename);

	//train/test vector manipulation from API:
	void defaultDatasets(void);
	void setTrainSet(vector<int> trainSet);
	void setTestSet(vector<int> testSet);
	//train/test vector manipulation involving files
	void readTrainFile(string filename);
	void readTestFile(string filename);
	void saveTrainFile(string filename);
	void saveTestFile(string filename);

	//data retrieval functions.
	int getNTest(void);
	vector<double> getTestIn(int index);
	vector<double> getTestOut(int index);

	int getNTrain(void);
	vector<double> getTrainIn(int index);
	vector<double> getTrainOut(int index);

	//normalization of data:
	void normalize(double lLim, double uLim);
	vector<double> scale(bool isX, vector<double> toScale);
	vector<double> unScale(bool isX, vector<double> toUnScale);



//	randomizeOrder()	//all of these are relative to the order the data was imported.
//	saveOrder()
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//default: read full line
data::data(string filename, vector<int> typeVec, int nTrials){try{
	type = typeVec;

	ifstream fin;
	fin.open(filename.c_str());
	if(!fin.good())	throw("Invalid data file.  Thrown from data constructor.");
	read(fin,nTrials);
	defaultDatasets();	//set full dataset as train and test sets.
	defaultNormalization();
	fin.close();

}catch(const char *message){	cout << message << endl;	}}

void data::read(ifstream&fin,int nTrials){	//* change to string?
	int nVars = type.size();

	int v,t; double tempDouble;
	for(t=0;t<nTrials;t++){//for reach trial
		vector<double> varVec;
		for(v=0;v<nVars;v++){
			fin >> tempDouble;//grab each var
			varVec.push_back(tempDouble); //and stick it into the varVec.
		}
		//****** this requires nVars to equal the total number of columns.  to get around this, add in a "eat everything you see until you find the end of the line" here.  Otherwise, !(nCols==nVars will cause problems.
		trial tempTrial(varVec,type);
		info.push_back(tempTrial);//and add tempTrial into info.
	}
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////blah...////////////////////////////////////////////////////////////

void data::printScreenFormatted(void){int i;
	int nTrials = info.size();
	for(int t=0;t<nTrials;t++){
		vector<double> in = info[t].getX();
		vector<double> out = info[t].getY();
		int nX = in.size();	int nY = out.size();
		cout << "ins: ";	for(i=0;i<nX;i++) cout << in[i] << ' ';
		cout << "outs: ";	for(i=0;i<nY;i++) cout << out[i] << ' ';
		cout << endl;
	}
}
void data::print(string filename){int i;
	ofstream fout;
	fout.open(filename.c_str());

	int nTrials = info.size();
	for(int t=0;t<nTrials;t++){
		vector<double> in = info[t].getX();
		vector<double> out = info[t].getY();
		int nX = in.size();	int nY = out.size();
		fout << "ins: ";	for(i=0;i<nX;i++) fout << in[i] << ' ';
		fout << "outs: ";	for(i=0;i<nY;i++) fout << out[i] << ' ';
		fout << endl;
	}
}





////////////////////////////////////////////////////////////////////////////////////////
////////////////Data Recovery//////////////////////////////////////////////////////

int data::getNTrain(void){	return train.size();	}	//returns the number of train examples.
vector<double> data::getTrainIn(int index){		return scale(1, info[train[index]].getX());	}
vector<double> data::getTrainOut(int index){	return scale(0, info[train[index]].getY());	}

int data::getNTest(void) {	return test.size();		}	//returns the number of test examples.
vector<double> data::getTestIn(int index){	return scale(1, info[test[index]].getX());	}
vector<double> data::getTestOut(int index){	return scale(0, info[test[index]].getY());	}

////////////////////////////////////////////////////////////////////////////////////////
////////////////Train/Test set setting modification////////////////////////////////

void data::defaultDatasets(void){	//use full dataset as both train and test datasets.
	for(int i=0;i<info.size();i++){
		train.push_back(i);
		test.push_back(i);
	}
}

void data::setTrainSet(vector<int> trainSet){	train = trainSet;	}
void data::setTestSet(vector<int> testSet){		test = testSet;		}

void data::readTrainFile(string filename){	train = readVecIntFile(filename);	}
void data::readTestFile(string filename) {	test =  readVecIntFile(filename);	}
void data::saveTrainFile(string filename){	writeVecIntFile(&train, filename);	}
void data::saveTestFile(string filename) {	writeVecIntFile(&test , filename);	}

vector<int> data::readVecIntFile(string filename){try{
	ifstream fin;	int N,n,temp;	vector<int> tempVec;
	fin.open(filename.c_str());
	fin >> N;
	for(n=0;n<N;n++){
		if(!fin.good())	throw("data::readVecIntFile(string filename) | incomplete input file");
		fin >> temp;
		temp--;		//indexing in .vec file starts at 1.
		tempVec.push_back(temp);
	}
	fin.close();
	return tempVec;
}catch(const char*m){cout<<m<<endl;exit(0);}}

void data::writeVecIntFile(vector<int>* toPrint, string filename){
	ofstream fout;
	fout.open(filename.c_str());
	int N = toPrint->size();
	fout << N << endl;
	for(int n=0;n<N;n++)
		fout << toPrint->at(n) << endl;
	fout.close();
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////Normalization//////////////////////////////////////////////////////

void data::defaultNormalization(void){
	//clear old scaling data:
	xScaler.clear();	yScaler.clear();
	xOffset.clear();	yOffset.clear();
	xMin.clear();		yMin.clear();

	vector<double> tempX = info[0].getX();
	vector<double> tempY = info[0].getY();
	for(int c=0;c<tempX.size();c++){
		xScaler.push_back(1.0);
		xOffset.push_back(0.0);
		xMin.push_back(0.0);
	}
	for(int c=0;c<tempY.size();c++){
		yScaler.push_back(1.0);
		yOffset.push_back(0.0);
		yMin.push_back(0.0);
	}
}


void data::normalize(double lLim, double uLim){try{
	if(lLim>=uLim)	throw("data::normalize(lLim,uLim) | lower limit is >= the upper limit.");
	if(info.size()<2) throw("data::normalize(lLim,uLim) |  info.size()<2.  normalization non-sensical.");
	int t,c;

	//reset all class-level normalization data structures:
	xScaler.clear();	yScaler.clear();
	xOffset.clear();	yOffset.clear();
	xMin.clear();		yMin.clear();


	//determine max and min for each var.
	vector<double> xMax,yMax,tX,tY;	//(duh, duh, tempX, tempY).  xMin and yMin already initialized at the class level!!!!
	xMax = xMin = info[0].getX();	//set min and max to the first trial.
	yMax = yMin = info[0].getY();	
	int nX = xMax.size();//this, like the rest of the code, assumes that the number of ins and outs don't ever change.
	int nY = yMax.size();

	for(t=1;t<info.size();t++){	//for every other trial
		tX = info[t].getX();
		for(c=0;c<nX;c++){		//for each component in X
			if(xMax[c]<tX[c])		//if the max isn't max,
				xMax[c]=tX[c];		//set it to the new max.
			if(xMin[c]>tX[c])		//if the min isn't the new min,
				xMin[c]=tX[c];		//set it to the new min.
		}


		tY = info[t].getY();
		for(c=0;c<nY;c++){		//for each component in Y
			if(yMax[c]<tY[c])		//if the max isn't max,
				yMax[c]=tY[c];		//set it to the new max.
			if(yMin[c]>tY[c])		//if the min isn't the new min,
				yMin[c]=tY[c];		//set it to the new min.
		}

	}

	//calculate the magnitudes, scalers, and offsets.
	double outMag = uLim-lLim;
	vector<double> xMag, yMag;
	for(c=0;c<nX;c++){
		xMag.push_back(xMax[c]-xMin[c]);
		if(xMag[c]==0)	xScaler.push_back(0);
		else			xScaler.push_back(outMag/xMag[c]);
		xOffset.push_back(xMin[c] * xScaler[c] - lLim);
	}
	for(c=0;c<nY;c++){
		yMag.push_back(yMax[c] - yMin[c]);
		if(yMag[c]==0)	yScaler.push_back(0);
		else			yScaler.push_back(outMag/yMag[c]);
		yOffset.push_back(yMin[c] * yScaler[c] - lLim);
	}


/*
cout << "The xMin vector:" << endl;
for(c=0;c<nX;c++)
	cout << xMin[c] << ' ';
cout << endl<< "The yMin vector:" << endl;
for(c=0;c<nY;c++)
	cout << yMin[c] << ' ';

cout << endl << "The xMax vector:" << endl;
for(c=0;c<nX;c++)
	cout << xMax[c] << ' ';
cout << endl << "The yMax vector:" << endl;
for(c=0;c<nY;c++)
	cout << yMax[c] << ' ';
cout << endl;
*/
}catch(const char*m){cout<<m<<endl;exit(0);}}


vector<double> data::scale(bool isX, vector<double> toScale){
	for(int c=0;c<toScale.size();c++)//for each 
	if(isX){	//if this is an X (input to the network)
		toScale[c]= toScale[c] * xScaler[c] - xOffset[c];
	}else{	//if this is a Y (output from the network)
		toScale[c]= toScale[c] * yScaler[c] - yOffset[c];
	}
	return toScale;
}
vector<double> data::unScale(bool isX, vector<double> toUnScale){
	for(int c=0;c<toUnScale.size();c++)//for each 
	if(isX){	//if this is an X (input to the network)
		if(xScaler[c]==0)
			toUnScale[c] = xMin[c];
		else
			toUnScale[c] = (toUnScale[c] + xOffset[c]) / xScaler[c];
	}else{	//if this is a Y (output from the network)
		if(yScaler[c]==0)
			toUnScale[c] = yMin[c];
		else
			toUnScale[c] = (toUnScale[c] + yOffset[c]) / yScaler[c];
	}
	return toUnScale;
}
