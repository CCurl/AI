#pragma once

class CMind;
class CNeuron;
class CDendrite;

#define NN_MAX_LAYERS 100

class CNNetLayer
{
public:
	CNNetLayer() { num_neurons = 0; neurons = NULL; bias = 1; }
	CNNetLayer(int NumNeurons);
	~CNNetLayer();
	
	void BuildConnections(CNNetLayer *From);
	CNeuron *NeuronAt(int Index) { return neurons[Index]; }

	int layer_num;
	int num_neurons;
	double bias;
	CNeuron **neurons;
};

class CNeuralNet
{
public:
	CNeuralNet();
	CNeuralNet(int NumLayers);
	~CNeuralNet();

	void Fire(CDendrite *Link);

	CMind *mind;
	CNeuron *root;

	void AdjustWeights(double ErrPct);
	void Bias(int Layer, double Value) { biases[Layer] = Value; }
	double Bias(int Layer) { return biases[Layer]; }
	void BuildConnections();
	void DefineLayer(int LayerNumber, int NumNeurons);
	double Go();
	void NumLayers(int Value);
	int NumLayers() { return num_layers; }
	void SetInput(int Index, double Value);
	double Output(int Index) { return 0; }

	static double Sigmoid(double Val);
	static double Derivative(double Val);

	int num_layers;
	CNNetLayer *layers[100];
	double biases[100];
};
