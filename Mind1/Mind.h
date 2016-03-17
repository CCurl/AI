#pragma once

#include "StringUtils.h"
#include "NeuralNet.h"

#define FN_THEMIND "TheMind.txt"
#define FN_CONCEPTS "Concepts.txt"
#define FN_ASSOCS "Associations.txt"
#define FORGET_THRESHOLD 5

#define MEMORY_SIZE 1024*1024

class CMind;
class CNeuron;
class CConceptSystem;
class CExecutiveSystem;
class CTextSystem;
class CVisionSystem;

// ----------------------------------------------------------------------------------------
// CDendrite
// ----------------------------------------------------------------------------------------
class CDendrite
{
public:
	CDendrite() { from = 0;  to = 0; weight = 1; strength = 0; bias = 1; activated = false; }
	CDendrite(int From, int To, double Weight) { from = From;  to = To; weight = Weight; strength = 0; bias = 1; activated = false; }

	int from, to, strength;

	void Activate();
	void Bias(double Value) { bias = Value; }
	double Bias() { return bias; }
	CNeuron *From(); // { return CNeuron::NeuronAt(from); }
	CNeuron *To(); // { return CNeuron::NeuronAt(to); }
	double Weight() { return weight; }
	void Weight(double Value) { weight = Value; }
	void AdjustWeight(double ErrPct);

private:
	bool activated;
	double weight;
	double bias;

public:
	static CDendrite *GrowDendrite(CNeuron *From, CNeuron *To, double Weight = 0);
	static CDendrite *GrowDendrite(int From, int To, double Weight = 0);
};

// ----------------------------------------------------------------------------------------
// CNeuron
// ----------------------------------------------------------------------------------------
class CNeuron
{
public:
	CNeuron();
	CNeuron(int Loc);
	~CNeuron();

	void Activate();
	void AdjustWeights(double ErrPct);
	void GrowDendriteTo(CNeuron *To, double Weight = 0);
	void GrowBouton(CDendrite *D) { boutons.AddTail(D); }
	void GrowDendrite(CDendrite *D) { dendrites.AddTail(D); }
	void Collect(double Val);
	double Threshold() { return threshold; }
	void Threshold(double Val) { threshold = Val; }
	LPCTSTR ToString();
	double Value() { return value; }
	void Value(double Val) { value = Val; }

	int location;
	int layer, offset;
	bool activated;

	// This node's connections

private:
	double threshold;
	double value;
	CList<CDendrite *> boutons;		// going out
	CList<CDendrite *> dendrites;	// coming in

	// Class statics ...
public:
	static CNeuron *AllocateNeuron();
	static CNeuron *NeuronAt(int Location, bool Add = false);
	static CNeuron *all_neurons[MEMORY_SIZE];
	static int last_memory_location;
};


// ----------------------------------------------------------------------------------------
// CConceptSystem
// ----------------------------------------------------------------------------------------
class CConceptSystem
{
public:
	CConceptSystem() { root = NULL; mind = NULL; }
	void Fire(CDendrite *Link);

	CMind *mind;
	CNeuron *root;
};

// ----------------------------------------------------------------------------------------
// CExecutiveSystem
// ----------------------------------------------------------------------------------------
class CExecutiveSystem
{
public:
	CExecutiveSystem() { root = NULL; mind = NULL; }
	void Fire(CDendrite *Link) {}

	CMind *mind;
	CNeuron *root;
};

// ----------------------------------------------------------------------------------------
// CTextSystem
// ----------------------------------------------------------------------------------------
class CTextSystem
{
public:
	//CTextSystem() { root = NULL; mind = NULL; }
	//LPCTSTR BuildOutput(CNeuron *PathEnd);
	//
	//void Fire(CDendrite *Link);
	//void LearnThis(LPCTSTR Input, CNeuron *Thing = NULL);
	//void LookAt(LPCTSTR Input);
	//TCHAR NextChar() { return cur_offset >= last_received.GetLength() ? NULL : last_received.GetAt(cur_offset++); }
	//void ReceiveInput(LPCTSTR Input) { input_queue.AddTail(Input); }

	//CMind *mind;
	//CNeuron *root;

	//CList<CString> input_queue;

	//CString last_received;
	//CString last_output;
	//int cur_offset;
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
	CConceptSystem concept_system;
	CExecutiveSystem executive_system;
	CTextSystem text_system;

	CList<CDendrite *> fire_queue;
	static CList<CNeuron *> activated;
	static void ActivateNeuron(CNeuron *Neuron) { activated.AddTail(Neuron); }
};
