#include "stdafx.h"
#include "Mind.h"

#define BUFSIZE 1024

CNeuron *CNeuron::all_neurons[MEMORY_SIZE];
int CNeuron::last_memory_location = 0;

// ----------------------------------------------------------------------------------------
// CNeuron
// ----------------------------------------------------------------------------------------
//int CNeuron::AddInput(CNeuron *Input)
//{ 
//	inputs.Add(Input);
//	return inputs.GetSize(); 
//}

// ----------------------------------------------------------------------------------------
//int CNeuron::AddOutput(CNeuron *Output)
//{
//	outputs.Add(Output);
//	return outputs.GetSize();
//}

// ----------------------------------------------------------------------------------------
CNeuron::~CNeuron()
{
	while (links.GetCount())
	{
		CDendron *link = links.GetHead();
		delete link;
		links.RemoveHead();
	}
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuron::AllocateNeuron(NEURON_T Type)
{
	CNeuron *ret = NULL;

	while (last_memory_location < MEMORY_SIZE)
	{
		if (all_neurons[last_memory_location] == NULL)
		{
			ret = new CNeuron(Type, last_memory_location);
			all_neurons[last_memory_location] = ret;
			break;
		}
		last_memory_location++;
	}
	return ret;
}

// ----------------------------------------------------------------------------------------
CDendron *CNeuron::FindDendron(int Threshold, DENDRON_T Type)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CDendron *link = links.GetNext(pos);
		if ((link->threshold == Threshold) && (link->type == Type))
		{
			return link;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
CDendron *CNeuron::FindDendron(DENDRON_T Type)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CDendron *link = links.GetNext(pos);
		if (link->type == Type)
		{
			return link;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
CDendron *CNeuron::FindDendronOtherThan(NEURON_T Type)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CDendron *link = links.GetNext(pos);
		if (link->to->type != Type)
		{
			return link;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
CDendron *CNeuron::FindDendronTo(int Location)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CDendron *link = links.GetNext(pos);
		if (link->to->location == Location)
		{
			return link;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
void CNeuron::ActivateDendronsEqualTo(DENDRON_T Type, int Threshold, CMind *Mind)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CDendron *link = links.GetNext(pos);
		if ((link->type == Type) && (link->threshold == Threshold))
		{
			Mind->Fire(link);
		}
	}
}

// ----------------------------------------------------------------------------------------
void CNeuron::ActivateDendronsOfType(DENDRON_T Type, CMind *Mind)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CDendron *link = links.GetNext(pos);
		if (link->type == Type)
		{
			Mind->Fire(link);
		}
	}
}

// ----------------------------------------------------------------------------------------
void CNeuron::ActivateDendronsNotToType(NEURON_T Type, CMind *Mind)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CDendron *link = links.GetNext(pos);
		if (link->to->type != Type)
		{
			Mind->Fire(link);
		}
	}
}

// ----------------------------------------------------------------------------------------
void CNeuron::LinkTo(CNeuron *To, int Threshold, DENDRON_T Type)
{
	links.AddTail(new CDendron(To, Threshold, Type));
}

// ----------------------------------------------------------------------------------------
CNeuron *CNeuron::NeuronAt(int Location, bool Add, NEURON_T Type)
{
	if ((Location < 0) || (Location >= MEMORY_SIZE))
		return NULL;

	CNeuron *node = all_neurons[Location];
	if ((node == NULL) && Add)
	{
		node = new CNeuron(Type, Location);
		all_neurons[Location] = node;
	}
	return node;
}

// ----------------------------------------------------------------------------------------
LPCTSTR CNeuron::ToString()
{
	static CString ret;
	ret.Format(_T("%d,%d,%d,%d"), location, type, value, links.GetCount());

	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CDendron *link = links.GetNext(pos);
		ret.AppendFormat(_T(",%d-%d-%d"), link->type, link->threshold, link->to->location);
	}

	if (type == TextNeuron)
	{
		ret.AppendFormat(_T(",%c"), char(value));
	}
	return ret;
}

// ----------------------------------------------------------------------------------------
// CMind
// ----------------------------------------------------------------------------------------
CMind::CMind()
{
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		CNeuron::all_neurons[i] = NULL;
	}

	memory_root = NeuronAt(MindRoot, true, MindRoot);

	text_system.root = NeuronAt(TextSystem, true, TextSystem);
	text_system.mind = this;

	concept_system.root = NeuronAt(ConceptSystem, true, ConceptSystem);
	concept_system.mind = this;

	executive_system.root = NeuronAt(ExecutiveSystem, true, ExecutiveSystem);
	executive_system.mind = this;

	memory_root->LinkTo(NeuronAt(ConceptSystem), 0, DT_Wakeup);
	memory_root->LinkTo(NeuronAt(ExecutiveSystem), 0, DT_Wakeup);
	memory_root->LinkTo(NeuronAt(TextSystem), 0, DT_Wakeup);
}

