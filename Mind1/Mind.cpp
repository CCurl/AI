#include "stdafx.h"
#include "Mind.h"

#define BUFSIZE 1024

CNeuron *CNeuron::all_neurons[MEMORY_SIZE];
int CNeuron::last_memory_location = 0;

// ----------------------------------------------------------------------------------------
CNeuron::CNeuron()
{
	location = 0; 
	value = 0; 
	//threshold = ((double)rand() / (double)RAND_MAX);
	threshold = 0.5;
	activated = false;
}

// ----------------------------------------------------------------------------------------
CNeuron::CNeuron(int Loc)
	: CNeuron()
{
	location = Loc;
}

// ----------------------------------------------------------------------------------------
CNeuron::~CNeuron()
{
	// Destroy my boutons ...
	while (boutons.GetCount())
	{
		CDendrite *link = boutons.GetHead();
		delete link;
		boutons.RemoveHead();
	}

	// The dendrites will be destroyed by the neuron from whom they emit
	dendrites.RemoveAll();
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuron::AllocateNeuron()
{
	CNeuron *ret = NULL;

	while (last_memory_location < MEMORY_SIZE)
	{
		if (all_neurons[last_memory_location] == NULL)
		{
			ret = new CNeuron(last_memory_location);
			all_neurons[last_memory_location] = ret;
			break;
		}
		last_memory_location++;
	}
	return ret;
}

// ----------------------------------------------------------------------------------------
void CNeuron::Activate()
{
	//if (this->layer > 0)
	//{
	//	value += 0.00001;	 // TEMP TESTING
	//}

	activated = true; // (this->value > this->threshold);
	if (activated)
	{
		POSITION pos = boutons.GetHeadPosition();
		while (pos)
		{
			boutons.GetNext(pos)->Activate();
		}
	}
}

// ----------------------------------------------------------------------------------------
void CNeuron::AdjustWeights(double ErrPct)
{
	//activated = true;
	if (activated)
	{
		threshold -= (ErrPct/2);
		activated = false;
		POSITION pos = dendrites.GetHeadPosition();
		while (pos)
		{
			CDendrite *d = dendrites.GetNext(pos);
			d->AdjustWeight(ErrPct);
			// dendrites.GetNext(pos)->AdjustWeight(ErrPct);
		}
	}

	if (boutons.GetHeadPosition() != NULL)
	{
		value = 0;
	}
}

// ----------------------------------------------------------------------------------------
void CNeuron::Collect(double Val)
{
	value += Val;
	// We can't fire here because a later negative collection event may lower our
	// value, putting us below the threshold.
}

// ----------------------------------------------------------------------------------------
void CNeuron::GrowDendriteTo(CNeuron *To, double Weight)
{
	CDendrite *d = new CDendrite(this->location, To->location, Weight);
	this->GrowBouton(d);
	To->GrowDendrite(d);
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuron::NeuronAt(int Location, bool Add)
{
	if ((Location < 0) || (Location >= MEMORY_SIZE))
	{
		return NULL;
	}

	CNeuron *neuron = all_neurons[Location];
	if ((neuron == NULL) && Add)
	{
		neuron = new CNeuron(Location);
		all_neurons[Location] = neuron;
	}
	return neuron;
}

// ----------------------------------------------------------------------------------------
LPCTSTR CNeuron::ToString()
{
	static CString ret;
	//ret.Format(_T("%d,%d,%d,%d"), location, type, value, boutons.GetCount());

	//POSITION pos = boutons.GetHeadPosition();
	//while (pos)
	//{
	//	CDendrite *link = boutons.GetNext(pos);
	//	ret.AppendFormat(_T(",%d-%d-%d"), link->type, link->threshold, link->pToDendrite->location);
	//}

	return ret;
}

// ----------------------------------------------------------------------------------------
// CDendrite
// ----------------------------------------------------------------------------------------
void CDendrite::Activate()
{
	CNeuron *tN = CNeuron::NeuronAt(to);
	if (tN)
	{
		// Pass my weight and bias to the neuron.
		// This causes it to activate itself should its threshold be exceeded.
		strength++;
		tN->Collect(weight * bias);
		activated = true;
	}
	else
	{
		strength = 0;
	}
}

// ----------------------------------------------------------------------------------------
void CDendrite::AdjustWeight(double ErrPct)
{
	if (activated)
	{
		activated = false;
		weight += ErrPct; // (weight*ErrPct);
	}
}

// ----------------------------------------------------------------------------------------
CDendrite *CDendrite::GrowDendrite(CNeuron *From, CNeuron *To, double Weight)
{
	CDendrite *d = NULL; // From->FindDendriteTo(To->location);
	if (From && To)
	{
		// Make sure there isn't one already
		if (!d)
		{
			if (Weight == 0)
			{
				Weight = ((double)rand() / (double)RAND_MAX);
				//Weight = 1;
			}
			d = new CDendrite(From->location, To->location, Weight);
			double b = (((double)rand() / (double)RAND_MAX));
			d->Bias(b);
			//d->Bias(1);
			From->GrowBouton(d);
			To->GrowDendrite(d);
		}
	}
	return d;
}

// ----------------------------------------------------------------------------------------
CDendrite *CDendrite::GrowDendrite(int From, int To, double Weight)
{
	CNeuron *fN = CNeuron::NeuronAt(From);
	CNeuron *tN = CNeuron::NeuronAt(To);
	return GrowDendrite(fN, tN, Weight);
}

// ----------------------------------------------------------------------------------------
// CMind
// ----------------------------------------------------------------------------------------

CList<CNeuron *> CMind::activated;

CMind::CMind()
{
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		CNeuron::all_neurons[i] = NULL;
	}
}

// ----------------------------------------------------------------------------------------
CMind::~CMind()
{
	srand(GetTickCount());
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		if (CNeuron::all_neurons[i])
			delete CNeuron::all_neurons[i];
	}
}

