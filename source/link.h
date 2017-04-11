// are all links of the sort directional?

//in
//weight
//xFer()
//out

//will I only use normalized data?
//will the output be (0,1) or (-1,1)?

class bpLink {
private:
	int source;
	int dest;
	double w;	//weight
	double input;
	double error;
	double sPError;

public:
	bpLink(int from, int to, double weight);

	int getSource(void);
	int getDest(void);

	double getW(void       );
	void   setW(double newW);

	void setEnergy(double sourceActivation);
	double getEnergy(void);

	double incrementError(double increment);	//error+=(ideal-actual) ?	//** write this funciton!
	double getSPError(void);
	double clearError(void);

	void updateWeight(double learningRate);		//w = w + lR*error		//**return error?

};

bpLink::bpLink(int from, int to, double weight){
	source = from;
	dest = to;
	w = weight;
	error = 0;
}

int 	bpLink::getSource	(void)			{	return source;	}
int 	bpLink::getDest		(void)			{	return dest;	}
double	bpLink::getW		(void)			{	return w;		}	
void	bpLink::setW		(double newW)	{	w = newW;		}

void bpLink::setEnergy(double sourceActivation){	input = sourceActivation;  }
double bpLink::getEnergy(void){	return w*input;	}

void bpLink::updateWeight(double learningRate){
	w = w + error*learningRate*input;
	sPError = 0;
	error = 0;
}

double bpLink::incrementError(double increment)	{	sPError = increment;	error += increment;	return error;	}
double bpLink::getSPError(void)					{	return sPError*w;				}
double bpLink::clearError(void)					{	error = 0;						}//not needed anymore.
