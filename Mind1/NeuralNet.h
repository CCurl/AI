#pragma once

class CMind;
class CNeuron;
class CSynapse;

class CNeuralNet
{
public:
	CNeuralNet();
	~CNeuralNet();

	CNeuron *GrowNeuron(int Layer);
	void GrowNeurons(int Num, int Layer);
	void GrowSynapses(int Num);
	void NextMoment(int Moment);

	CMind *mind;
	CList<CNeuron *>Neurons;
	CList<CNeuron *>ActivateQueue;
};
