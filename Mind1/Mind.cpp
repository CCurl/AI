#include "stdafx.h"
#include "Mind.h"

#define BUFSIZE 1024

// ----------------------------------------------------------------------------------------
// CMind
// ----------------------------------------------------------------------------------------
CMind::CMind()
{
	last_concept_id = 0;
	last_assoc_id = 0;
	//text_tree = new CTTNode('A', 0);
	Load();
}

// ----------------------------------------------------------------------------------------
CMind::~CMind()
{
	int id;
	CAssociation *assoc;
	POSITION pos = concepts.GetStartPosition();
	while (pos)
	{
		associations.GetNextAssoc(pos, id, assoc);
		delete assoc;
	}
	associations.RemoveAll();

	CConcept *concept;
	pos = concepts.GetStartPosition();
	while (pos)
	{
		concepts.GetNextAssoc(pos, id, concept);
		delete concept;
	}
	concepts.RemoveAll();
}

// ----------------------------------------------------------------------------------------
void CMind::DumpConcepts(CString& Output, FILE *fp)
{
	Output.Empty();
	for (int id = 1; id <= last_concept_id; id++)
	{
		CConcept *concept = NULL;
		if (concepts.Lookup(id, concept))
		{
			Output.AppendFormat(_T("%d %s\n"), concept->id, concept->name);
			if (fp)
			{
				fputws(Output, fp);
				Output.Empty();
			}
		}
	}
}

// ----------------------------------------------------------------------------------------
void CMind::DumpAssociations(CString& Output, FILE *fp)
{
	Output.Empty();
	CAssociation *assoc = NULL;
	CConcept *concept = NULL;
	int id = 0;


	for (int id = 1; id <= last_assoc_id; id++)
	{
		if (!associations.Lookup(id, assoc))
		{
			continue;
		}
		Output.AppendFormat(_T("%d "), assoc->id);
		POSITION pos2 = assoc->concepts.GetHeadPosition();
		while (pos2)
		{
			int concept_id = assoc->concepts.GetNext(pos2);
			if (concepts.Lookup(concept_id, concept))
			{
				Output.AppendFormat(_T("%s "), concept->name);
			}
			else
			{
				Output.AppendFormat(_T("notFound(%d) "), concept_id);
			}
		}
		Output.AppendFormat(_T("\n"));
		if (fp)
		{
			fputws(Output, fp);
			Output.Empty();
		}
	}
}

// ----------------------------------------------------------------------------------------
CAssociation *CMind::BuildAssociation(CStrings& Words, int ID)
{
	CAssociation *assoc = new CAssociation();
	CString *word = Words.GetFirst();
	int num = 0;
	while (word)
	{
		if (word->GetLength())
		{
			CConcept *concept = EnsureConcept(*word, 0);
			assoc->concepts.AddTail(concept->id);
			num++;
		}
		word = Words.GetNext();
	}

	if (num > 1)
	{
		if (ID == 0)
			ID = ++last_assoc_id;
		assoc->id = ID;
		associations.SetAt(assoc->id, assoc);
	}
	else
	{
		delete assoc;
		assoc = NULL;
	}

	return assoc;
}

// ----------------------------------------------------------------------------------------
CAssociation *CMind::BuildAssociation(CString& Input, int ID)
{
	CStrings words;
	words.Split(Input, ' ');
	return BuildAssociation(words, ID);
}

void CMind::CleanUpAssociations()
{
	CAssociation *assoc = NULL;
	CConcept *concept = NULL;
	int id = 0;

	for (int id = 1; id <= last_assoc_id; id++)
	{
		if (!associations.Lookup(id, assoc))
		{
			continue;
		}
		POSITION pos2 = assoc->concepts.GetHeadPosition();
		while (pos2)
		{
			int concept_id = assoc->concepts.GetNext(pos2);
			if (!concepts.Lookup(concept_id, concept))
			{
				// delete assoc
			}
		}
	}
}

