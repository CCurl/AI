#pragma once

#include "StringUtils.h"

#define FN_CONCEPTS "Concepts.txt"
#define FN_ASSOCS "Associations.txt"

// Text tree node
// When the concept_id != 0, we are at a concept
//class CTTNode
//{
//public:
//	CTTNode() { next = child = parent = NULL; concept_id = 0; }
//	CTTNode(TCHAR Ch, int ConceptID) { next = child = NULL; ch = Ch;  concept_id = ConceptID; }
//	~CTTNode();
//
//	CTTNode *next, *child, *parent;
//	TCHAR ch;
//	int concept_id;
//};

class CAssociation
{
public:
	CAssociation() { id = 0; }
	~CAssociation();
	int id;
	CList<int> concepts;
};

class CConcept
{
public:
	CConcept(int ID, LPCTSTR Name) { id = ID; name = Name; }
	CConcept() { id = 0; }
	int id;
	CList<int> association_ids;
	CString name;
};

class CMind
{
public:
	CMind();
	~CMind();

	CAssociation *BuildAssociation(CStrings& Words, int ID);
	CAssociation *BuildAssociation(CString& Input, int ID);
	void CleanUpAssociations();
	void DumpConcepts(CString& Output, FILE *fp);
	void DumpAssociations(CString& Output, FILE *fp);
	CConcept *EnsureConcept(LPCTSTR name, int ID);
	void Think(CString& Output);

	int Load();
	int PurgeAssociation(int ID);
	int PurgeConcept(int ConceptID, LPCTSTR Name);
	int Save();

	CMap<int, int, CConcept *, CConcept *> concepts;
	CMap<int, int, CAssociation *, CAssociation *> associations;
	CMapStringToPtr concept_names;

	int last_concept_id;
	int last_assoc_id;

	// Future general pattern processing
	// int ProcessAudio(int *Stream);
	// CTTNode *SearchTextNodes(int val, CTTNode *Start);
	// CTTNode *text_tree;
};
