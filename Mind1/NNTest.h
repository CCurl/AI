#pragma once

#include "NeuralNet.h"

class CNNTest
{
public:
	CNNTest();
	~CNNTest();

	CNeuralNet my_net;
	
	void Train(TCHAR Ch);
};

