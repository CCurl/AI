#pragma once

//#define TAB 9

class CStringNode
{
public:
	CStringNode() { next = NULL; }
	CStringNode( LPCTSTR Val ) { next = NULL; data = Val; }
	CString *Data() { return &data; }
	CStringNode *Next() { return next; }
	CStringNode *InsertAfter( LPCTSTR Val );

private:
	CString data;
	CStringNode *next;
};

class CStrings
{
public:
	CStrings()
	{
		num = 0;
		first_node = NULL;
		last_node = NULL;
		cur_node = NULL;
	}
	~CStrings() { RemoveNodes(); }

	void RemoveHead();
	void RemoveNodes();

	int GetCount() { return num; }
	int Join(TCHAR Delim, CString& Target);
	int Split(LPCTSTR Line, TCHAR Delim);
	void Append( LPCTSTR Val );

	LPCTSTR GetAt(int Index);
	int GetIntAt(int Index);
	CString *StringAt( int Index );

	CString *GetFirst();
	CString *GetNext();

private:
	int num;
	CStringNode *first_node;
	CStringNode *last_node;
	CStringNode *cur_node; // For GetFirst() and GetNext()

public:
	static void Ascii2Unicode(char *Source, CString &Target);
	static void Unicode2Ascii(LPCTSTR Target, char *Source);
};
