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
	CDendrite *d = Dendrites.GetHead();
	while (d)
	{
		Dendrites.RemoveHead();
		delete d;
		d = Dendrites.GetHead();
	}

	CNeuron *n = Neurons.GetHead();
	while (n)
	{
		Neurons.RemoveHead();
		delete n;
		n = Neurons.GetHead();
	}

	Fired.RemoveAll();
}

// ----------------------------------------------------------------------------------------
CDendrite *CNeuralNet::GrowDendrite(CNeuron *From, CNeuron *To)
{
	CDendrite *d = new CDendrite();
	d->from = From;
	d->to = To;
	From->boutons.AddTail(d);
	To->dendrites.AddTail(d);
	Dendrites.AddTail(d);
	return d;
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuralNet::GrowNeuron()
{
	CNeuron *n = new CNeuron();
	n->nnet = this;
	Neurons.AddTail(n);
	return n;
}

// ----------------------------------------------------------------------------------------
// Returns the value of the last neuron in the output layer
// This works out nicely when there is only one output neuron
// ----------------------------------------------------------------------------------------
void CNeuralNet::NextMoment(int Moment)
{
	while (Fired.GetCount() > 0)
	{
		CNeuron *n = Fired.GetHead();
		Fired.RemoveHead();
		n->Activate(Moment);
	}
}
