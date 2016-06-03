#include "stdafx.h"
#include "NeuralNet.h"
#include "Mind.h"

// ----------------------------------------------------------------------------------------
// CNeuralNet
// ----------------------------------------------------------------------------------------
CNeuralNet::CNeuralNet()
{
}

CNeuralNet::~CNeuralNet()
{
	CNeuron *n = Neurons.GetHead();
	while (n)
	{
		Neurons.RemoveHead();
		delete n;
		n = Neurons.GetHead();
	}

	ActivateQueue.RemoveAll();
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuralNet::GrowNeuron(int Layer)
{
	CNeuron *n = new CNeuron();
	n->layer = Layer;
	n->nnet = this;
	Neurons.AddTail(n);
	return n;
}

// ----------------------------------------------------------------------------------------
void CNeuralNet::GrowNeurons(int Num, int Layer)
{
	for (int i = 0; i < Num; i++)
	{
		GrowNeuron(Layer);
	}
}

// ----------------------------------------------------------------------------------------
void CNeuralNet::GrowSynapses(int Num)
{
	if (CNeuron::num_neurons < 1)
	{
		return;
	}

	while (Num > 0)
	{
		CNeuron *f = NULL;
		while (f == NULL)
		{
			int loc = rand() % CNeuron::num_neurons;
			f = CNeuron::all_neurons[loc];
			if (f->layer >= 999)
				f = NULL;
		}

		int xxx = (rand() % 5) + 1;
		while (xxx > 0)
		{
			CNeuron *t = NULL;
			while (t == NULL)
			{
				int loc = rand() % CNeuron::num_neurons;
				if (loc != f->loc)
				{
					t = CNeuron::all_neurons[loc];
					if (t)
					{
						if ((t->layer <= f->layer) || (t->resting_potential >= FIRE_THRESHOLD))
							t = NULL;
					}
				}
			}
			f->GrowSynapseTo(t);
			--xxx;
			--Num;
		}
	}
	for (int i = 0; i < Num; i++)
	{
	}
}

// ----------------------------------------------------------------------------------------
void CNeuralNet::NextMoment(int Moment)
{
	while (ActivateQueue.GetCount() > 0)
	{
		CNeuron *n = ActivateQueue.GetHead();
		ActivateQueue.RemoveHead();
		n->queued = false;
		n->Activate(Moment);
	}
}
