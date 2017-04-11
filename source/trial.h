
#include<vector>

using namespace std;

// *** this assumes all data to be best represented by "double".  fuck that.  at some point in the nearish future, make it a template so it can use things like, say, int and bool.  right?  

// **************  what about normalization?  have this bitch normalize all data according to the respecive xFerType and then un-normlize the output behind the scenes?

class trial{
private:

	vector<double> compVec;
	vector<int> typeVec;  // -1 for output.  0 for ignore.  1 for input.

public:

	trial(vector<double> info); // for just submitting the data with using the default.. which is: last entry is y.
	trial(vector<double> info, int nX, int nY); //trivial case. first nX are the inputs.  The rest (equalling nY) = output
	trial(vector<double> info, vector<int> in, vector<int> out); //info with lists of the indices to use for in and out.
	trial(vector<double> info, vector<int> type);//directly import the component and type vectors.

	void getTrial(vector<double>*X, vector<double>*Y);
	vector<double> getX(void);
	vector<double> getY(void);

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////Constructors and data-input function/////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

trial::trial(vector<double> info){try{
	if(info.size()<2)	throw(0); //you need to have at least one input and at least one output.


}catch(int errorCode){
cout << "Error in trial::trial(vector<double> xAndY). Error code " << errorCode << endl;
}}

////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

trial::trial(vector<double> info, int nX, int nY){try{
	if(info.size()<2)			throw(0); //you need to have at least one input and at least one output.
	if(info.size()!=(nX+nY))	throw(101);
	int i;

	compVec = info;
	for(i=0;i<nX;i++)		typeVec.push_back(1);
	for(i=nX;i<(nX+nY);i++)	typeVec.push_back(1);

}catch(int errorCode){
cout << "Error in trial::trial(vector<double> info, int nX, int nY). Error code " << errorCode << endl;
}}

////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

trial::trial(vector<double> info, vector<int> type){try{
	if(info.size()<2)				throw(0);
	if(info.size()!=type.size())	throw(100);

	compVec = info;
	typeVec = type;  // -1 for output.  0 for ignore.  1 for input.

}catch(int errorCode){
cout << "Error in trial::trial(vector<double> info, vector<int> type). Error code " << errorCode << endl;
}}

////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

trial::trial(vector<double> info, vector<int> in, vector<int> out){ try{
	if(info.size()<2)				throw(0);
	int i;

	compVec = info;
//	typeVec = type;  // -1 for output.  0 for ignore.  1 for input.

	for(i=0;i<compVec.size();i++)	typeVec.push_back(0);			//set whole vector to 0.
	for(i=0;i<in.size();i++)			typeVec[in[i]]=1;		//pull in the "in"
	for(i=0;i<out.size();i++)			typeVec[out[i]]=-1;	//pull in the "out"

	throw(666);
	/// ****** function not codded yet!

}catch(int errorCode){
cout << "Error in trial::trial(vector<double> info, vector<int> in, vector<int> out). Error code " << errorCode << endl;
}}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////Data-output functions////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void trial::getTrial(vector<double>*X, vector<double>*Y){try{
	int i;

/*	for(i=0;i<compVec.size();i++)
		if(typeVec[i]==1)
			&X.push_back(compVec[i]);
		else if(typeVec[i]==-1)
			&Y.push_back
*/
	throw(666);
}catch(int errorCode){
cout << "Error in trial::getTrial. Error code " << errorCode << endl;
}}

////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

vector<double> trial::getX(void){
	vector<double> tempVec;
	for(int i=0;i<compVec.size();i++)
		if(typeVec[i]==1)
			tempVec.push_back(compVec[i]);
	return tempVec;
}

////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

vector<double> trial::getY(void){
	vector<double> tempVec;
	for(int i=0;i<compVec.size();i++)
		if(typeVec[i]==-1)
			tempVec.push_back(compVec[i]);
	return tempVec;
}

