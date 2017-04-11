

#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include"link.h"
#include"bpNode.h"
#include"layer.h"
#include"node.h"


#define DSFMT_MEXP 19937	//make period bigger or smaller as desired.
#include "dSFMT/dSFMT.c"

using namespace std;

class backprop{
private:
//NN settings
	int xFerType;

//functionality
	dsfmt_t rng;

//NN structure
	vector<layer> layerVec;
	vector<node> nodeVec;
	vector<bpLink> linkVec;
	//data interface?


	void constructNodes(void);
	void constructLinks(void);

	void placeLink(int fromNode, int toNode);	//w is randomly assigned.
	void placeLink(int fromNode, int toNode, double w);
	vector<double> f(vector<double> *X);

public:

	//construct via API (probably Experiment class)
	void setSeed(int seed);
	backprop(int front, int hidden, int back, int seed); //special case for 1 hidden layer
	backprop(int front, int hidden1, int hidden2, int back, int seed); //special case for 2 hidden layers

	//File Interfaces
	backprop(string filename); //read network from file.
	void read(std::ifstream& fin);
	void print(string filename);
	void print(std::ofstream& fout);


	//train/test network:
	vector<double> run(vector<double> *X);
	vector<double> propagateError(vector<double>* X, vector<double> idealY);
	void updateWeights(double learningRate);  //after epochSize training cycles, this is to be called to update weights.


};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////Network Construction/Printing////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
backprop::backprop(string filename){
	ifstream a;
	a.open(filename.c_str());
	read(a);
	a.close();
}
void backprop::read(std::ifstream& fin){try{
	int i,f,t,l,n,nL,nN;	char line[100];	int nNodes=0; float w;


	fin.getline(line,100);
	sscanf(line, "xFerFunc is %d", &xFerType);

	fin.getline(line,100);
	sscanf(line, "number of layers = %d", &nL);

	for(l=0;l<nL;l++){		//in both cases of returning 'i', it can be used for error checking but should never be needed
		fin.getline(line,100);
		sscanf(line, "number of nodes in layer %d is %d", &i, &nN);
		//if(i==l)  {all is good}

		//create layer
		layer tempLayer(nN,&nNodes);	layerVec.push_back(tempLayer);

		//add nodes
		for(n=0;n<nN;n++){
//			i = layerVec[l].getIndex(n);
			//if(i==nodeVec.size()) {all is good}
			node tempNode(xFerType);
			nodeVec.push_back(tempNode);
		}
	}

	fin.getline(line,100);	//read the number of links.
		if(line[0]!='#') throw("backprop.h read(fin) | expected to see # in file.net");

	fin.getline(line,100);
	sscanf(line,"%d",&nL);

	for(l=0;l<nL;l++){	//for each link,
		fin.getline(line,100);
		sscanf(line,"%d %d %d %f", &i, &f, &t, &w);    //index, fromNode, toNode, weight.
		placeLink(f,t,w);
	}

}catch(const char*m){cout<<m<<endl;exit(0);}}


backprop::backprop(int front, int hidden, int back, int seed){
	setSeed(seed);
	xFerType = 1;
	int nNodes = 0;
	//create layers.  these are used to as a reference for the node locations.
	//if I make my own function for building a layer and, within that, include the node construction and link placements, then this may be cleaner..
	layer firstLayer(front,&nNodes);	layerVec.push_back(firstLayer);
	layer hiddenLayer(hidden,&nNodes);	layerVec.push_back(hiddenLayer);
	layer backLayer(back,&nNodes);		layerVec.push_back(backLayer);

	constructNodes();
	constructLinks();

}

backprop::backprop(int front, int hidden1, int hidden2, int back, int seed){
	setSeed(seed);
	xFerType = 1;

	int nNodes = 0;
	//create layers.  these are used to as a reference for the node locations.  ..may not even be needed.
	layer firstLayer(front,&nNodes);		layerVec.push_back(firstLayer);
	layer hiddenLayer1(hidden1,&nNodes);	layerVec.push_back(hiddenLayer1);
	layer hiddenLayer2(hidden2,&nNodes);	layerVec.push_back(hiddenLayer2);
	layer backLayer(back,&nNodes);			layerVec.push_back(backLayer);

	constructNodes();
	constructLinks();
}