// ----------------------------------------------------------------------------------------
CMind::~CMind()
{
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
				text_system.LearnThis(*w);
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
CNeuron *CMind::NeuronAt(int Location, bool Add, NEURON_T Type)
{
	return CNeuron::NeuronAt(Location, Add, Type);
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
void CMind::FireOne(CDendron *Link)
{
	switch (Link->type)
	{
	case DT_Wakeup:
		text_system.Fire(Link);
		concept_system.Fire(Link);
		executive_system.Fire(Link);
		break;

	case DT_TextIn:
	case DT_TextOut:
		text_system.Fire(Link);
		break;

	case DT_Concept:
		concept_system.Fire(Link);
		break;

	default:
		break;
	}
}

// ----------------------------------------------------------------------------------------
bool CMind::Think(CString& Output)
{
	const int MAX_CYCLES_PER_WAKEUP = 100;
	int cycle = 0;

	// Send out a "wakeup" event
	CDendron wakeup_link(NULL, 0, DT_Wakeup);
	FireOne(&wakeup_link);

	while (fire_queue.GetCount() > 0)
	{
		FireOne(fire_queue.RemoveHead());

		if (++cycle > MAX_CYCLES_PER_WAKEUP)
		{
			break;
		}
	}

	if (!text_system.last_output.IsEmpty())
	{
		Output = text_system.last_output;
		text_system.last_output.Empty();
	}

	return false;
}

// ----------------------------------------------------------------------------------------
// This is where the rubber meets the road.
// ----------------------------------------------------------------------------------------
void CConceptSystem::Fire(CDendron *Link)
{
	switch (Link->type)
	{
	case DT_Wakeup:
		break;

	case DT_Concept:
	{
		// vvvvvvvvvv --- Testing --- vvvvvvvvvvvv
		CNeuron *node = mind->NeuronAt(Link->to->location - 1);
		while (node)
		{
			if (node->type == ConceptNeuron)
			{
				node->ActivateDendronsOfType(DT_TextOut, mind);
				break;
			}
			node = mind->NeuronAt(node->location - 1);
		}

		Link->to->ActivateDendronsOfType(DT_TextOut, mind);
		// ^^^^^^^^^^ --- Testing --- ^^^^^^^^^^^^
	}
	break;

	default:
		break;
	}
}

// ----------------------------------------------------------------------------------------
// CTextSystem
// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// Builds a text string by walking backwards through the pathway.
// ----------------------------------------------------------------------------------------
LPCTSTR CTextSystem::BuildOutput(CNeuron *PathEnd)
{
	CString output;

	// Handle the case where the node is not a TextNeuron.
	if ((PathEnd) && (PathEnd->type != TextNeuron))
	{
		CDendron *link = PathEnd->FindDendron(DT_TextOut);
		PathEnd = link ? link->to : NULL;
	}

	while ((PathEnd) && (PathEnd->type == TextNeuron))
	{
		output.AppendChar(TCHAR(PathEnd->value));
		CDendron *link = PathEnd->FindDendron(DT_TextOut);
		PathEnd = link ? link->to : NULL;
	}

	if (!last_output.IsEmpty())
	{
		last_output.AppendChar(' ');
	}
	last_output.Append(output.MakeReverse());

	return last_output;
}

// ----------------------------------------------------------------------------------------
// This is where the rubber meets the road.
// ----------------------------------------------------------------------------------------
void CTextSystem::Fire(CDendron *Link)
{
	switch (Link->type)
	{
	case DT_Wakeup:
		if (input_queue.GetCount() > 0)
		{
			CString input = input_queue.RemoveHead();
			LookAt(input);
		}
		break;

	case DT_TextIn:
	{
		TCHAR ch = NextChar();
		if (ch == NULL)
		{
			Link->to->ActivateDendronsNotToType(TextNeuron, mind);
		}
		else
		{
			Link->to->ActivateDendronsEqualTo(DT_TextIn, ch, mind);
		}
	}
		break;

	case DT_TextOut:
		BuildOutput(Link->to);
		break;

	default:
		break;
	}
}

// ----------------------------------------------------------------------------------------
// Learns the text by building a pathway to it.
// ----------------------------------------------------------------------------------------
void CTextSystem::LearnThis(LPCTSTR Input, CNeuron *Thing)
{
	if (*Input == NULL)
		return;

	CNeuron *cur = root;
	last_received = Input;

	while (*Input)
	{
		int ch = *(Input++);
		CDendron *link = cur->FindDendron(ch, DT_TextIn);
		CNeuron *to = link ? link->to : NULL;
		if (!to)
		{
			to = CNeuron::AllocateNeuron(TextNeuron);
			to->value = ch;
			cur->LinkTo(to, ch, DT_TextIn);
			to->LinkTo(cur, cur->value, DT_TextOut);
		}
		cur = to;
	}

	// At this point, cur is where (Thing) should go ...
	if (Thing == NULL)
	{
		// No "Thing" was passed in, so create a new one ...
		CDendron *link = cur->FindDendronOtherThan(TextNeuron);
		Thing = link ? link->to : NULL;
		if (Thing == NULL)
		{
			Thing = CNeuron::AllocateNeuron(ConceptNeuron);
		}
	}

	// See if we already know about it ...
	if (cur->FindDendronTo(Thing->location) == NULL)
	{
		cur->LinkTo(Thing, 0, DT_Concept);
		Thing->LinkTo(cur, cur->value, DT_TextOut);
	}
}

// ----------------------------------------------------------------------------------------
// Handles input provided.
// If recognized, fires the output nodes.
// ----------------------------------------------------------------------------------------
void CTextSystem::LookAt(LPCTSTR Input)
{
	last_received = Input;
	cur_offset = 0;
	TCHAR ch = NextChar();

	root->ActivateDendronsEqualTo(DT_TextIn, ch, mind);
}
