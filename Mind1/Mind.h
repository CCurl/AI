#pragma once

#include "StringUtils.h"
#include "NeuralNet.h"

#define FN_THEMIND "TheMind.txt"
#define FN_CONCEPTS "Concepts.txt"
#define FN_ASSOCS "Associations.txt"
#define FORGET_THRESHOLD 5

class CNeuron;
class CDendrite;
class CMind;

typedef enum
{
	SIGMOID = 0,
	SIGMOID_BOOL = 1,
	BOOL_AF = 2,
	RELU = 3,
	RELU_LEAKY = 4,
	RELU_NOISY = 5,
	RELU_PARAMETRIC = 6,
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

	LPCTSTR ToString();
	//last_moment >= Moment
	void Activate(int Moment);
	void AdjustWeights(double DesiredOutput, double LearningRate);
	double Sigmoid(double Val) { return 1 / (1 + exp(-Val)); }
	double Derivative(double Val) { return Val * (1 - Val); }
	void Fire() { nnet->Fired.AddTail(this); }
	void GrowDendriteTo(CNeuron *To);

	int fired; // 0 => not fired, 1 => fired
	int location;
	ActivationFunctiion_T activation_function;

	// This neuron's connections
	CList<CDendrite *> boutons;		// going out
	CList<CDendrite *> dendrites;	// coming in

	CNeuralNet *nnet;

	int last_moment;
	int input, bias, output, error_term, activation_param;

	// Class statics ...
public:
	static CNeuron *NeuronAt(int Location, bool Add = false);
	static CMap<int, int, CNeuron *, CNeuron *&> all_neurons;
	static int last_used;
};

// ----------------------------------------------------------------------------------------
// CDendrite
// ----------------------------------------------------------------------------------------
class CDendrite
{
public:
	CDendrite() { from = to = NULL; weight = weight_adjusted = 1; }
	CDendrite(CNeuron *From, CNeuron *To, double Weight) { from = From;  to = To; weight = weight_adjusted = Weight; }

	int id;
	CNeuron *from, *to;
	double weight, weight_adjusted;

public:
	static CDendrite *GrowDendrite(CNeuron *From, CNeuron *To, double Weight = 0);
	static CDendrite *CDendrite::DendriteAt(int ID);

private:
	//static CMap<int, int, CDendrite *, CDendrite *&> CDendrite::all_dendrites;
	//static int last_used;
};


// ----------------------------------------------------------------------------------------
// CMind
// ----------------------------------------------------------------------------------------
class CMind
{
public:
	CMind();
	~CMind();

	int Load(char *Filename);
	int Save(char *Filename);
	bool NextMoment();
	CNeuralNet *CreateNetwork();

	CList<CNeuralNet *>Nets;
	int moment;
};
