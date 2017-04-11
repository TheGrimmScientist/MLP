# MLP
A neural network simulator implementing backprop in C++ I wrote in 2011.  I was way worse at creating interfaces, never heard of tests, and totally didn't document the file formats I expected.  So I'll overview some important bits here, but be warned, you'll probalby have to have at least a familiarity with C++ to be able to use this thing.

## Getting it running

You'll need to have `g++` installed (c++ command line compiler), as the `build` script uses `g++` to compile the main executable `run` calls.  I'm pretty sure there aren't any other dependencies, though let me know if you find any.

I have a basic file used to pass parameters into this framework, which I'll cover later in this readme.  I have it pre-loaded with the 8-bit autoencoder example.  As its currently set up, just run the script `run` with:

`./run`

You may need to `chmod` it to give it and `build` permission to be an executable.

If successful, there will be one line printed to the screen showing the final rms error and classification rates on the train and test sets.  Given that the example is very simple, only has 8 samples, and all 8 samples are in both the train and test set, you should see the same performance on both train and test sets.

In the `results` folder (yeah, I know.  I shouldn't have train.vec and test.vec in there, but I wanted some fast way to make sure you had a results folder in a clean clone of this repo...), you'll find several files generated from running this thing.  The most notable ones are:
 * `Output.txt` showing you the actual activations for each (test set?) example
 * `training_log.txt` showing you the learning curves (in terms of rms error and classifciation rate on the train and test sets) every N training cycles.  Note N, among many other things is determined by the 838Encoder.dat


## Random number generation

I used the Double precision SIMD-oriented Fast Mersenne Twister for random number generation.  That code is copied into a folder within `source`.  Looks like the current version of that code lives [here](https://github.com/MersenneTwister-Lab/dSFMT).


## The Experiment File

The experiment file is a normal text file holding all the paramters I needed to run a single experiment.  A working example is included with this repo.  Please use that as the starting point.  You'll find many parameter names straight out of the NN literature, and they behave in the most academically accurate definition I knew back in 2011.  

Note order of variables absolutely matters here.

Also note that, depending on what the leading word is on any line, I read exaclty what I need out of each line to make that setting work.  So anything 'extra' at the end of a line is technially read in by the parser, but immediately dropped as the useful bits are parsed out of each line.  For example `topology3` within `backprop` requires exactly three numbers: number of input nodes, number of hidden nodes, and number of output nodes.  If you add in a fourth number, letter, word, essay (as long as it doesn't have newlines in it), it will be ignored.  So `topology3 8 3 8`, `topology3 8 3 8 laijsdfi`, and `topology3 8 3 8 3 8 3 8` are all identical in the perspective of this file format.

Given the sample file: 

```
backprop
	seed 100
	topology3 8 3 8
data
	format 8 8
	nTrials 8
	datFile 838Encoder.dat
	normalize unit
train 
	trainSet results/train.vec
	testSet results/test.vec
	nTrainIters 10000
	epochSize  1
	learnRate 0.9
	track results/training_log.txt 5  -or- none
save
	rms results/RMS.txt -or- none
	network results/Trained.net -or- none
	output results/Output.txt
varReduction
	single results/varRed.dat -or- none
```

here's an explanation of some of the parameters:
* backprop
  * `seed 100` is setting your random number generator seed to 100.
  * `topology3` specifies a topology with one layer of hidden nodes, where the actual number of input, hidden, and output nodes are given in the same line in that order as seen above.  You can change this to `topology4` to have two hidden layers, and will of course need to now have 4 integers following that word.
* data
  * `format 8 8` is telling the framework to expect a data file 16 lines long, where the first 8 are to be interpreted as inputs and the second 8 are to be interpreted as outputs to the NN.  I also let you pass a file in here where you can set each row to be input, output, or ignored.  I'll let you dig into the code to figure out how to use that one.
  * `nTrials` is how many rows in the data to use.
  * `file <filename>` is filename for the data you want to pass in.
* train
  * `trainSet` and `testSet` are the files that point to the rows within data.file you want to have in each set.  Note this is somehow 1-indexed.  Rather than passing in a file to specify, you can set both to `none` and have it simply build the network using all data and not have test statistics.
  * The next several paramters are right out of NN literature, though in hindsight I fear I defined learnRate a bit wrong.
* save
  * `network` is the actual trained network saved to file.
  * `output` is the sample-by-sample actual activations.

