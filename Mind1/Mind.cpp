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
CNodeLink *CMemoryNode::FindLink(int Threshold, NODELINK_TYPE Type)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CNodeLink *link = links.GetNext(pos);
		if ((link->threshold == Threshold) && (link->type == Type))
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
		if (link->to->location == Location)
		{
			return link;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------
void CMemoryNode::FireLinksEqualTo(NODELINK_TYPE Type, int Threshold, CMind *Mind)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CNodeLink *link = links.GetNext(pos);
		if ((link->type == Type) && (link->threshold == Threshold))
		{
			Mind->Fire(link);
		}
	}
}

// ----------------------------------------------------------------------------------------
void CMemoryNode::FireLinksOfType(NODELINK_TYPE Type, CMind *Mind)
{
	POSITION pos = links.GetHeadPosition();
	while (pos)
	{
		CNodeLink *link = links.GetNext(pos);
		if (link->type == Type)
		{
			Mind->Fire(link);
		}
	}
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
void CMemoryNode::LinkTo(CMemoryNode *To, int Threshold, NODELINK_TYPE Type)
{
	links.AddTail(new CNodeLink(To, Threshold, Type));
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
		ret.AppendFormat(_T(",%d-%d-%d"), link->type, link->threshold, link->to->location);
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

	text_system.root = NodeAt(TextSystem, true, TextSystem);
	text_system.mind = this;

	concept_system.root = NodeAt(ConceptSystem, true, ConceptSystem);
	concept_system.mind = this;

	executive_system.root = NodeAt(ExecutiveSystem, true, ExecutiveSystem);
	executive_system.mind = this;

	memory_root->LinkTo(NodeAt(ConceptSystem), 0, LINK_Wakeup);
	memory_root->LinkTo(NodeAt(ExecutiveSystem), 0, LINK_Wakeup);
	memory_root->LinkTo(NodeAt(TextSystem), 0, LINK_Wakeup);
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
void CMind::FireOne(CNodeLink *Link)
{
	switch (Link->type)
	{
	case LINK_Wakeup:
		text_system.Fire(Link);
		concept_system.Fire(Link);
		executive_system.Fire(Link);
		break;

	case LINK_TextIn:
	case LINK_TextOut:
		text_system.Fire(Link);
		break;

	case LINK_Concept:
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
	CNodeLink wakeup_link(NULL, 0, LINK_Wakeup);
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
void CConceptSystem::Fire(CNodeLink *Link)
{
	switch (Link->type)
	{
	case LINK_Wakeup:
		break;

	case LINK_Concept:
	{
		// vvvvvvvvvv --- Testing --- vvvvvvvvvvvv
		CMemoryNode *node = mind->NodeAt(Link->to->location - 1);
		while (node)
		{
			if (node->type == ConceptNode)
			{
				node->FireLinksOfType(LINK_TextOut, mind);
				break;
			}
			node = mind->NodeAt(node->location - 1);
		}

		Link->to->FireLinksOfType(LINK_TextOut, mind);
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
LPCTSTR CTextSystem::BuildOutput(CMemoryNode *PathEnd)
{
	CString output;

	// Handle the case where the node is not a TextNode.
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
void CTextSystem::Fire(CNodeLink *Link)
{
	switch (Link->type)
	{
	case LINK_Wakeup:
		if (input_queue.GetCount() > 0)
		{
			CString input = input_queue.RemoveHead();
			LookAt(input);
		}
		break;

	case LINK_TextIn:
	{
		TCHAR ch = NextChar();
		if (ch == NULL)
		{
			Link->to->FireLinksNotToType(TextNode, mind);
		}
		else
		{
			Link->to->FireLinksEqualTo(LINK_TextIn, ch, mind);
		}
	}
		break;

	case LINK_TextOut:
		BuildOutput(Link->to);
		break;

	default:
		break;
	}
}

// ----------------------------------------------------------------------------------------
// Learns the text by building a pathway to it.
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
		// No "Thing" was passed in, so create a new one ...
		CNodeLink *link = cur->FindLinkOtherThan(TextNode);
		Thing = link ? link->to : NULL;
		if (Thing == NULL)
		{
			Thing = CMemoryNode::AllocateNode(ConceptNode);
		}
	}

	// See if we already know about it ...
	if (cur->FindLinkTo(Thing->location) == NULL)
	{
		cur->LinkTo(Thing, 0, LINK_Concept);
		Thing->LinkTo(cur, cur->value, LINK_TextOut);
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

	root->FireLinksEqualTo(LINK_TextIn, ch, mind);
}
