#include "stdafx.h"
#include "Mind.h"

#define BUFSIZE 1024

CMap<int, int, CNeuron *, CNeuron *&> CNeuron::all_neurons;
int CNeuron::last_used = 0;

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
CNeuron::CNeuron()
{
	input = 0; 
	output = 0;
	fired = 0;
	error_term = 0;
	activation_function = SIGMOID;
	//activation_function = RELU;
	activation_param = 0;
	nnet = NULL;
}

// ----------------------------------------------------------------------------------------
CNeuron::CNeuron(int Loc)
	: CNeuron()
{
	location = Loc;
	bias = ((double)rand() / (double)RAND_MAX) * 2 - 1;
}

// ----------------------------------------------------------------------------------------
CNeuron::~CNeuron()
{
	// Destroy my boutons ...
	while (boutons.GetCount())
	{
		CDendrite *link = boutons.GetHead();
		delete link;
		boutons.RemoveHead();
	}

	// The dendrites will be destroyed by the neurons from whom they emit.
	// They are the boutons of other neurons.
	POSITION pos = dendrites.GetHeadPosition();
	while (pos)
	{
		CDendrite *link = dendrites.GetNext(pos);
		//link->to = NULL;
	}
	dendrites.RemoveAll();
	CNeuron::all_neurons.RemoveKey(location);
}

// ----------------------------------------------------------------------------------------
void CNeuron::Activate(int Moment)
{
	if (last_moment >= Moment)
	{
		return;
	}
	last_moment = Moment;
	double sum = 0;

	// Only sum the inputs for hidden and output neurons
	if (dendrites.GetCount() > 0)
	{
		POSITION pos = dendrites.GetHeadPosition();
		while (pos)
		{
			CDendrite *d = dendrites.GetNext(pos);
			// Carry forward the adjustment from the last epoch
			d->weight = d->weight_adjusted;
			sum += (d->from->output * d->weight);
		}
		output = funcs[activation_function](sum + bias, activation_param);
		input = sum;
		//output = Sigmoid(input + Bias);
	}
	else
	{
		// I must be an input neuron
		output = input;
	}
}

// ----------------------------------------------------------------------------------------
void CNeuron::AdjustWeights(double Desired, double LearningRate)
{
	if (dendrites.GetCount() == 0)
	{
		// I must be an input neuron
		return;
	}

	CDendrite *d = NULL;
	POSITION pos = boutons.GetHeadPosition();
	if (pos == NULL)
	{	
		// I must be an output neuron
		error_term = Derivative(output) * (Desired - output);
	}
	else
	{
		// I must be a hidden neuron
		double sumOfOutputErrors = 0;
		while (pos)
		{
			d = boutons.GetNext(pos);
			sumOfOutputErrors += (d->to->error_term * d->weight);
		}
		error_term = Derivative(output) * sumOfOutputErrors;
	}
	
	// Now we can adjust our input weights
	pos = dendrites.GetHeadPosition();
	while (pos)
	{
		d = dendrites.GetNext(pos);
		double input = d->from->output;
		d->weight_adjusted = d->weight + LearningRate * error_term * input;
	}
}

// ----------------------------------------------------------------------------------------
void CNeuron::GrowDendriteTo(CNeuron *To)
{
	CDendrite::GrowDendrite(this, To);
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuron::NeuronAt(int Location, bool Add)
{
	CNeuron *ret = NULL;
	all_neurons.Lookup(Location, ret);
	if ((ret == NULL) && Add)
	{
		if (Location < 0)
		{
			Location = ++last_used;
		}
		ret = new CNeuron(Location);
		all_neurons[Location] = ret;
	}
	return ret;
}

// ----------------------------------------------------------------------------------------
LPCTSTR CNeuron::ToString()
{
	static CString ret;
	//ret.Format(_T("%d,%d,%d,%d"), location, type, value, boutons.GetCount());

	//POSITION pos = boutons.GetHeadPosition();
	//while (pos)
	//{
	//	CDendrite *link = boutons.GetNext(pos);
	//	ret.AppendFormat(_T(",%d-%d-%d"), link->type, link->threshold, link->pToDendrite->location);
	//}

	return ret;
}

// ----------------------------------------------------------------------------------------
// CDendrite
// ----------------------------------------------------------------------------------------
//int CDendrite::last_used;
//CMap<int, int, CDendrite *, CDendrite *&> CDendrite::all_dendrites;

CDendrite *CDendrite::GrowDendrite(CNeuron *From, CNeuron *To, double Weight)
{
		static int count = 0;
	if (From && To)
	{
		if (Weight == 0)
		{
			Weight = ((double)rand() / (double)RAND_MAX);
			if (count++ % 2 == 1)
				Weight = -Weight;
		}
		CDendrite *new_dendrite = new CDendrite(From, To, Weight);
		From->boutons.AddTail(new_dendrite);
		To->dendrites.AddTail(new_dendrite);
		//new_dendrite->id = ++last_used;
		//all_dendrites.SetAt(new_dendrite->id, new_dendrite);
		return new_dendrite;
	}
	return NULL;
}

CDendrite *CDendrite::DendriteAt(int ID)
{
	CDendrite *d = NULL;
	//all_dendrites.Lookup(ID, d);
	return d;
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
	while (CNeuron::all_neurons.GetCount() > 0)
	{
		POSITION pos = CNeuron::all_neurons.GetStartPosition();
		int loc = 0;
		CNeuron *n = NULL;
		CNeuron::all_neurons.GetNextAssoc(pos, loc, n);
		delete n;
	}
	CNeuron::all_neurons.RemoveAll();
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

