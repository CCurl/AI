#include "stdafx.h"
#include "Mind.h"

#define BUFSIZE 1024

CMemoryNode *CMemoryNode::all_nodes[MEMORY_SIZE];
int CMemoryNode::last_memory_location = 0;

// ----------------------------------------------------------------------------------------
// CMemoryNode
// ----------------------------------------------------------------------------------------
//int CMemoryNode::AddInput(CMemoryNode *Input)
//{ 
//	inputs.Add(Input);
//	return inputs.GetSize(); 
//}

// ----------------------------------------------------------------------------------------
//int CMemoryNode::AddOutput(CMemoryNode *Output)
//{
//	outputs.Add(Output);
//	return outputs.GetSize();
//}

// ----------------------------------------------------------------------------------------
CMemoryNode::~CMemoryNode()
{
	while (links.GetCount())
	{
		CNodeLink *link = links.GetHead();
		delete link;
		links.RemoveHead();
	}
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
CNodeLink *CMemoryNode::FindLink(int Trigger, NODELINK_TYPE Type)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CNodeLink *link = links.GetNext(pos);
		if ((link->trigger == Trigger) && (link->type == Type))
		{
			return link;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
CNodeLink *CMemoryNode::FindLink(NODELINK_TYPE Type)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CNodeLink *link = links.GetNext(pos);
		if (link->type == Type)
		{
			return link;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
CNodeLink *CMemoryNode::FindLinkOtherThan(MEMNODE_TYPE Type)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CNodeLink *link = links.GetNext(pos);
		if (link->to->type != Type)
		{
			return link;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
CNodeLink *CMemoryNode::FindLinkTo(int Location)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CNodeLink *link = links.GetNext(pos);
		if (link->to->location != Location)
		{
			return link;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
void CMemoryNode::FireLinksNotToType(MEMNODE_TYPE Type, CMind *Mind)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CNodeLink *link = links.GetNext(pos);
		if (link->to->type != Type)
		{
			Mind->Fire(link);
		}
	}
}

// ----------------------------------------------------------------------------------------
void CMemoryNode::LinkTo(CMemoryNode *To, int Trigger, NODELINK_TYPE Type)
{
	links.AddTail(new CNodeLink(To, Trigger, Type));
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
	ret.Format(_T("%d,%d,%d,%d"), location, type, value, links.GetCount());

	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CNodeLink *link = links.GetNext(pos);
		ret.AppendFormat(_T(",%d-%d-%d"), link->type, link->trigger, link->to->location);
	}

	if (type == TextNode)
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
		CMemoryNode::all_nodes[i] = NULL;
	}

	memory_root = NodeAt(MindRoot, true, MindRoot);

	concept_system.root = NodeAt(ConceptSystem, true, ConceptSystem);
	concept_system.mind = this;

	executive_system.root = NodeAt(ConceptSystem, true, ConceptSystem);
	executive_system.mind = this;

	text_system.root = NodeAt(TextSystem, true, TextSystem);
	text_system.mind = this;
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
			//CMemoryNode *theThing = text_system.LearnThis(line);
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

	//Output.Empty();
	text_system.WakeUp();

	while (fire_queue.GetCount() > 0)
	{
		CNodeLink *link = fire_queue.RemoveHead();
		switch (link->type)
		{
		case LINK_TextIn:
		case LINK_TextOut:
			text_system.Fire(link);
			break;

		case LINK_Concept:
			concept_system.Fire(link);
			break;

		default:
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
// CTextSystem
// ----------------------------------------------------------------------------------------
CTextSystem::CTextSystem()
{
	root = NULL;
}

// ----------------------------------------------------------------------------------------
// Builds a text string by walking backwards through the pathway.
// ----------------------------------------------------------------------------------------
LPCTSTR CTextSystem::BuildOutput(CMemoryNode *PathEnd)
{
	output.Empty();
	if ((PathEnd) && (PathEnd->type != TextNode))
	{
		CNodeLink *link = PathEnd->FindLink(LINK_TextOut);
		PathEnd = link ? link->to : NULL;
	}

	while ((PathEnd) && (PathEnd->type == TextNode))
	{
		output.AppendChar(TCHAR(PathEnd->value));
		CNodeLink *link = PathEnd->FindLink(LINK_TextOut);
		PathEnd = link ? link->to : NULL;
	}
	output.MakeReverse();
	return output;
}

// ----------------------------------------------------------------------------------------
// This is where the rubber meets the road.
// ----------------------------------------------------------------------------------------
void CTextSystem::Fire(CNodeLink *Link)
{
	CMemoryNode *node = Link->to;
	if (Link->type == LINK_TextIn)
	{
		TCHAR ch = NextChar();
		if (ch == NULL)
		{
			node->FireLinksNotToType(TextNode, mind);
			// vvvvv --- for testing --- vvvvv
			Link = node->FindLinkOtherThan(TextNode);
			if (Link)
			{
				Link = Link->to->FindLink(LINK_TextOut);
				if (Link)
				{
					mind->Fire(Link);
				}
			}
			// ^^^^^ --- for testing --- ^^^^^
		}
		else
		{
			Link = node->FindLink(ch, LINK_TextIn);
			if (Link)
			{
				mind->Fire(Link);
			}
		}
		return;
	}

	if (Link->type == LINK_TextOut)
	{
		if (node->type == TextSystem)
		{
			last_output = output.MakeReverse();
			output.Empty();
		}
		else if (node->type == TextNode)
		{
			output.AppendChar(TCHAR(node->value));
			Link = node->FindLink(LINK_TextOut);
			if (Link)
			{
				mind->Fire(Link);
			}
		}
	}
}

// ----------------------------------------------------------------------------------------
// Learns the text by building a pathway to it.
// Returns the last node in the pathway.
// This can be used as a backpointer to the pathway for output.
// ----------------------------------------------------------------------------------------
void CTextSystem::LearnThis(LPCTSTR Input, CMemoryNode *Thing)
{
	if (*Input == NULL)
		return;

	CMemoryNode *cur = root;
	last_received = Input;

	while (*Input)
	{
		int ch = *(Input++);
		CNodeLink *link = cur->FindLink(ch, LINK_TextIn);
		CMemoryNode *to = link ? link->to : NULL;
		if (!to)
		{
			to = CMemoryNode::AllocateNode(TextNode);
			to->value = ch;
			cur->LinkTo(to, ch, LINK_TextIn);
			to->LinkTo(cur, cur->value, LINK_TextOut);
		}
		cur = to;
	}

	// At this point, cur is where (Thing) should go ...
	if (Thing == NULL)
	{
		// No "Thing" was passed in, so create one ...
		CNodeLink *link = cur->FindLinkOtherThan(TextNode);
		Thing = link ? link->to : NULL;
		if (Thing == NULL)
		{
			Thing = CMemoryNode::AllocateNode(SomeThing);
			cur->LinkTo(Thing, 0, LINK_Concept);
			Thing->LinkTo(cur, cur->value, LINK_TextOut);
		}
	}
	else
	{
		// See if we already know about it ...
		if (cur->FindLinkTo(Thing->location) == NULL)
		{
			cur->LinkTo(Thing, 0, LINK_Concept);
			Thing->LinkTo(cur, cur->value, LINK_TextOut);
		}
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

	CNodeLink *link = root->FindLink(ch, LINK_TextIn);
	if (link)
	{
		mind->Fire(link);
	}
}

// ----------------------------------------------------------------------------------------
// Checks to see if there are any requests or input to process.
// ----------------------------------------------------------------------------------------
void CTextSystem::WakeUp()
{
	if (input_queue.GetCount() > 0)
	{
		CString input = input_queue.RemoveHead();
		LookAt(input);
	}
}
