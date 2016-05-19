#pragma once

class CMind;
class CNeuron;
class CDendrite;

class CNeuralNet
{
public:
	CNeuralNet();
	~CNeuralNet();

	CDendrite *GrowDendrite(CNeuron *From, CNeuron *To);
	CNeuron *GrowNeuron();
	void NextMoment(int Moment);

	CMind *mind;
	CList<CDendrite *>Dendrites;
	CList<CNeuron *>Neurons;
	CList<CNeuron *>Fired;
};