int CMind::Load(char *Filename)
{
	// int tmp = 0;
	CStrings words;
	int num = 0, max_id = 0;
	char buf[BUFSIZE];
	FILE *fp = NULL;
	CString line;

	fopen_s(&fp, "Fish.txt", "rt");
	//fopen_s(&fp, Filename, "rt");
	if (fp)
	{
		while (fgets(buf, BUFSIZE, fp) == buf)
		{
			line = buf;
			line.TrimRight();
			words.Split(line, ' ');
			CString *w = words.GetFirst();
			while (w)
			{
				//text_system.LearnThis(*w);
				w = words.GetNext();
			}
			// int id = words.GetIntAt(0);
			// id = ++tmp;
			// LPCTSTR name = words.GetAt(1);
			//CNeuron *theThing = text_system.LearnThis(line);
			num++;
		}
		fclose(fp);
	}

	return num;
}

// ----------------------------------------------------------------------------------------
CNeuron *CMind::NeuronAt(int Location, bool Add)
{
	return CNeuron::NeuronAt(Location, Add);
}

// ----------------------------------------------------------------------------------------
int CMind::Save()
{
	CString line;
	int num = 0;
	FILE *fp = NULL;
	fopen_s(&fp, FN_THEMIND, "wt");
	if (fp)
	{
		for (int i = 0; i < CNeuron::last_memory_location; i++)
		{
			CNeuron *n = NeuronAt(i);
			if (n)
			{
				fputws(n->ToString(), fp);
				fputws(_T("\n"), fp);
			}
		}
		fclose(fp);
	}
	return num;
}

// ----------------------------------------------------------------------------------------
int CMind::WorkNeurons()
{
	int num = 0;
	while (activated.GetCount() > 0)
	{
		activated.RemoveHead()->Activate();
		++num;
	}
	return num;
}

// ----------------------------------------------------------------------------------------
bool CMind::Think(CString& Output)
{
	//const int MAX_CYCLES_PER_WAKEUP = 100;
	//int cycle = 0;

	// Activation may cause neurons in the next layer to be activated.
	// If we process those before making it through all the neurons in the current
	// layer, then we can end up with incorrect brain waves.
	// Therefore, only activate the neurons that are currently in the list.
	int num = activated.GetCount();
	for (int i = 0; i < num; i++)
	{
		activated.RemoveHead()->Activate();
	}

	//if (!text_system.last_output.IsEmpty())
	//{
	//	Output = text_system.last_output;
	//	text_system.last_output.Empty();
	//}

	return false;
}

// ----------------------------------------------------------------------------------------
// This is where the rubber meets the road.
// ----------------------------------------------------------------------------------------
void CConceptSystem::Fire(CDendrite *Link)
{
	//switch (Link->type)
	//{
	//case DT_Wakeup:
	//	break;

	//case DT_Concept:
	//{
	//	// vvvvvvvvvv --- Testing --- vvvvvvvvvvvv
	//	CNeuron *node = mind->NeuronAt(Link->pToDendrite->location - 1);
	//	while (node)
	//	{
	//		if (node->type == ConceptNeuron)
	//		{
	//			node->ActivateDendritesOfType(DT_TextOut, mind);
	//			break;
	//		}
	//		node = mind->NeuronAt(node->location - 1);
	//	}

	//	Link->pToDendrite->ActivateDendritesOfType(DT_TextOut, mind);
	//	// ^^^^^^^^^^ --- Testing --- ^^^^^^^^^^^^
	//}
	//break;

	//default:
	//	break;
	//}
}

