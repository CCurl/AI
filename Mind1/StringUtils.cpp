#include "StdAfx.h"
#include "StringUtils.h"

CStringNode *CStringNode::InsertAfter( LPCTSTR Val )
{
	CStringNode *old_next = next;
	next = new CStringNode( Val );
	next->next = old_next;
	return next;
}

void CStrings::RemoveHead()
{
	if (first_node)
	{
		CStringNode *delete_this = first_node;
		first_node = first_node->Next();
		delete delete_this;
		num--;
	}

	if (first_node == NULL)
	{
		last_node = NULL;
		num = 0;
	}
}

void CStrings::RemoveNodes()
{
	while (first_node)
	{
		RemoveHead();
	}
	last_node = NULL;
	num = 0;
}

LPCTSTR CStrings::GetAt(int Index)
{
	CString *str = StringAt(Index);
	return (str != NULL) ? (*str) : NULL;
}

int CStrings::GetIntAt(int Index)
{
	LPCTSTR str = GetAt(Index);
	return _tstoi(str);
}

CString *CStrings::StringAt(int Index)
{
	if ( Index < 0 )
		return NULL;

	CStringNode *here = first_node;
	while ( (Index > 0) && (here != NULL) )
	{
		here = here->Next();
		Index--;
	}

	return (here != NULL) ? here->Data() : NULL;
}

int CStrings::Join(TCHAR Delim, CString& Target)
{
	num = 0;
	Target.Empty();

	CStringNode *here = first_node;
	while (here != NULL)
	{
		LPCTSTR str = *here->Data();
		++num;
		if (num > 1)
			Target.AppendChar(Delim);
		Target.Append(str);

		here = here->Next();
	}
	return num;
}

int CStrings::Split(LPCTSTR Line, TCHAR Delim)
{
	RemoveNodes();

	CStringNode *last = first_node;
	CString tmp = Line;
	int pos = tmp.Find( Delim );
	while ( pos >= 0 )
	{
		CString val = tmp.Left(pos);
		val.TrimLeft();
		val.TrimRight();

		tmp = tmp.Mid( pos+1 );
		pos = tmp.Find( Delim );

		Append( val );
	}

	if ( ! tmp.IsEmpty() )
	{
		tmp.TrimLeft();
		tmp.TrimRight();
		Append( tmp );
	}

	return num;
}

void CStrings::Append( LPCTSTR Val )
{
	if ( first_node == NULL )
	{
		first_node = new CStringNode( Val );
		last_node = first_node;
	}
	else
	{
		last_node = last_node->InsertAfter( Val );
	}
	num++;
}

CString *CStrings::GetFirst()
{
	cur_node = first_node;
	return cur_node ? cur_node->Data() : NULL;
}

CString *CStrings::GetNext()
{
	if ( cur_node )
		cur_node = cur_node->Next();

	return cur_node ? cur_node->Data() : NULL;
}

void CStrings::Ascii2Unicode(char *Source, CString &Target)
{
	TCHAR ch;
	Target.Empty();
	while (*Source)
	{
		ch = TCHAR(*(Source++));
		Target.AppendChar(ch);
	}
}

void CStrings::Unicode2Ascii(LPCTSTR Source, char *Target)
{
	char ch;
	while (*Source)
	{
		ch = char(*(Source++));
		*(Target++) = ch;
	}
	*(Target++) = (char)NULL;
}
