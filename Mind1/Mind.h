#pragma once

#include "StringUtils.h"

#define FN_THEMIND "TheMind.txt"
#define FN_CONCEPTS "Concepts.txt"
#define FN_ASSOCS "Associations.txt"
#define FORGET_THRESHOLD 5

#define MEMORY_SIZE 1024*1024

typedef enum {
	MindRoot = 0, TextSystem, SomeThing, MemType_Unknown = 999
} MEMNODE_TYPE;

typedef enum {
	LINK_Unknown, LINK_TextIn, LINK_TextOut, LINK_Concept,
} NODELINK_TYPE;

class CMind;
class CMemoryNode;

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

	// int AddInput(CMemoryNode *Other);
	// int AddOutput(CMemoryNode *Other);
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
	//CArray<CMemoryNode *>inputs;
	//CArray<CMemoryNode *>outputs;
	CList<CNodeLink *> links;

	// Class statics ...
	static CMemoryNode *AllocateNode(MEMNODE_TYPE Type = MemType_Unknown);
	//static void ConnectNodes(CMemoryNode *From, CMemoryNode *To);
	static CMemoryNode *NodeAt(int Location, bool Add = false, MEMNODE_TYPE Type = MemType_Unknown);
	static CMemoryNode *all_nodes[MEMORY_SIZE];
	static int last_memory_location;
};


// ----------------------------------------------------------------------------------------
// CTextSystem
// ----------------------------------------------------------------------------------------
class CTextSystem
{
public:
	CTextSystem();
	TCHAR *BuildOutput(CMemoryNode *PathEnd);
	CMemoryNode *LearnThis(LPCTSTR Input, CMemoryNode *Thing = NULL);
	CMemoryNode *RecognizeThis(LPCTSTR Input);

	CMemoryNode *root;
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
	CTextSystem text_system;
};
