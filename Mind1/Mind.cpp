#include "stdafx.h"
#include "Mind.h"

#define BUFSIZE 1024

CMemoryNode *CMemoryNode::all_nodes[MEMORY_SIZE];
int CMemoryNode::last_memory_location = 0;

// ----------------------------------------------------------------------------------------
// CMemoryNode
// ----------------------------------------------------------------------------------------
int CMemoryNode::AddInput(CMemoryNode *Input)
{ 
	inputs.Add(Input);
	return inputs.GetSize(); 
}

// ----------------------------------------------------------------------------------------
int CMemoryNode::AddOutput(CMemoryNode *Output)
{
	outputs.Add(Output);
	return outputs.GetSize();
}

// ----------------------------------------------------------------------------------------
CMemoryNode *CMemoryNode::AllocateNode(MEMNODE_TYPE Type)
{
	CMemoryNode *ret = NULL;

	while (last_memory_location < MEMORY_SIZE)
	{
		if (all_nodes[last_memory_location] == NULL)
		{
			ret = new CMemoryNode(Type, last_memory_location);
			all_nodes[last_memory_location] = ret;
			break;
		}
		last_memory_location++;
	}
	return ret;
}

// ----------------------------------------------------------------------------------------
CMemoryNode *CMemoryNode::FindInputOfType(MEMNODE_TYPE Type)
{
	for (int i = 0; i < inputs.GetSize(); i++)
	{
		if (inputs[i]->type == Type)
			return inputs[i];
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
CMemoryNode *CMemoryNode::FindOutputFor(int Trigger, MEMNODE_TYPE Type)
{
	for (int i = 0; i < outputs.GetSize(); i++)
	{
		if ((outputs[i]->trigger == Trigger) && (outputs[i]->type == Type))
			return outputs[i];
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
CMemoryNode *CMemoryNode::FindOutputOtherThan(MEMNODE_TYPE Type)
{
	for (int i = 0; i < outputs.GetSize(); i++)
	{
		if (outputs[i]->type != Type)
			return outputs[i];
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
CMemoryNode *CMemoryNode::FindOutputTo(int Location)
{
	for (int i = 0; i < outputs.GetSize(); i++)
	{
		if (outputs[i]->location == Location)
			return outputs[i];
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
CMemoryNode *CMemoryNode::NodeAt(int Location, bool Add, MEMNODE_TYPE Type)
{
	if ((Location < 0) || (Location >= MEMORY_SIZE))
		return NULL;

	CMemoryNode *node = all_nodes[Location];
	if ((node == NULL) && Add)
	{
		node = new CMemoryNode(Type, Location);
		all_nodes[Location] = node;
	}
	return node;
}

// ----------------------------------------------------------------------------------------
LPCTSTR CMemoryNode::ToString()
{
	static CString ret;
	ret.Format(_T("%d,%d,%d,%d"), location, type, trigger, outputs.GetCount());
	for (int i = 0; i < outputs.GetCount(); i++)
	{
		if (outputs[i])
			ret.AppendFormat(_T(",%d"), outputs[i]->location);
		else
			ret.Append(_T(",0"));
	}
	if (type == TextSystem)
	{
		ret.AppendFormat(_T(",%c"), char(trigger));
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
		CMemoryNode::all_nodes[i] = NULL;
	}

	memory_root = NodeAt(MindRoot, true, MindRoot);
	text_system.root = NodeAt(TextSystem, true, TextSystem);
}

// ----------------------------------------------------------------------------------------
CMind::~CMind()
{
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		if (CMemoryNode::all_nodes[i])
			delete CMemoryNode::all_nodes[i];
	}
}

// ----------------------------------------------------------------------------------------
void CMemoryNode::ConnectNodes(CMemoryNode *From, CMemoryNode *To)
{
	From->AddOutput(To);
	To->AddInput(From);
}

// ----------------------------------------------------------------------------------------
int CMind::Load(char *Filename)
{
	// int tmp = 0;
	// CStrings words;
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
			// words.Split(line, ' ');
			// int id = words.GetIntAt(0);
			// id = ++tmp;
			// LPCTSTR name = words.GetAt(1);
			CMemoryNode *theThing = text_system.LearnThis(line);
			num++;
		}
		fclose(fp);
	}

	return num;
}

// ----------------------------------------------------------------------------------------
CMemoryNode *CMind::NodeAt(int Location, bool Add, MEMNODE_TYPE Type)
{
	return CMemoryNode::NodeAt(Location, Add, Type);
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
		for (int i = 0; i < CMemoryNode::last_memory_location; i++)
		{
			CMemoryNode *n = NodeAt(i);
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
bool CMind::Think(CString& Output)
{
	static int cycle = 1;
	static int last_ob = 1;

	++cycle;
	if (cycle % 10 == 0)
	{
		Output.Format(_T("cleanup ..."), ++cycle);
		return true;
	}
	int i = last_ob + 1;
	while (true)
	{
		CMemoryNode *cur = NodeAt(i);
		if (!cur)
		{
			Output.Format(_T("(end)"));
			last_ob = 1;
			return false;
		}

		if (cur->type == SomeThing)
		{
			last_ob = i;
			Output = text_system.BuildOutput(cur);
			Output.AppendFormat(_T(" (%s)"), cur->ToString());
			return false;
		}
		i++;
	}

	return false;
}

// ----------------------------------------------------------------------------------------
// CTextSystem
// ----------------------------------------------------------------------------------------
CTextSystem::CTextSystem()
{
	root = NULL;
}

// ----------------------------------------------------------------------------------------
// Builds a text string by walking backwards through the pathway.
// ----------------------------------------------------------------------------------------
TCHAR *CTextSystem::BuildOutput(CMemoryNode *PathEnd)
{
	static TCHAR buf[256];
	TCHAR *cp = &buf[255];
	*(--cp) = NULL;

	if ((PathEnd) && (PathEnd->type != TextSystem))
	{
		PathEnd = PathEnd->FindInputOfType(TextSystem);
	}

	while ((PathEnd) && (PathEnd->type == TextSystem) && (PathEnd != root))
	{
		CMemoryNode *prev = PathEnd->FindInputOfType(TextSystem);
		*(--cp) = TCHAR(PathEnd->trigger);
		PathEnd = prev;
	}
	
	return cp;
}

// ----------------------------------------------------------------------------------------
// Learns the text by building a pathway to it.
// Returns the last node in the pathway.
// This can be used as a backpointer to the pathway for output.
// ----------------------------------------------------------------------------------------
CMemoryNode *CTextSystem::LearnThis(LPCTSTR Input, CMemoryNode *Thing)
{
	if (*Input == NULL)
		return NULL;

	CMemoryNode *cur = root;
	last_received = Input;

	while (*Input)
	{
		int ch = *(Input++);
		CMemoryNode *to = cur->FindOutputFor(ch, TextSystem);
		if (!to)
		{
			to = CMemoryNode::AllocateNode(TextSystem);
			to->trigger = ch;
			CMemoryNode::ConnectNodes(cur, to);
		}
		cur = to;
	}

	// At this point, cur is where (Thing) should go ...
	if (Thing == NULL)
	{
		// No "Thing" was passed in, so create one ...
		Thing = cur->FindOutputOtherThan(TextSystem);
		if (Thing == NULL)
		{
			Thing = CMemoryNode::AllocateNode(SomeThing);
			CMemoryNode::ConnectNodes(cur, Thing);
		}
	}
	else
	{
		// See if we already know about it ...
		if (cur->FindOutputTo(Thing->location) == NULL)
		{
			CMemoryNode::ConnectNodes(cur, Thing);
		}
	}

	return cur;
}

// ----------------------------------------------------------------------------------------
// Searches the pathways for the input.
// Returns the recognized thing, or NULL if not recognized.
// NB: if more than one thing is associated with this text, only the first is returned.
// ----------------------------------------------------------------------------------------
CMemoryNode *CTextSystem::RecognizeThis(LPCTSTR Input)
{
	CMemoryNode *cur = root;
	last_received = Input;

	while ((cur) && (*Input))
	{
		int ch = *(Input++);
		cur = cur->FindOutputFor(ch, TextSystem);
	}

	return (cur != NULL) ? cur->FindOutputOtherThan(TextSystem) : NULL;
}

