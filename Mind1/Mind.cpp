#include "stdafx.h"
#include "Mind.h"

#define BUFSIZE 1024

CNeuron *CNeuron::all_neurons[MEMORY_SIZE];
int CNeuron::last_memory_location = 0;

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

double Sigmoid_Bool(double Val, double not_used)
{
	return Val > 0 ? 1 : 0;
}

typedef double (*AF_T)(double, double);
AF_T funcs[6] = { Sigmoid, Sigmoid_Bool, ReLU, ReLU_Leaky, ReLU_Noisy, ReLU_Parametric };

// ----------------------------------------------------------------------------------------
CNeuron::CNeuron()
{
	double xxx = NormalProbability(10);
	location = 0; 
	input = 0; 
	output = 0;
	error_term = 0;
	activation_function = SIGMOID;
}

// ----------------------------------------------------------------------------------------
CNeuron::CNeuron(int Loc)
	: CNeuron()
{
	location = Loc;
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

	// The dendrites will be destroyed by the neuron from whom they emit
	dendrites.RemoveAll();
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuron::AllocateNeuron()
{
	CNeuron *ret = NULL;

	while (last_memory_location < MEMORY_SIZE)
	{
		if (all_neurons[last_memory_location] == NULL)
		{
			ret = new CNeuron(last_memory_location);
			all_neurons[last_memory_location] = ret;
			break;
		}
		last_memory_location++;
	}
	return ret;
}

// ----------------------------------------------------------------------------------------
void CNeuron::Activate(double Bias)
{
	// Only sum the inputs for hidden and output neurons
	if (dendrites.GetCount() > 0)
	{
		input = 0;
		POSITION pos = dendrites.GetHeadPosition();
		while (pos)
		{
			CDendrite *d = dendrites.GetNext(pos);
			// Carry forward the adjustment from the last epoch
			d->weight = d->weight_adjusted;
			input += (d->from->output * d->weight);
		}
		output = funcs[activation_function](input + Bias, 0);
		//output = Sigmoid(input + Bias);
	}
	else
	{
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
CDendrite *CNeuron::GrowDendriteTo(CNeuron *To, double Weight)
{
	CDendrite *d = new CDendrite(this, To, Weight);
	this->GrowBouton(d);
	To->GrowDendrite(d);
	return d;
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuron::NeuronAt(int Location, bool Add)
{
	if ((Location < 0) || (Location >= MEMORY_SIZE))
	{
		return NULL;
	}

	CNeuron *neuron = all_neurons[Location];
	if ((neuron == NULL) && Add)
	{
		neuron = new CNeuron(Location);
		all_neurons[Location] = neuron;
	}
	return neuron;
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
CDendrite *CDendrite::GrowDendrite(CNeuron *From, CNeuron *To, double Weight)
{
	static int count = 0;
	CDendrite *d = NULL;
	if (From && To)
	{
		if (Weight == 0)
		{
			Weight = ((double)rand() / (double)RAND_MAX);
			if ( count++ % 2 == 1 )
				Weight = -Weight;
		}
		d = From->GrowDendriteTo(To, Weight);
	}
	return d;
}

// ----------------------------------------------------------------------------------------
// CMind
// ----------------------------------------------------------------------------------------

CList<CNeuron *> CMind::activated;

CMind::CMind()
{
	srand(GetTickCount());
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		CNeuron::all_neurons[i] = NULL;
	}
	epoch = 0;
}

// ----------------------------------------------------------------------------------------
CMind::~CMind()
{
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		if (CNeuron::all_neurons[i])
			delete CNeuron::all_neurons[i];
	}
}

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
CNeuron *CMind::NeuronAt(int Location, bool Add)
{
	return CNeuron::NeuronAt(Location, Add);
}

// ----------------------------------------------------------------------------------------
int CMind::Save()
{
	CString line;
	int num = 0;
	FILE *fp = NULL;
	fopen_s(&fp, FN_THEMIND, "wt");
	if (fp)
	{
		for (int i = 0; i < CNeuron::last_memory_location; i++)
		{
			CNeuron *n = NeuronAt(i);
			if (n)
			{
				fputws(n->ToString(), fp);
				fputws(_T("\n"), fp);
			}
		}
		fclose(fp);
	}
	return num;
}

// ----------------------------------------------------------------------------------------
int CMind::WorkNeurons()
{
	int num = 0;
	while (activated.GetCount() > 0)
	{
		activated.RemoveHead()->Activate(1);
		++num;
	}
	return num;
}

// ----------------------------------------------------------------------------------------
bool CMind::Think(CString& Output)
{
	//const int MAX_CYCLES_PER_WAKEUP = 100;
	//int cycle = 0;

	// Activation may cause neurons in the next layer to be activated.
	// If we process those before making it through all the neurons in the current
	// layer, then we can end up with incorrect brain waves.
	// Therefore, only activate the neurons that are currently in the list.
	int num = activated.GetCount();
	for (int i = 0; i < num; i++)
	{
		activated.RemoveHead()->Activate(1);
	}
	epoch++;

	return false;
}

