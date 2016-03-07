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

class CMind;

// ----------------------------------------------------------------------------------------
// CMemoryNode
// ----------------------------------------------------------------------------------------
class CMemoryNode
{
public:
	CMemoryNode() { type = MemType_Unknown; location = 0; trigger = 0; }
	CMemoryNode(MEMNODE_TYPE Type) { type = Type; location = 0; trigger = 0; }
	CMemoryNode(MEMNODE_TYPE Type, int Loc) { type = Type; location = Loc; trigger = 0; }

	int AddInput(CMemoryNode *Other);
	int AddOutput(CMemoryNode *Other);

	CMemoryNode *FindInputOfType(MEMNODE_TYPE Type);

	CMemoryNode *FindOutputTo(int Location);
	CMemoryNode *FindOutputFor(int Trigger, MEMNODE_TYPE Type);
	CMemoryNode *FindOutputOtherThan(MEMNODE_TYPE Type);

	LPCTSTR ToString();

	int location;
	int trigger;
	MEMNODE_TYPE type;

	// This node's inputs and outputs
	CArray<CMemoryNode *>inputs;
	CArray<CMemoryNode *>outputs;

	// Class statics ...
	static CMemoryNode *AllocateNode(MEMNODE_TYPE Type = MemType_Unknown);
	static void ConnectNodes(CMemoryNode *From, CMemoryNode *To);
	static CMemoryNode *NodeAt(int Location, bool Add = false, MEMNODE_TYPE Type = MemType_Unknown);
	static CMemoryNode *all_nodes[MEMORY_SIZE];
	static int last_memory_location;
};


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
