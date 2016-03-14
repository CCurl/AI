#pragma once

class CMind;
class CNeuron;
class CDendron;

#define NN_MAX_LAYERS 100

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

	void Bias(int Layer, double Value) { biases[Layer] = Value; }
	double Bias(int Layer) { return biases[Layer]; }
	void BuildConnections();
	void DefineLayer(int LayerNumber, int NumNeurons);
	void Go();
	void NumLayers(int Value);
	int NumLayers() { return num_layers; }
	void SetInput(int Index, double Value);
	double Output(int Index) { return 0; }

	int num_layers;
	CNNetLayer *layers[100];
	double biases[100];
};
