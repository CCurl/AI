#pragma once

#include "StringUtils.h"
#include "NeuralNet.h"

#define FN_THEMIND "TheMind.txt"
#define FN_CONCEPTS "Concepts.txt"
#define FN_ASSOCS "Associations.txt"
#define FORGET_THRESHOLD 5

#define MEMORY_SIZE 1024*1024

typedef enum {
	MindRoot = 0, 
	// Systems first
	TextSystem, ConceptSystem, ExecutiveSystem, VisionSystem,

	TextNeuron, ConceptNeuron, ExecutiveNeuron, VisionNeuron,
	NT_Unknown = 999
} NEURON_T;

typedef enum {
	DT_Unknown, DT_TextIn, DT_TextOut, DT_Concept, DT_Wakeup
} DENDRON_T;

class CMind;
class CNeuron;
class CConceptSystem;
class CExecutiveSystem;
class CTextSystem;
class CVisionSystem;

// ----------------------------------------------------------------------------------------
// CDendron
// ----------------------------------------------------------------------------------------
class CDendron
{
public:
	CDendron() { pToDendron = NULL; threshold = 0; type = DT_Unknown; }
	CDendron(CNeuron *To, int Threshold, DENDRON_T Type) { pToDendron = To; threshold = Threshold; type = Type; }
	CDendron(int From, int To, float Weight) { from = From;  to = To; weight = Weight; strength = 1; }

	int from, to, strength;
	float weight;

	void Activate();
	CNeuron *From(); // { return CNeuron::NeuronAt(from); }
	CNeuron *To(); // { return CNeuron::NeuronAt(to); }
	void Weight(float Value) { weight = Value; }
	float Weight() { return weight; }

	// deprecated
	DENDRON_T type;
	CNeuron *pToDendron;
	int threshold;

	static void GrowDendron(CNeuron *From, CNeuron *To, float Weight);
	static void GrowDendron(int From, int To, float Weight);
};

// ----------------------------------------------------------------------------------------
// CNeuron
// ----------------------------------------------------------------------------------------
class CNeuron
{
public:
	CNeuron();
	CNeuron(NEURON_T Type) : CNeuron() { type = Type; location = 0; value = 0; }
	CNeuron(NEURON_T Type, int Loc) : CNeuron() { type = Type; location = Loc; value = 0; }
	~CNeuron();

	void GrowDendronTo(CNeuron *To, int Trigger, DENDRON_T Type);
	void GrowDendronTo(CNeuron *To, float Weight);

	CDendron *FindDendronTo(int Location);
	CDendron *FindDendron(DENDRON_T Type);
	CDendron *FindDendron(int Threshold, DENDRON_T Type);
	CDendron *FindDendronOtherThan(NEURON_T Type);

	void Activate();
	void Activated();
	void ActivateDendronsOfType(DENDRON_T Type, CMind *Mind);
	void ActivateDendronsToType(NEURON_T Type, CMind *Mind) {}
	void ActivateDendronsNotToType(NEURON_T Type, CMind *Mind);
	void ActivateDendronsEqualTo(DENDRON_T Type, int Threshold, CMind *Mind);
	void ActivateDendronsNotEqualTo(DENDRON_T Type, int Threshold, CMind *Mind) {}
	void ActivateDendronsGreaterThan(DENDRON_T Type, int Threshold, CMind *Mind) {}
	void ActivateDendronsLessThan(DENDRON_T Type, int Threshold, CMind *Mind) {}

	double Value() { return dVal; }
	void Value(double Val) { dVal = Val; }

	bool HasLinkTo(CNeuron *To) { return false; }

	LPCTSTR ToString();

	int location;
	int layer;
	int value;
	bool activated;
	NEURON_T type;

	// This node's connections
	CList<CDendron *> boutons;		// going out
	CList<CDendron *> dendrites;	// coming in

private:
	double dVal;

	// Class statics ...
public:
	static CNeuron *AllocateNeuron(NEURON_T Type = NT_Unknown);
	static CNeuron *NeuronAt(int Location, bool Add = false, NEURON_T Type = NT_Unknown);
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
	void Fire(CDendron *Link);

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
	void Fire(CDendron *Link) {}

	CMind *mind;
	CNeuron *root;
};

// ----------------------------------------------------------------------------------------
// CTextSystem
// ----------------------------------------------------------------------------------------
class CTextSystem
{
public:
	CTextSystem() { root = NULL; mind = NULL; }
	LPCTSTR BuildOutput(CNeuron *PathEnd);
	
	void Fire(CDendron *Link);
	void LearnThis(LPCTSTR Input, CNeuron *Thing = NULL);
	void LookAt(LPCTSTR Input);
	TCHAR NextChar() { return cur_offset >= last_received.GetLength() ? NULL : last_received.GetAt(cur_offset++); }
	void ReceiveInput(LPCTSTR Input) { input_queue.AddTail(Input); }

	CMind *mind;
	CNeuron *root;

	CList<CString> input_queue;

	CString last_received;
	CString last_output;
	int cur_offset;
};

// ----------------------------------------------------------------------------------------
// CMind
// ----------------------------------------------------------------------------------------
class CMind
{
public:
	CMind();
	~CMind();

	CNeuron *AllocateNeuron(NEURON_T Type = NT_Unknown);
	void Fire(CDendron *Link) { if (Link) fire_queue.AddTail(Link); }
	void FireOne(CDendron *Link);
	int Load(char *Filename);
	CNeuron *NeuronAt(int Location, bool Add = false, NEURON_T Type = NT_Unknown);
	int Save();
	bool Think(CString& Output);

	CNeuron *memory_root;
	CConceptSystem concept_system;
	CExecutiveSystem executive_system;
	CTextSystem text_system;

	CList<CDendron *> fire_queue;
	static CList<CNeuron *> activated;
	static void ActivateNeuron(CNeuron *Neuron) { activated.AddTail(Neuron); }
};
