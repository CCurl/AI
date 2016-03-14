#pragma once

class CMind;
class CNeuron;
class CDendron;

class CNNetLayer
{
public:
	CNNetLayer() { num_neurons = 0; neurons = NULL; }
	CNNetLayer(int NumNeurons);
	~CNNetLayer();
	
	void BuildConnections(CNNetLayer *From);
	CNeuron *NeuronAt(int Index) { return neurons[Index]; }

	int num_neurons;
	CNeuron **neurons;
};

class CNeuralNet
{
public:
	CNeuralNet();
	CNeuralNet(int NumLayers);
	~CNeuralNet();

	void Fire(CDendron *Link);

	CMind *mind;
	CNeuron *root;

	void BuildConnections();
	void DefineLayer(int LayerNumber, int NumNeurons);
	void NumLayers(int Value);

	int num_layers;
	CNNetLayer **layers;
};
