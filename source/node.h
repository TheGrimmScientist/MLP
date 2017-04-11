
#include<math.h>
#include<vector>

using namespace std;

#ifndef node_h
#define node_h

class node{  
private:
	int xT;		//transfer funciton type.  1=sigmoid.  2=tan-1 ... gaussian at some point.
	void initNode(int xType);
	double Y;

	vector <int> inVec;
	vector <int> outVec;

	double f(double input);

public:
	node(int xFerType);


	double setActivation(double sum);
	double getActivation(void);
	double setY(double output);

	void addIn(int linkIndex);
	void addOut(int linkIndex);
	int getNIn(void);
	int getNOut(void);
	int getIn(int index);
	int getOut(int index);
};

//node::node(void)		{	initNode(1);		}		//use default type. 
node::node(int xFerType){	initNode(xFerType);	}
void node::initNode(int xFerType){
	xT = xFerType;
	// anything else?  will I use location... ever?  possible.
}

double node::f(double input){try{

	//should the bp class be used to tally all inputs or should the node know what links are present and figure it out?
	double x = input;
	double y;  //output.
	
	if(xT==1){	//sigmoid
			y=(tanh(x)+1)/2;
//cout << input << ' ' << y << endl;
	}
	else if(xT==2){	//tanh.
		y=tanh(x);
	}
	else
		throw("invalid transfer function type in node object");

//cout << "xT = " << xT << " input = " << input <<  " y = " << y << endl;

	return y;
}catch(const char*m){cout<<m<<endl;}}

void node::addIn	(int linkIndex){ 	inVec.push_back(linkIndex); }
void node::addOut	(int linkIndex){	outVec.push_back(linkIndex);}
int  node::getNIn	(void){	return inVec.size();	}
int  node::getNOut	(void){	return outVec.size();	}
int  node::getIn	(int index){ 
//cout << index << " | " << inVec.size() << ' ' << inVec[index] <<endl;	
	return inVec[index];	}
int  node::getOut	(int index){	return outVec[index];	}


double node::setActivation(double sum){try{
	Y=f(sum);
	return Y;
}catch(const char*message){ cout << message << endl;}}
double node::getActivation(void){
	return Y;
}
double node::setY(double output){ Y = output;	return Y;	}

#endif

