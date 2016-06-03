#pragma once

#include "StringUtils.h"
#include "NeuralNet.h"

#define FN_THEMIND "TheMind.txt"
#define FN_CONCEPTS "Concepts.txt"
#define FN_ASSOCS "Associations.txt"
#define FORGET_THRESHOLD 5

class CNeuron;
class CSynapse;
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
} ActivationFunction_T;

// ----------------------------------------------------------------------------------------
// CNeuron
// ----------------------------------------------------------------------------------------
#define MAX_NEURONS (1024*8)
#define POTENTIAL_RESTING -30
#define FIRE_THRESHOLD 0

class CNeuron
{
public:
	CNeuron();
	~CNeuron();

	LPCTSTR ToString();
	void Activate(int Moment);
	void Die();
	CSynapse *FindSynapseTo(CNeuron *To);
	void Fire();
	CSynapse *GrowSynapseTo(CNeuron *To);
	void ReceiveSignal(int potential, bool inhibitory);
	void RemoveSynapseTo(CNeuron *To);

	int loc, layer;
	int potential, resting_potential;
	int last_fired, fire_count;
	bool queued;
	//ActivationFunction_T activation_function;

	// This neuron's connections
	CList<CSynapse *> synapses;

	CNeuralNet *nnet;

	static CNeuron *all_neurons[MAX_NEURONS];
	static int num_neurons;
	CNeuron *NeuronAt(int Loc);
};

// ----------------------------------------------------------------------------------------
// CSynapse
// ----------------------------------------------------------------------------------------
#define MAX_SYNAPSES (MAX_NEURONS*64)
#define SYNAPSE_POTENTIAL 4

class CSynapse
{
public:
	CSynapse();
	~CSynapse();
	LPCTSTR ToString(int FromID);

	CNeuron *to;
	int id, potential;
	bool inhibitory;

	static CSynapse *all_synapses[MAX_SYNAPSES];
	static CSynapse *SynapseAt(int Loc);
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
