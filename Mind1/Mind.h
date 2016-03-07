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
	SomeThing, 
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
	CNodeLink() { to = NULL; trigger = 0; type = LINK_Unknown; }
	CNodeLink(CMemoryNode *To, int Trigger, NODELINK_TYPE Type) { to = To; trigger = Trigger; type = Type; }
	CMemoryNode *to;
	int trigger;
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

	CMemoryNode *FindInputOfType(MEMNODE_TYPE Type);

	CMemoryNode *FindLinkTo(int Location);
	CMemoryNode *FindLink(NODELINK_TYPE Type);
	CMemoryNode *FindLink(int Trigger, NODELINK_TYPE Type);
	CMemoryNode *FindLinkThan(MEMNODE_TYPE Type);
	bool HasLinkTo(CMemoryNode *To);

	LPCTSTR ToString();

	int location;
	int value;
	MEMNODE_TYPE type;

	// This node's inputs and outputs
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
	TCHAR *BuildOutput(CMemoryNode *PathEnd);
	
	void FireNode(CMemoryNode *Node) { fire_queue.AddTail(Node); }
	CMemoryNode *LearnThis(LPCTSTR Input, CMemoryNode *Thing = NULL);
	CMemoryNode *LookAt(LPCTSTR Input);
	void ReceiveInput(LPCTSTR Input) { input_queue.AddTail(Input); }
	void WakeUp();

	CMind *mind;
	CMemoryNode *root;

	CList<CMemoryNode *> fire_queue;
	CList<CString> input_queue;

	CString last_received;
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
	int Load(char *Filename);
	CMemoryNode *NodeAt(int Location, bool Add = false, MEMNODE_TYPE Type = MemType_Unknown);
	int Save();
	bool Think(CString& Output);

	CMemoryNode *memory_root;
	CConceptSystem concept_system;
	CExecutiveSystem executive_system;
	CTextSystem text_system;
};