void backprop::constructNodes(void){
	int l,n,i;

	for(l=0;l<layerVec.size();l++)
		for(n=0;n<layerVec[l].getNNodes();n++){
			i = layerVec[l].getIndex(n);
			node tempNode(xFerType);
//			if(i!=layerVec[l].getIndex(n))
//				cout << "ERROR in constructor. Indexing of nodes not matching indexing within layers." << endl;
			nodeVec.push_back(tempNode);
		}
}
void backprop::constructLinks(void){
	int l,nA,nB,n;

	//make connections from one layer to the next.
	for(l=0;l<layerVec.size()-1;l++)//for each layer A that has a next layer B
		for(nA=0;nA<layerVec[l].getNNodes();nA++)//for each node in Layer A
			for(nB=0;nB<layerVec[l+1].getNNodes();nB++)//for each node in Layer B
				placeLink(layerVec[l].getIndex(nA),layerVec[l+1].getIndex(nB));	//make a connection

	//add in the bias for every layer but the first.
	for(l=1;l<layerVec.size();l++)	//for each layer.
		for(n=0;n<layerVec[l].getNNodes();n++)//for each node within that layer
			placeLink(l*(-1),layerVec[l].getIndex(n)); //add a bias link "from the layer" to said node.
}














////////////////////////////////////////////////////////////////////////////////////////
////////////////print()////////////////////////////////////////////////////////////

void backprop::print(string filename){

	ofstream a;
	a.open(filename.c_str());
	print(a);
	a.close();
}


void backprop::print(std::ofstream& fout){ try{
	int l;

	//header - metadata
	fout << "xFerFunc is " << xFerType <<endl;


	//layer and node info 
	fout << "number of layers = " << layerVec.size() << endl;
	for(l=0;l<layerVec.size();l++){
		fout << "number of nodes in layer " << l << " is " << layerVec[l].getNNodes() << endl;
		//consider also including the list of nodes here with node information.  possibly including in and outVecs.
	}

	fout << '#' << endl;


	fout << linkVec.size() << endl;
	for(l=0;l<linkVec.size();l++)	//linkN fromNode toNode weight
		fout << l <<' '<< linkVec[l].getSource() <<' '<< linkVec[l].getDest() <<' '<< linkVec[l].getW() <<endl;

}catch(int errorCode){
	cout << "Error in backprop print(std::ofstream& fout). Error code " << errorCode << endl;
	
}}



////////////////////////////////////////////////////////////////////////////////////////
////////////////placeLink()////////////////////////////////////////////////////////

void backprop::placeLink(int fromNode, int toNode){

	//initialize weight
	double w = dsfmt_genrand_open_open(&rng);
	w = (w-0.5)/2; //scale w to be positive or negative and keep it near 0.

	placeLink(fromNode,toNode,w);
}
void backprop::placeLink(int fromNode, int toNode, double w){
	//the index of the link to be added.
	int i = linkVec.size();

	//make link
	bpLink tempLink(fromNode, toNode, w);
	linkVec.push_back(tempLink);

	//add references to in and out nodes. 
	if(fromNode>-1)	nodeVec[fromNode].addOut(i);//out from fromNode.
		else layerVec[fromNode*(-1)].addBias(i); //out from a layer as a bias.
	nodeVec[toNode].addIn(i);//in to toNode
}


////////////////////////////////////////////////////////////////////////////////////////
////////////////setSeed()//////////////////////////////////////////////////////////

void backprop::setSeed(int seed){
	dsfmt_init_gen_rand(&rng, seed);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////Network Training/Test////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<double> backprop::run(vector<double> *X){
	return f(X);

}

////////////////////////////////////////////////////////////////////////////////////////
////////////////blah...////////////////////////////////////////////////////////////

