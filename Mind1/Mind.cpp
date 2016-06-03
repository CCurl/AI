#include "stdafx.h"
#include "Mind.h"

#define BUFSIZE 1024


// ----------------------------------------------------------------------------------------
// Activation functions
// ----------------------------------------------------------------------------------------
static double pi = 3.14159265358979324;

double NormalProbability(double Val)
{
	static double coEff = 1 / (sqrt(2 * pi));
	return coEff * exp(-(Val*Val) / 2);
}

double ReLU(double Val, double not_used)
{
	return Val > 0 ? Val : 0;
}

double ReLU_Noisy(double Val, double not_used)
{
	Val += NormalProbability(Val);
	return Val > 0 ? Val : 0;
}

double ReLU_Parametric(double Val, double Param)
{
	return Val > 0 ? Val : Param*Val;
}

double ReLU_Leaky(double Val, double not_used)
{
	return ReLU_Parametric(Val, 0.01);
}

double Sigmoid(double Val, double not_used)
{
	return 1 / (1 + exp(-Val));
}

double Bool_AF(double Val, double HowClose)
{
	if (abs(1 - Val) <= HowClose)
	{
		Val = 1;
	}
	else if (abs(0 - Val) <= HowClose)
	{
		Val = 0;
	}
	return Val;
}

double Sigmoid_Bool(double Val, double HowClose)
{
	const double upper = Sigmoid(1, 0);
	const double lower = Sigmoid(0, 0);

	Val = Sigmoid(Val, 0);
	if (abs(upper - Val) <= HowClose)
	{
		Val = upper;
	}
	else if (abs(lower - Val) <= HowClose)
	{
		Val = lower;
	}
	return Val;
}

typedef double(*AF_T)(double, double);
AF_T funcs[7] = { Sigmoid, Sigmoid_Bool, Bool_AF, ReLU, ReLU_Leaky, ReLU_Noisy, ReLU_Parametric };

// ----------------------------------------------------------------------------------------
CNeuron *CNeuron::all_neurons[MAX_NEURONS];
int CNeuron::num_neurons = 0;

// ----------------------------------------------------------------------------------------
CNeuron::CNeuron()
{
	if (num_neurons == 0)
	{
		memset(all_neurons, 0, sizeof(all_neurons));
	}

	queued = false;
	resting_potential = POTENTIAL_RESTING;
	potential = resting_potential;
	nnet = NULL;
	fire_count = 0;
	last_fired = 0;
	loc = num_neurons++;
	layer = 0;
	all_neurons[loc] = this;
}

// ----------------------------------------------------------------------------------------
CNeuron::~CNeuron()
{
	// Destroy my synapses ...
	while (synapses.GetCount())
	{
		CSynapse *s = synapses.GetHead();
		delete s;
		synapses.RemoveHead();
	}
	all_neurons[loc] = NULL;
}

// ----------------------------------------------------------------------------------------
void CNeuron::Activate(int Moment)
{
	if ((potential >= FIRE_THRESHOLD) && (last_fired < Moment))
	{
		Fire();
		last_fired = Moment;
	}
	potential = resting_potential;
}

// ----------------------------------------------------------------------------------------
void CNeuron::Die()
{
	while (synapses.GetCount())
	{
		CSynapse *s = synapses.GetHead();
		delete s;
		synapses.RemoveHead();
	}
	fire_count = 0;
}