// ----------------------------------------------------------------------------------------
// CTextSystem
// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// Builds a text string by walking backwards through the pathway.
// ----------------------------------------------------------------------------------------
//LPCTSTR CTextSystem::BuildOutput(CNeuron *PathEnd)
//{
//	CString output;

	//// Handle the case where the node is not a TextNeuron.
	//if ((PathEnd) && (PathEnd->type != TextNeuron))
	//{
	//	CDendrite *link = PathEnd->FindDendrite(DT_TextOut);
	//	PathEnd = link ? link->pToDendrite : NULL;
	//}

	//while ((PathEnd) && (PathEnd->type == TextNeuron))
	//{
	//	output.AppendChar(TCHAR(PathEnd->value));
	//	CDendrite *link = PathEnd->FindDendrite(DT_TextOut);
	//	PathEnd = link ? link->pToDendrite : NULL;
	//}

	//if (!last_output.IsEmpty())
	//{
	//	last_output.AppendChar(' ');
	//}
	//last_output.Append(output.MakeReverse());

//	return output;
//}

// ----------------------------------------------------------------------------------------
// This is where the rubber meets the road.
// ----------------------------------------------------------------------------------------
//void CTextSystem::Fire(CDendrite *Link)
//{
	//switch (Link->type)
	//{
	//case DT_Wakeup:
	//	if (input_queue.GetCount() > 0)
	//	{
	//		CString input = input_queue.RemoveHead();
	//		LookAt(input);
	//	}
	//	break;

	//case DT_TextIn:
	//{
	//	TCHAR ch = NextChar();
	//	if (ch == NULL)
	//	{
	//		Link->pToDendrite->ActivateDendritesNotToType(TextNeuron, mind);
	//	}
	//	else
	//	{
	//		Link->pToDendrite->ActivateDendritesEqualTo(DT_TextIn, ch, mind);
	//	}
	//}
	//	break;

	//case DT_TextOut:
	//	BuildOutput(Link->pToDendrite);
	//	break;

	//default:
	//	break;
	//}
//}

// ----------------------------------------------------------------------------------------
// Learns the text by building a pathway pToDendrite it.
// ----------------------------------------------------------------------------------------
//void CTextSystem::LearnThis(LPCTSTR Input, CNeuron *Thing)
//{
//	if (*Input == NULL)
//		return;
//
//	CNeuron *cur = root;
//	last_received = Input;
//
//	while (*Input)
//	{
//		int ch = *(Input++);
//		CDendrite *link = cur->FindDendrite(ch, DT_TextIn);
//		CNeuron *pToDendrite = link ? link->pToDendrite : NULL;
//		if (!pToDendrite)
//		{
//			pToDendrite = CNeuron::AllocateNeuron(TextNeuron);
//			pToDendrite->value = ch;
//			cur->GrowDendriteTo(pToDendrite, ch, DT_TextIn);
//			pToDendrite->GrowDendriteTo(cur, cur->value, DT_TextOut);
//		}
//		cur = pToDendrite;
//	}
//
//	// At this point, cur is where (Thing) should go ...
//	if (Thing == NULL)
//	{
//		// No "Thing" was passed in, so create a new one ...
//		CDendrite *link = cur->FindDendriteOtherThan(TextNeuron);
//		Thing = link ? link->pToDendrite : NULL;
//		if (Thing == NULL)
//		{
//			Thing = CNeuron::AllocateNeuron(ConceptNeuron);
//		}
//	}
//
//	// See if we already know about it ...
//	if (cur->FindDendriteTo(Thing->location) == NULL)
//	{
//		cur->GrowDendriteTo(Thing, 0, DT_Concept);
//		Thing->GrowDendriteTo(cur, cur->value, DT_TextOut);
//	}
//}

// ----------------------------------------------------------------------------------------
// Handles input provided.
// If recognized, fires the output nodes.
// ----------------------------------------------------------------------------------------
//void CTextSystem::LookAt(LPCTSTR Input)
//{
//	last_received = Input;
//	cur_offset = 0;
//	TCHAR ch = NextChar();
//
//	root->ActivateDendritesEqualTo(DT_TextIn, ch, mind);
//}