void backprop::updateWeights(double learningRate){  //after epochSize training cycles, this is to be called to update weights.
	for(int l=0;l<linkVec.size();l++){		//for all links
		linkVec[l].updateWeight(learningRate);	//update weight.
	}
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////f()////////////////////////////////////////////////////////////////


vector<double> backprop::f(vector<double> *X){try{
	vector<double> outVec;

	int n,l,i,o;	
	double a;  //temporary storage for activation.

	//tend to the first layer (layer 0).
	//set input values.
	l=0;	double b;
	for(n=0;n<layerVec[l].getNNodes();n++){					//for nodes in first layer
		if(layerVec[l].getNNodes() != (*X).size()) throw("number of nodes in first layer does not match number of network inputs.  Thrown from backprop::f(vector<double> X)");

		i=layerVec[l].getIndex(n);							//recover network-level index of node
		a = (*X).at(n);
		b = nodeVec[i].setY(a);								//set activation of input node.
		for(o=0;o<nodeVec[i].getNOut();o++)					//for each _o_utBound link,
			linkVec[nodeVec[i].getOut(o)].setEnergy(b);		//pass energy into next links
	}

	//tend to bias stuff.  this can be moved into the next sections, but it fits too nicely here in it's own loop.
	for(l=1;l<layerVec.size();l++){							//for each layer other than the first.
		if(layerVec[l].getNOut()!=layerVec[l].getNNodes()) 
			throw("backprop.h f(vector<double> x) | invalid number of bias links found in \"tend to bias stuff\" part");
		for(n=0;n<layerVec[l].getNNodes();n++)				//for each node within that layer
			linkVec[layerVec[l].getOut(n)].setEnergy(1);	//set input to each link to 1 and calculate energy.
	}

	//for the hidden layers:
	for(l=1;l<layerVec.size()-1;l++){		

		for(n=0;n<layerVec[l].getNNodes();n++){	//for all nodes in layer
			i=layerVec[l].getIndex(n);//recover network-level index of node
			//calculate the node's actiavtion.
			a=0;	//start with a zero actiavtion.
			for(o=0;o<nodeVec[i].getNIn();o++)	//and for each input link,
				a+=linkVec[nodeVec[i].getIn(o)].getEnergy();//return the energy and add it to the net activation.
			b = nodeVec[i].setActivation(a);
			//then calcualte the outbound energy if applicable.
			for(o=0;o<nodeVec[i].getNOut();o++)  //for each _o_utBound link,
				linkVec[nodeVec[i].getOut(o)].setEnergy(b);//pass energy into next links
		}
	}

	//then deal with the last layer.
	l = layerVec.size()-1;
	for(n=0;n<layerVec[l].getNNodes();n++){	//for all nodes in layer
		i=layerVec[l].getIndex(n);//recover network-level index of node
		//calculate the node's actiavtion.
		a=0;	//start with a zero actiavtion.
		for(o=0;o<nodeVec[i].getNIn();o++)	//and for each input link,
			a+=linkVec[nodeVec[i].getIn(o)].getEnergy(); //return the energy and add it to the net activation.
		b = nodeVec[i].setActivation(a);
		//the activation is the output, so use the same 'a' to 
		outVec.push_back(b);//save and return output.
	}

//cout << "in backprop::f, X.size() is "<<X->size()<<", Y.size() is "<<outVec.size()<<" and the number of nodes in last layer is "<<layerVec[l].getNNodes()<<endl;

	return outVec;
}catch(const char * message){cout << message << endl;exit(0);}}


////////////////////////////////////////////////////////////////////////////////////////
////////////////blah...////////////////////////////////////////////////////////////

vector<double> backprop::propagateError(vector<double>* X, vector<double> idealY){	//**should pass these by reference...

//    cout << "-----" << endl << (*X).size() << endl;
	vector<double> actualY = f(X);
	//run backprop error accumulating function.

	//back-propagate errors
	int l,n,o,i;	double e,t,y,s,a;	//layer, node, outboundLink, index, error, target, actualOut[n], sum, activation.
	l = layerVec.size()-1;	//start with the last nodes.
	for(n=0;n<layerVec[l].getNNodes();n++){	//for all nodes in layer
		i=layerVec[l].getIndex(n);//recover network-level index of node

		//solve for node's activation's error.
		t = idealY[n];
		y = actualY[n];
		e = (t-y)*y*(1-y);	//don't really understand this one either.  probably involves derivative of activation func.

		//send error into links
		for(o=0;o<nodeVec[i].getNIn();o++)  //for each _o_utBound link,
			linkVec[nodeVec[i].getIn(o)].incrementError(e);//pass energy into next links
	}


	//for all hidden layers. (all remining layers that isn't the input layer.)
	for(l=(layerVec.size()-2);l>0;l--){
		for(n=0;n<layerVec[l].getNNodes();n++){	//for all nodes in layer
			i=layerVec[l].getIndex(n);//recover network-level index of node

			//calculate back-propped actiavtion (error) of current node.
			s=0;
			for(o=0;o<nodeVec[i].getNOut();o++)  //for each _o_utBound link,
				s += linkVec[nodeVec[i].getOut(o)].getSPError();//pass _s_ingle _p_ass energy into next links
			a = nodeVec[i].getActivation();
			e = a*(1-a)*s;

			//send error into previous links.
			for(o=0;o<nodeVec[i].getNIn();o++)  //for each _o_utBound link,
				linkVec[nodeVec[i].getIn(o)].incrementError(e);//pass energy into next links
		}
	}

	return actualY;	//this will always be delayed one rotation of trials... weights aren't updated till after.
}

