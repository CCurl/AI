#pragma once

class CAuditoryMemory;
class CTextMemory;
class CAssociation;
class CConcept;
class CMemorySlot;
class CMind;

#define DEFAULT_MEM_SIZE 4096

typedef enum
{
	Unknown, Concept, Text, Audio, Association
} MEM_TYPE;

class CMemoryTime
{
public:
	static int time;
	static int Time() { return CMemoryTime::time; }
	static int Next() { return ++CMemoryTime::time; }
};

class CMemoryBase
{
public:
	CMemoryBase() { type = Unknown; }
	CMemoryBase(MEM_TYPE Type) { type = Type; }
	int index;
	MEM_TYPE type;
};

class CConceptx : public CMemoryBase
{
public:
	CConceptx(int ConceptID);
	int concept_id;
};

class CAuditoryMemory : public CMemoryBase
{
public:
	CAuditoryMemory() { phoneme = 0; }
	CAuditoryMemory(int Pho) : CMemoryBase(Audio) { phoneme = Pho; }
	TCHAR phoneme;
};

class CTextMemory : public CMemoryBase
{
public:
	CTextMemory(char Ch) : CMemoryBase(Text) { ch = Ch; }
	char ch;
};

class CAssociationX : public CMemoryBase
{
public:
	CAssociationX(int Time, CAuditoryMemory *Aud, CTextMemory *Text)
	{
		time = Time;
		auditory = Aud;
		text = Text;
	}

	CAssociationX()
	{
		time = 0;
		auditory = NULL;
		text = NULL;
	}

	int time;
	CAuditoryMemory *auditory;
	CTextMemory *text;
	CAssociationX *prev;
};

class CConceptMemory : public CMemoryBase
{
public:
	CConceptMemory() : CMemoryBase(Concept) { ref_count = 0; concept_id = next_memory++; }
	int ref_count;
	int concept_id;

	static int next_memory;
};

class CMemory
{
public:
	CMemory() { num_used = 0; }
	void Dump(CString& Output);
	CMemoryBase *Recall(int Index);
	int RememberNew(CMemoryBase *Value);
	void UpdateOld(int Index, CMemoryBase *Value);

	CMap<int, int, CMemoryBase *, CMemoryBase *>theMemory;

	int num_used;
};


class CMind
{
public:
	CMind();
	~CMind();

	int CreateNewConcept(int StartTime);
	int SearchForConcept(int StartTime, CAuditoryMemory *Auditory);
	int TakeInput_Auditory(CString& theInput);
	int TakeInput_Text(CString& theInput);
	int TakeInput_Visual(CString& theInput);

	void Think(CString& Output);

	int time, next_concept_id;
	CMemory memory;

	static CMind *theMind;
};
