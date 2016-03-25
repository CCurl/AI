#include "stdafx.h"
#include "NeuralNet.h"
#include "Mind.h"

// ----------------------------------------------------------------------------------------
// CNeuralNet
// ----------------------------------------------------------------------------------------
CNNetLayer::CNNetLayer(int NumNeurons)
{
	num_neurons = NumNeurons;
	neurons = NULL;
	bias = 0;
}


CNNetLayer::~CNNetLayer()
{
	if (neurons)
	{
		delete[] neurons;
	}
}

// ----------------------------------------------------------------------------------------
void CNNetLayer::BuildConnections(CNNetLayer *From)
{
	if (From->neurons == NULL)
	{
		From->neurons = new CNeuron *[From->num_neurons];
		for (int i = 0; i < From->num_neurons; i++)
		{
			From->neurons[i] = NULL;
		}
	}

	if (this->neurons == NULL)
	{
		this->neurons = new CNeuron *[this->num_neurons];
		for (int i = 0; i < this->num_neurons; i++)
		{
			this->neurons[i] = NULL;
		}
	}

	for (int fi = 0; fi < From->num_neurons; fi++)
	{
		for (int ti = 0; ti < this->num_neurons; ti++)
		{
			CNeuron *from = From->NeuronAt(fi);
			CNeuron *to = this->NeuronAt(ti);
			if (from == NULL)
			{
				From->neurons[fi] = CNeuron::AllocateNeuron();
				from = From->NeuronAt(fi);
				from->layer = From->layer_num;
				from->offset = fi;
			}
			if (to == NULL)
			{
				this->neurons[ti] = CNeuron::AllocateNeuron();
				to = this->NeuronAt(ti);
				to->layer = this->layer_num;
				to->offset = ti;
			}
			CDendrite *d = CDendrite::GrowDendrite(from, to);
		}
	}
}

// ----------------------------------------------------------------------------------------
// CNeuralNet
// ----------------------------------------------------------------------------------------
CNeuralNet::CNeuralNet()
{
	num_layers = 0;
	for (int i = 0; i < NN_MAX_LAYERS; i++)
	{
			layers[i] = NULL;
	}
}

CNeuralNet::CNeuralNet(int NumLayers) 
	: CNeuralNet()
{
	this->NumLayers(NumLayers);
}

CNeuralNet::~CNeuralNet()
{
	this->NumLayers(0);
}

// ----------------------------------------------------------------------------------------
void CNeuralNet::BuildConnections()
{
	for (int i = 1; i < num_layers; i++)
	{
		CNNetLayer *from = layers[i - 1];
		CNNetLayer *to = layers[i];
		if (from && to)
		{
			to->BuildConnections(from);
		}
	}
}

// ----------------------------------------------------------------------------------------
void CNeuralNet::DefineLayer(int LayerNumber, int NumNeurons)
{
	if ((LayerNumber < 0) || (LayerNumber >= NN_MAX_LAYERS))
	{
		return;
	}

	if (num_layers <= LayerNumber)
	{
		num_layers = LayerNumber + 1;
	}

	if (layers[LayerNumber])
	{
		delete layers[LayerNumber];
	}
	layers[LayerNumber] = new CNNetLayer(NumNeurons);
	layers[LayerNumber]->layer_num = LayerNumber;
}

// ----------------------------------------------------------------------------------------
// Returns the value of the last neuron in the output layer
// This works out nicely when there is only one output neuron
// ----------------------------------------------------------------------------------------
double CNeuralNet::Go()
{
	CNeuron *n = NULL;
	CNNetLayer *l = NULL;
	for (int layer_num = 0; layer_num < num_layers; layer_num++)
	{
		l = layers[layer_num];
		for (int i = 0; i < l->num_neurons; i++)
		{
			n = l->NeuronAt(i);
			n->Activate(l->bias);
		}
	}

	// n->Output(n->Input());
	return n->output;
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuralNet::NeuronAt(int Layer, int Offset)
{
	return ((0 <= Layer) && (Layer < num_layers)) ? layers[Layer]->NeuronAt(Offset) : NULL;
}

// ----------------------------------------------------------------------------------------
void CNeuralNet::NumLayers(int Value)
{
	// Free any current layers
	for (int i = 0; i < NN_MAX_LAYERS; i++)
	{
		if (layers[i])
		{
			delete layers[i];
			layers[i] = NULL;
		}
	}

	num_layers = Value;
}

// ----------------------------------------------------------------------------------------
void CNeuralNet::SetInput(int Index, double Value)
{
	layers[0]->NeuronAt(Index)->input = Value;
}

// ----------------------------------------------------------------------------------------
void CNeuralNet::AdjustWeights(double DesiredOutput, double LearningRate)
{
	// This works backwards through the layers
	CNNetLayer *l = NULL;
	CNeuron *n;
	for (int layer_num = num_layers - 1; layer_num > 0; layer_num--)
	{
		l = layers[layer_num];
		for (int i = 0; i < l->num_neurons; i++)
		{
			n = l->NeuronAt(i);
			n->AdjustWeights(DesiredOutput, LearningRate);
		}
	}
}
