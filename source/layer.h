
#include<vector>

using namespace std;

//lots of potential in how to cut this.  the orignal goal is to make a "layer" just an organizer for a vector of nodes.  I would like to pull in the links too, but that would make NNS too specialized.  Right?

class layer{
private:

	vector<int> nodes;
	vector<int> biasLinks;

public:
	layer(int nodesInLayer, int*nNodesBeforeLayer);

	void addBias(int linkIndex);
	int getNOut(void);	//this should equal the number of nodes..
	int getOut(int index);

	int getIndex(int node);  //"node" refers the the index of the desired row relative to this layer.
	int getNNodes(void);

};

layer::layer(int nodesInLayer, int*nNodesBeforeLayer){//nodes to fit into this layer, pointer maintaining the global nNodes
	int n;
	for(n=0;n<nodesInLayer;n++)
		nodes.push_back((*nNodesBeforeLayer)++);
}
int layer::getIndex(int node){	return nodes[node];	}
int layer::getNNodes(void){	return nodes.size();	}

void layer::addBias(int linkIndex){
	biasLinks.push_back(linkIndex);
}
int layer::getNOut(void){	//this should equal the number of nodes..
	return biasLinks.size();
}
int layer::getOut(int index){
	return biasLinks[index];
}
