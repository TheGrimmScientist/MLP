//  front end for NNS to be used for framework testing.

#include<iostream>
#include<fstream>
#include<string.h>

#include"experiment.h"

int main(int argc, char** argv){try{


	if(argc>1){	//if at least one paramter was passed (paramter 0 is the .exe name), then first param is filename.exp
		string expFilename;
		expFilename = argv[1];
		experiment testExp(expFilename);
	}
	else
		throw("No .exp filename was passed to NNS.exe.  Filename needed becuase UI is not yet built.");
	


return 1;
}catch(const char*m){cout<<m<<endl;}}