// ----------------------------------------------------------------------------------------
CSynapse *CNeuron::FindSynapseTo(CNeuron *To)
{
	POSITION pos = synapses.GetHeadPosition();
	while (pos)
	{
		CSynapse *s = synapses.GetNext(pos);
		if (s->to == To)
		{
			return s;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
void CNeuron::Fire()
{
	fire_count++;
	POSITION pos = synapses.GetHeadPosition();
	while (pos)
	{
		CSynapse *s = synapses.GetNext(pos);
		s->to->ReceiveSignal(s->potential, s->inhibitory);
	}
}

// ----------------------------------------------------------------------------------------
CSynapse *CNeuron::GrowSynapseTo(CNeuron *To)
{
	CSynapse *s = FindSynapseTo(To);
	if (s == NULL)
	{
		s = new CSynapse();
		synapses.AddTail(s);
		s->to = To;
	}
	s->potential += SYNAPSE_POTENTIAL;
	return s;
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuron::NeuronAt(int Loc)
{
	return all_neurons[Loc];
}

// ----------------------------------------------------------------------------------------
void CNeuron::ReceiveSignal(int potential, bool inhibitory)
{
	if (1 == 0)
	{
		this->potential = resting_potential;
	}

	this->potential += potential;
	if (!queued)
	{
		queued = true;
		nnet->ActivateQueue.AddTail(this);
	}
}

// ----------------------------------------------------------------------------------------
void CNeuron::RemoveSynapseTo(CNeuron *To)
{
	POSITION pos = synapses.GetHeadPosition();
	while (pos)
	{
		CSynapse *s = synapses.GetNext(pos);
		if (s->to == To)
		{
			synapses.RemoveAt(pos);
			break;
		}
	}
}

// ----------------------------------------------------------------------------------------
LPCTSTR CNeuron::ToString()
{
	static CString ret;
	ret.Format(_T("%d,%d"), loc, resting_potential);
	return ret;
}

// ----------------------------------------------------------------------------------------
// CSynapse
// ----------------------------------------------------------------------------------------
CSynapse *CSynapse::all_synapses[MAX_SYNAPSES];

// ----------------------------------------------------------------------------------------
CSynapse::CSynapse()
{
	static int num_synapses = 0;
	if (num_synapses == 0)
	{
		memset(all_synapses, 0, sizeof(all_synapses));
	}

	to = NULL;
	potential = 0;
	inhibitory = (rand() % 1000) < 400;
	id = num_synapses++;
	all_synapses[id] = this;
}

// ----------------------------------------------------------------------------------------
CSynapse::~CSynapse()
{
	all_synapses[id] = NULL;
}

// ----------------------------------------------------------------------------------------
CSynapse *CSynapse::SynapseAt(int Loc)
{
	return all_synapses[Loc];
}

// ----------------------------------------------------------------------------------------
LPCTSTR CSynapse::ToString(int FromID)
{
	CString ret;
	ret.Format(_T("%d,%d,%d"), FromID, this->to->loc, this->potential);
	return ret;
}

// ----------------------------------------------------------------------------------------
// CMind
// ----------------------------------------------------------------------------------------

CMind::CMind()
{
	srand(GetTickCount());
	moment = 0;
}

// ----------------------------------------------------------------------------------------
CMind::~CMind()
{
	//while (CNeuron::all_neurons.GetCount() > 0)
	//{
	//	POSITION pos = CNeuron::all_neurons.GetStartPosition();
	//	int loc = 0;
	//	CNeuron *n = NULL;
	//	CNeuron::all_neurons.GetNextAssoc(pos, loc, n);
	//	delete n;
	//}
	//CNeuron::all_neurons.RemoveAll();
}

// ----------------------------------------------------------------------------------------
int CMind::Load(char *Filename)
{
	// int tmp = 0;
	CStrings words;
	int num = 0, max_id = 0;
	char buf[BUFSIZE];
	FILE *fp = NULL;
	CString line;

	fopen_s(&fp, "Fish.txt", "rt");
	//fopen_s(&fp, Filename, "rt");
	if (fp)
	{
		while (fgets(buf, BUFSIZE, fp) == buf)
		{
			line = buf;
			line.TrimRight();
			words.Split(line, ' ');
			CString *w = words.GetFirst();
			while (w)
			{
				//text_system.LearnThis(*w);
				w = words.GetNext();
			}
			// int id = words.GetIntAt(0);
			// id = ++tmp;
			// LPCTSTR name = words.GetAt(1);
			//CNeuron *theThing = text_system.LearnThis(line);
			num++;
		}
		fclose(fp);
	}

	return num;
}

// ----------------------------------------------------------------------------------------
int CMind::Save(char *Filename)
{
	CString line;
	int num = 0;
	FILE *fp = NULL;
	fopen_s(&fp, Filename, "wt");
	if (fp)
	{
		fclose(fp);
	}
	return num;
}

// ----------------------------------------------------------------------------------------
CNeuralNet *CMind::CreateNetwork()
{
	CNeuralNet *nn = new CNeuralNet();
	Nets.AddTail(nn);
	nn->mind = this;
	return nn;
}

// ----------------------------------------------------------------------------------------
bool CMind::NextMoment()
{
	++moment;

	// Activation may cause neurons in the next layer to be activated.
	// If we process those before making it through all the neurons in the current
	// layer, then we can end up with incorrect brain waves.
	// Therefore, only activate the neurons that are currently in the list.

	POSITION pos = Nets.GetHeadPosition();
	while (pos)
	{
		CNeuralNet *n = Nets.GetNext(pos);
		n->NextMoment(moment);
	}

	return false;
}
