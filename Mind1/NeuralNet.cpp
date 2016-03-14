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
	}

	if (this->neurons == NULL)
	{
		this->neurons = new CNeuron *[this->num_neurons];
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
			}
			if (to == NULL)
			{
				this->neurons[ti] = CNeuron::AllocateNeuron();
				to = this->NeuronAt(ti);
			}
			CDendron::GrowDendron(from, to, 1);
		}
	}
}

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
// CNeuralNet
// ----------------------------------------------------------------------------------------
CNeuralNet::CNeuralNet()
{
	num_layers = 0;
	layers = NULL;
}

CNeuralNet::CNeuralNet(int NumLayers)
{
	layers = NULL;
	this->NumLayers(NumLayers);
}

CNeuralNet::~CNeuralNet()
{
	this->NumLayers(0);
}

// ----------------------------------------------------------------------------------------
void CNeuralNet::NumLayers(int Value)
{
	// Free any current layers
	if (layers)
	{
		for (int i = 0; i < num_layers; i++)
		{
			if (layers[i])
			{
				delete layers[i];
				layers[i] = NULL;
			}
		}
		delete[] layers;
		layers = NULL;
	}

	num_layers = Value;
	if (num_layers)
	{
		layers = new CNNetLayer *[num_layers];
		for (int i = 0; i < num_layers; i++)
		{
			layers[i] = NULL;
		}
	}
}

// ----------------------------------------------------------------------------------------
void CNeuralNet::DefineLayer(int LayerNumber, int NumNeurons)
{
	if ((0 <= LayerNumber) && (LayerNumber < num_layers))
	{
		if (layers[LayerNumber])
		{
			delete layers[LayerNumber];
		}
		layers[LayerNumber] = new CNNetLayer(NumNeurons);
	}
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
