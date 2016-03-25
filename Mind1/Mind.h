#pragma once

#include "StringUtils.h"
#include "NeuralNet.h"

#define FN_THEMIND "TheMind.txt"
#define FN_CONCEPTS "Concepts.txt"
#define FN_ASSOCS "Associations.txt"
#define FORGET_THRESHOLD 5

#define MEMORY_SIZE 1024*1024

class CNeuron;
class CDendrite;
class CMind;

typedef enum
{
	SIGMOID = 0,
	SIGMOID_BOOL = 1,
	RELU = 2,
	RELU_LEAKY = 3,
	RELU_NOISY = 4,
	RELU_PARAMETRIC = 5,
} ActivationFunctiion_T;

// ----------------------------------------------------------------------------------------
// CNeuron
// ----------------------------------------------------------------------------------------
class CNeuron
{
public:
	CNeuron();
	CNeuron(int Loc);
	~CNeuron();

	CDendrite *GrowDendriteTo(CNeuron *To, double Weight = 0);
	void GrowBouton(CDendrite *D) { boutons.AddTail(D); }
	void GrowDendrite(CDendrite *D) { dendrites.AddTail(D); }
	LPCTSTR ToString();

	void Activate(double Bias);
	void AdjustWeights(double DesiredOutput, double LearningRate);
	double Sigmoid(double Val) { return 1 / (1 + exp(-Val)); }
	double Derivative(double Val) { return Val * (1 - Val); }

	int location;
	int layer, offset;
	ActivationFunctiion_T activation_function;

	// This neuron's connections
	CList<CDendrite *> boutons;		// going out
	CList<CDendrite *> dendrites;	// coming in

	double input, output, error_term;

	// Class statics ...
public:
	static CNeuron *AllocateNeuron();
	static CNeuron *NeuronAt(int Location, bool Add = false);
	static CNeuron *all_neurons[MEMORY_SIZE];
	static int last_memory_location;
};

// ----------------------------------------------------------------------------------------
// CDendrite
// ----------------------------------------------------------------------------------------
class CDendrite
{
public:
	CDendrite() { from = to = NULL; weight = weight_adjusted = 1; }
	CDendrite(CNeuron *From, CNeuron *To, double Weight) { from = From;  to = To; weight = weight_adjusted = Weight; }

	CNeuron *from, *to;
	double weight, weight_adjusted;

public:
	static CDendrite *GrowDendrite(CNeuron *From, CNeuron *To, double Weight = 0);
};


// ----------------------------------------------------------------------------------------
// CMind
// ----------------------------------------------------------------------------------------
class CMind
{
public:
	CMind();
	~CMind();

	CNeuron *AllocateNeuron();
	void Fire(CDendrite *Link) { if (Link) fire_queue.AddTail(Link); }
	void FireOne(CDendrite *Link);
	int Load(char *Filename);
	CNeuron *NeuronAt(int Location, bool Add = false);
	int Save();
	bool Think(CString& Output);
	int WorkNeurons();

	CNeuron *memory_root;
	int epoch;

	CList<CDendrite *> fire_queue;
	static CList<CNeuron *> activated;
	static void ActivateNeuron(CNeuron *Neuron) { activated.AddTail(Neuron); }
};





