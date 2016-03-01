#include "stdafx.h"
#include "Mind.h"

#define ONE_K (1024)
#define FOUR_K (1024*4)
#define ONE_MB (1024*1024)

CMind *CMind::theMind;
int CMemoryTime::time = 0;

CMind::CMind()
{
	time = next_concept_id = 0;
	theMind = this;
}

CMind::~CMind()
{
}

int CMind::TakeInput_Auditory(CString& theInput)
{
	int start = 0;
	return start;
}

int CMind::TakeInput_Text(CString& theInput)
{
	theInput.AppendChar(' ');
	int start = time;
	CTextMemory *aud = NULL;
	TCHAR *tcp = theInput.GetBuffer();
	while (*tcp)
	{
		TCHAR tch = *(tcp++);
		char ch = (char)tch;
		if (ch == ' ')
		{
			ch = NULL;
		}
		this->memory.RememberNew(new CTextMemory(ch));
	}

	return start;
}

int CMind::TakeInput_Visual(CString& theInput)
{
	return 0;
}

void CMind::Think(CString& Output)
{
	static int cycle = 0;
	Output.Format(_T("thought %d"), ++cycle);
}

void CMemory::Dump(CString& Output)
{
	Output.Empty();
	for (int i = 0; i < num_used; i++)
	{
		CMemoryBase *mem = Recall(i);
		if (mem == NULL)
		{
			continue;
		}
		else if (mem->type == Text)
		{
			CTextMemory *m = (CTextMemory *)mem;
			char ch = m->ch;
			Output.AppendFormat(_T("%d: text %c\r\n"), i, ch ? ch : '.');
		}
		else if (mem->type == Audio)
		{
		}
		else if (mem->type == Concept)
		{
		}
		else if (mem->type == Association)
		{
		}
	}
	int xxx = 0;
	int yyy = xxx + 1;
}

CMemoryBase *CMemory::Recall(int Index)
{
	CMemoryBase *theVal = NULL;
	if (theMemory.Lookup(Index, theVal))
	{
		return theVal;
	}
	return NULL;
}

int CMemory::RememberNew(CMemoryBase *Mem)
{
	theMemory.SetAt(num_used++, Mem);
	return num_used;
}