//int CMind::ProcessAudio(TCHAR *Word)
//{
//	CTTNode *res = NULL;
//	CTTNode *node = text_tree;
//	while (*Word)
//	{
//		TCHAR ch = *(Word++);
//		CTTNode *res = SearchTextNodes(ch, node);
//		if (!res)
//		{
//			res = new CTTNode(ch, 0);
//			res->parent = node->parent;
//			res->next = node->next;
//			node->next = res;
//		}
//
//		ch = *Word;
//		if (ch)
//		{
//			if (!res->child)
//			{
//				res->child = new CTTNode(ch, 0);
//				res->parent = res;
//			}
//			node = res->child;
//		}
//	}
//	if (res->concept_id == 0)
//		res->concept_id = ++last_concept_id;
//	return res->concept_id;
//}

//CTTNode *CMind::SearchTextNodes(TCHAR Ch, CTTNode *Start)
//{
//	while (Start)
//	{
//		if (Start->ch == Ch)
//			break;
//
//		Start = Start->next;
//	}
//	return Start;
//}

// ----------------------------------------------------------------------------------------
CConcept *CMind::EnsureConcept(LPCTSTR Name, int ID)
{
	if (*Name == NULL)
		return NULL;

	int id = ID;
	CConcept *concept = NULL;
	if (!concept_names.Lookup(Name, (void *&)id))
	{
		if (id == 0)
		{
			id = ++last_concept_id;
		}
		concept = new CConcept(id, Name);
		concepts.SetAt(concept->id, concept);
		concept_names.SetAt(concept->name, (void *)concept->id);
	}
	else
	{
		concepts.Lookup(id, concept);
	}
	last_concept_id = max(last_concept_id, id);
	return concept;
}

// ----------------------------------------------------------------------------------------
int CMind::Load()
{
	// int tmp = 0;
	CStrings words;
	int num = 0, max_id = 0;
	char buf[BUFSIZE];
	FILE *fp = NULL;
	CString line;
	fopen_s(&fp, FN_CONCEPTS, "rt");
	if (fp)
	{
		while (fgets(buf, BUFSIZE, fp) == buf)
		{
			line = buf;
			line.TrimRight();
			words.Split(line, ' ');
			int id = words.GetIntAt(0);
			// id = ++tmp;
			LPCTSTR name = words.GetAt(1);
			EnsureConcept(name, id);
			max_id = max(id, max_id);
		}
		fclose(fp);
		last_concept_id = max_id;
	}

	// tmp = 0;
	max_id = 0;
	fopen_s(&fp, FN_ASSOCS, "rt");
	if (fp)
	{
		while (fgets(buf, BUFSIZE, fp) == buf)
		{
			line = buf;
			line.TrimRight();
			words.Split(line, ' ');
			int id = words.GetIntAt(0);
			// id = ++tmp;
			words.RemoveHead();
			CAssociation *assoc = BuildAssociation(words, id);
			max_id = max(id, max_id);
		}
		fclose(fp);
		last_assoc_id = max_id;
	}
	return num;
}

// ----------------------------------------------------------------------------------------
int CMind::PurgeAssociation(int ID)
{
	CAssociation *a = NULL;
	if (associations.Lookup(ID, a))
	{
		associations.RemoveKey(a->id);
		delete a;
	}
	return ID;
}

// ----------------------------------------------------------------------------------------
int CMind::PurgeConcept(int ID, LPCTSTR Name)
{
	if (ID == 0)
	{
		concept_names.Lookup(Name, (void *&)ID);
	}

	CConcept *c = NULL;
	if (concepts.Lookup(ID, c))
	{
		concept_names.RemoveKey(c->name);
		concepts.RemoveKey(c->id);
		CleanUpAssociations();
		delete c;
	}
	return ID;
}

// ----------------------------------------------------------------------------------------
int CMind::Save()
{
	CString line;
	int num = 0;
	FILE *fp = NULL;
	fopen_s(&fp, FN_CONCEPTS, "wt");
	if (fp)
	{
		DumpConcepts(line, fp);
		fclose(fp);
	}

	fopen_s(&fp, FN_ASSOCS, "wt");
	if (fp)
	{
		DumpAssociations(line, fp);
		fclose(fp);
	}
	return num;
}

// ----------------------------------------------------------------------------------------
void CMind::Think(CString& Output)
{
	static int cycle = 0;
	Output.Format(_T("thought %d"), ++cycle);
}

// ----------------------------------------------------------------------------------------
// CAssociation
// ----------------------------------------------------------------------------------------
CAssociation::~CAssociation()
{
}
