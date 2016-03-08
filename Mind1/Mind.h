#pragma once

#include "StringUtils.h"

#define FN_THEMIND "TheMind.txt"
#define FN_CONCEPTS "Concepts.txt"
#define FN_ASSOCS "Associations.txt"
#define FORGET_THRESHOLD 5

#define MEMORY_SIZE 1024*1024

typedef enum {
	MindRoot = 0, 
	// Systems first
	TextSystem, ConceptSystem, ExecutiveSystem, 

	TextNode, ConceptNode, ExecutiveNode,
	MemType_Unknown = 999
} MEMNODE_TYPE;

typedef enum {
	LINK_Unknown, LINK_TextIn, LINK_TextOut, LINK_Concept,
} NODELINK_TYPE;

class CMind;
class CMemoryNode;
class CConceptSystem;
class CExecutiveSystem;
class CTextSystem;

// ----------------------------------------------------------------------------------------
// CNodeLink
// ----------------------------------------------------------------------------------------
class CNodeLink
{
public:
	CNodeLink() { to = NULL; threshold = 0; type = LINK_Unknown; }
	CNodeLink(CMemoryNode *To, int Threshold, NODELINK_TYPE Type) { to = To; threshold = Threshold; type = Type; }
	CMemoryNode *to;
	int threshold;
	NODELINK_TYPE type;
};

// ----------------------------------------------------------------------------------------
// CMemoryNode
// ----------------------------------------------------------------------------------------
class CMemoryNode
{
public:
	CMemoryNode() { type = MemType_Unknown; location = 0; value = 0; }
	CMemoryNode(MEMNODE_TYPE Type) { type = Type; location = 0; value = 0; }
	CMemoryNode(MEMNODE_TYPE Type, int Loc) { type = Type; location = Loc; value = 0; }
	~CMemoryNode();

	void LinkTo(CMemoryNode *To, int Trigger, NODELINK_TYPE Type);

	CNodeLink *FindLinkTo(int Location);
	CNodeLink *FindLink(NODELINK_TYPE Type);
	CNodeLink *FindLink(int Threshold, NODELINK_TYPE Type);
	CNodeLink *FindLinkOtherThan(MEMNODE_TYPE Type);

	void FireLinksToType(MEMNODE_TYPE Type, CMind *Mind) {}
	void FireLinksNotToType(MEMNODE_TYPE Type, CMind *Mind);
	void FireLinksEqualTo(NODELINK_TYPE Type, int Threshold, CMind *Mind);
	void FireLinksNotEqualTo(NODELINK_TYPE Type, int Threshold, CMind *Mind) {}
	void FireLinksGreaterThan(NODELINK_TYPE Type, int Threshold, CMind *Mind) {}
	void FireLinksLessThan(NODELINK_TYPE Type, int Threshold, CMind *Mind) {}

	bool HasLinkTo(CMemoryNode *To) { return false; }

	LPCTSTR ToString();

	int location;
	int value;
	MEMNODE_TYPE type;

	// This node's links
	CList<CNodeLink *> links;

	// Class statics ...
	static CMemoryNode *AllocateNode(MEMNODE_TYPE Type = MemType_Unknown);
	static CMemoryNode *NodeAt(int Location, bool Add = false, MEMNODE_TYPE Type = MemType_Unknown);
	static CMemoryNode *all_nodes[MEMORY_SIZE];
	static int last_memory_location;
};


// ----------------------------------------------------------------------------------------
// CConceptSystem
// ----------------------------------------------------------------------------------------
class CConceptSystem
{
public:
	CConceptSystem() {}
	void Fire(CNodeLink *Link);

	CMind *mind;
	CMemoryNode *root;
};

// ----------------------------------------------------------------------------------------
// CExecutiveSystem
// ----------------------------------------------------------------------------------------
class CExecutiveSystem
{
public:
	CExecutiveSystem() {}
	void Fire(CNodeLink *Link) {}

	CMind *mind;
	CMemoryNode *root;
};

// ----------------------------------------------------------------------------------------
// CTextSystem
// ----------------------------------------------------------------------------------------
class CTextSystem
{
public:
	CTextSystem();
	LPCTSTR BuildOutput(CMemoryNode *PathEnd);
	
	void Fire(CNodeLink *Link);
	void LearnThis(LPCTSTR Input, CMemoryNode *Thing = NULL);
	void LookAt(LPCTSTR Input);
	TCHAR NextChar() { return cur_offset >= last_received.GetLength() ? NULL : last_received.GetAt(cur_offset++); }
	void ReceiveInput(LPCTSTR Input) { input_queue.AddTail(Input); }
	void WakeUp();

	CMind *mind;
	CMemoryNode *root;

	CList<CString> input_queue;

	CString last_received;
	CString output;
	CString last_output;
	int cur_offset;
};

// ----------------------------------------------------------------------------------------
// CMind
// ----------------------------------------------------------------------------------------
class CMind
{
public:
	CMind();
	~CMind();

	CMemoryNode *AllocateNode(MEMNODE_TYPE Type = MemType_Unknown);
	void Fire(CNodeLink *Link) { if (Link) fire_queue.AddTail(Link); }
	int Load(char *Filename);
	CMemoryNode *NodeAt(int Location, bool Add = false, MEMNODE_TYPE Type = MemType_Unknown);
	int Save();
	bool Think(CString& Output);

	CMemoryNode *memory_root;
	CConceptSystem concept_system;
	CExecutiveSystem executive_system;
	CTextSystem text_system;

	CList<CNodeLink *> fire_queue;
};
