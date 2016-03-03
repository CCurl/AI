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
	CAssociation *assoc = NULL;
	CString *word = Words.GetFirst();
	if (Words.GetCount() > 1)
	{
		assoc = new CAssociation();
		if (ID == 0)
			ID = ++last_assoc_id;
		assoc->id = ID;
		associations.SetAt(assoc->id, assoc);
	}

	while (word)
	{
		CConcept *concept = NULL;
		if (word->GetLength())
		{
			concept = EnsureConcept(*word, 0);
			if (assoc)
			{
				assoc->concepts.AddTail(concept->id);
				concept->associations.Add(assoc->id);
			}
		}
		word = Words.GetNext();
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

// ----------------------------------------------------------------------------------------
void CMind::CleanUpAssociations()
{
	CAssociation *assoc = NULL;
	CConcept *concept = NULL;
	CList<CAssociation *>delete_these;
	int id, num, concept_id;

	POSITION pos = associations.GetStartPosition();
	while (pos)
	{
		associations.GetNextAssoc(pos, id, assoc);

		POSITION pos2 = assoc->concepts.GetHeadPosition();
		while (pos2)
		{
			concept_id = assoc->concepts.GetNext(pos2);
			if (!concepts.Lookup(concept_id, concept))
			{
				delete_these.AddTail(assoc);
				break;
			}
		}
	}

	num = 0;
	pos = delete_these.GetHeadPosition();
	while (pos)
	{
		assoc = delete_these.GetNext(pos);
		associations.RemoveKey(assoc->id);
		delete assoc;
		num++;
	}

	if (num > 0)
	{
		CleanUpAssociations();
	}
}

// ----------------------------------------------------------------------------------------
void CMind::CleanUpConcepts()
{
	CAssociation *assoc = NULL;
	CConcept *concept = NULL;
	CList<CConcept *>concepts_to_delete;
	CList<int> ids_to_delete;
	int id, num, assoc_id;

	POSITION pos = concepts.GetStartPosition();
	while (pos)
	{
		concepts.GetNextAssoc(pos, id, concept);

		num = concept->associations.GetCount();
		for (int i = 0; i < num; i++)
		{
			assoc_id = concept->associations.GetAt(i);
			if (!associations.Lookup(assoc_id, assoc))
			{
				ids_to_delete.AddHead(i);
			}
		}

		while (ids_to_delete.GetCount() > 0)
		{
			int index = ids_to_delete.GetHead();
			concept->associations.RemoveAt(index);
			ids_to_delete.RemoveHead();
		}

		if (concept->associations.GetCount() == 0)
		{
			++concept->forget_hold;
			if (concept->forget_hold > FORGET_THRESHOLD)
				concepts_to_delete.AddTail(concept);
		}
		else
		{
			concept->forget_hold = 0;
		}
	}

	pos = concepts_to_delete.GetHeadPosition();
	while (pos)
	{
		concept = concepts_to_delete.GetNext(pos);
		concepts.RemoveKey(concept->id);
		delete concept;
	}

	if (concepts_to_delete.GetCount() > 0)
	{
		CleanUpAssociations();
	}
}

// ----------------------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------------------
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
bool CMind::Think(CString& Output)
{
	static int cycle = 0;

	if (cycle % 10 == 0)
	{
		CleanUpConcepts();
		CleanUpAssociations();
		Output.Format(_T("cleanup ..."), ++cycle);
		return true;
	}
	else
	{
		Output.Format(_T("Please tell me more."), ++cycle);
	}
	return false;
}

// ----------------------------------------------------------------------------------------
// CAssociation
// ----------------------------------------------------------------------------------------
CAssociation::~CAssociation()
{
}
