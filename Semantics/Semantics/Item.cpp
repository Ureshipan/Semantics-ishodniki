//  ����� ����: ���������� �.�., akizelokro@mail.ru , 2013-2014
//  Author: Tatarintsev V.V., akizelokro@mail.ru, 2013-2014
// Item.cpp : implementation file
//

#include "stdafx.h"
#include "xmlarchive.h"
//#include "XMLSerialization.h"
#include "Item.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString version_of_file = _T("1.0.2");

/////////////////////////////////////////////////////////////////////////////
// CItem - ����� �������� xml-����� ��� ������������ ����� (.fms)

IMPLEMENT_XMLSERIAL(CItem, CObject)

//
// ����������� �� ���������
//
CItem::CItem()
{
	static int sequentialNo = 0;    // �������� �� ������� 

	DateOfCreation = CTime::GetCurrentTime();    // ������� �����
	Version = _T("1.0.1");    // ������� ������ �����


// ������� ��� �������
//	m_intAttribute = sequentialNo++;
//	m_stringAttribute.Format(_T("1.1"), m_intAttribute);
//	m_boolAtribute = false;
//	m_doubleAttribute = m_intAttribute;
//	m_stringArrayAttribute.Add(_T("Ciao"));
//	m_stringArrayAttribute.Add(_T("Hello"));
//	m_stringArrayAttribute.Add(_T("Goodbye"));
}


//
//  ����������
//
CItem::~CItem()
{
}


//
// ������� ������������ (������ � ����/�������� �� �����)
//
void CItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE

	// Do not call CObject::Serialize() , it's not XML aware !
	
	XMLDATA(DateOfCreation);    // ������������ ���� ��������
	XMLDATA(Version);    // ������������ ������ �����

//	XMLDATA(m_boolAtribute);
//	XMLDATA(m_doubleAttribute);
//	XMLDATA(m_stringArrayAttribute);
	
	XMLENDNODE
	version_of_file = Version;
}

////////////////////////////////////////////////////////////////////////////
// CDerivedItem ����������� ����� �� CItem

IMPLEMENT_XMLSERIAL(CDerivedItem, CItem)

//
// ����������� �� ���������
//
CDerivedItem::CDerivedItem()
{
	static int sequentialNo = 0;

	m_enumAttribute = eFIRST;

}

//
// ����������
//
CDerivedItem::~CDerivedItem()
{
}


//
// ������� ������������ (������ � ����/�������� �� �����)
//
void CDerivedItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE

	CItem::Serialize(ar);    // ������������ �������� ������

	XMLINTDATA(m_enumAttribute);
	
	XMLENDNODE

}


////////////////////////////////////////////////////////////////////////////
// CVariableItem - ����� ��� ������������ ������ EntityVariable, ������������ ����������

IMPLEMENT_XMLSERIAL(CVariableItem, CObject)

//
// �����������
// 
CVariableItem::CVariableItem()
{
	Variable = NONE_VARIABLE;    // "�������" ����������
}

//
// ����������
//
CVariableItem::~CVariableItem()
{
}

//
// ������� ������������ (������ � ����/�������� �� �����)
//
void CVariableItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE
//	XMLINTDATA(m_enumAttribute);
	XMLINTDATA(Variable);     // ������������ ���� ����������
	XMLDATA(Label);           // ������������ ����������
	XMLDATA(Index);           // ������������ �������
	XMLDATA(Text);            // ������������ ������ ����������, ����� � ������� � ������
	
	XMLENDNODE
}

//
// �����������, ����������� � �������� ��������� ������ �� ������ EntityVariable
//
CVariableItem::CVariableItem(EntityVariable& ev)
{
	Variable = ev.GetType();    // ��� ����������
	Label = ev.GetLabel().data();   // ������ ����������
	Index = ev.GetIndex().data();   // ������ �������
	Text = ev.GetText()->data();   // ����� ����������, ����� � ������� � ������
}


////////////////////////////////////////////////////////////////////////////
// CParentLinkItem - ����� ��� ������������ ������������ ������ ParentLink

IMPLEMENT_XMLSERIAL(CParentLinkItem, CObject)

//
// ����������� �� ���������
//
CParentLinkItem::CParentLinkItem()
{
	FormulaNumber = -1;
	VariableNumber = -1;
}


//
// ����������
//
CParentLinkItem::~CParentLinkItem()
{
}


//
// ������� ������������ (������ � ����/�������� �� �����)
//
void CParentLinkItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE
	// ����������� ���������� ������ ������������ ������� � ������������ ����������
	XMLINTDATA(FormulaNumber);
	XMLINTDATA(VariableNumber);
	
	XMLENDNODE
}

//
// �����������, ����������� � �������� ���������� ���������� ����� ������� � ���������� ����� ���������� ��� �������� ������������ �����
//
CParentLinkItem::CParentLinkItem(int FormulaNumber_, int VariableNumber_)
{
	VariableNumber = VariableNumber_;
	FormulaNumber = FormulaNumber_;
}



////////////////////////////////////////////////////////////////////////////
// CFormulaItem

IMPLEMENT_XMLSERIAL(CFormulaItem, CObject)

//
// ����������� �� ���������
//
CFormulaItem::CFormulaItem()
{
	Feature = NONE_FEATURE;
	EntityType = NONE_FORMULA;
	Number = -1;
	Redefined = "";
	Redefined_Flag = false;
}


//
// ����������
//
CFormulaItem::~CFormulaItem()
{
	// ���������� ��������� ����������
	for ( int i = 0; i < FreeVariables.GetSize(); i++)
	{
		delete FreeVariables[i];
	}
	FreeVariables.RemoveAll();
}


//
// ������� ������������ (������ � ����/�������� �� �����)
//
void CFormulaItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE
	
	// ����������� ��������� ������
	XMLINTDATA(Number);
	XMLINTDATA(Feature);
	XMLINTDATA(EntityType);
	XMLDATA(Description);
	XMLDATA(Label);
	XMLDATA(Addons);
	XMLDATA(Text);
	if(version_of_file != _T("1.0"))
		XMLDATA(Redefined);
	if((version_of_file != _T("1.0")) && (version_of_file != _T("1.0.1")))
		XMLDATA(Redefined_Flag);

	XMLDATA(FreeVariables);
	XMLDATA(ParentLinks);
	
	XMLENDNODE
}


//
//  �����������, ����������� � �������� ��������� ������ �� ������ Formula
//
CFormulaItem::CFormulaItem(Formula& f)
{
	// ������������� ���������� ���������� �� ������� Formula
	Feature =  f.getFeature();
	Description = f.getDescription()->data();
	Label = f.getLabel()->data();
	Addons = f.getAddOns()->data();
	EntityType = f.getType();
	Text = f.getInitialText()->data();
	Redefined = f.getRedefinedText()->data();
	Redefined_Flag = f.isRedefined();
	vector <EntityVariable *> * freeVariables = f.getFreeVariables();
	for ( size_t i = 0; i < freeVariables->size(); i++)
	{
		EntityVariable * pev = freeVariables->at(i);
		CVariableItem * pvi = new CVariableItem(*pev);
		FreeVariables.Add(pvi);
	}
}


////////////////////////////////////////////////////////////////////////////
// CSetItem

IMPLEMENT_XMLSERIAL(CSetItem, CObject)

//
// ����������� �� ���������
//
CSetItem::CSetItem()
{
	EntityType = FORMULA_SET;
	FormulaNumber = -1;
	Number = -1;
	Redefined = "";
	Redefined_Flag = false;}


//
// ����������
//
CSetItem::~CSetItem()
{
	for ( int i = 0; i < FreeVariables.GetSize(); i++)
	{
		delete FreeVariables[i];
	}
	FreeVariables.RemoveAll();
	for ( int i = 0; i < BaseVariables.GetSize(); i++)
	{
		delete BaseVariables[i];
	}
	BaseVariables.RemoveAll();
}


//
// ������� ������������ (������ � ����/�������� �� �����)
//
void CSetItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE

	// ������������� ���������� ���������� �� ������� FormulaSet
	XMLINTDATA(Number);
	XMLINTDATA(FormulaNumber);
	XMLINTDATA(EntityType);
	XMLDATA(Description);
	XMLDATA(Label);
	XMLDATA(Addons);
	XMLDATA(Text);
	if(version_of_file != _T("1.0"))
		XMLDATA(Redefined);
	if((version_of_file != _T("1.0")) && (version_of_file != _T("1.0.1")))
		XMLDATA(Redefined_Flag);

	XMLDATA(FreeVariables);
	XMLDATA(BaseVariables);	

	XMLENDNODE
}


//
//  �����������, ����������� � �������� ��������� ������ �� ������ FormulaSet
//
CSetItem::CSetItem(FormulaSet& f)
{
	// ������������� ���������� ���������� �� ������� FormulaSet
	Description = f.getDescription()->data();
	Label = f.getLabel()->data();
	Addons = f.getAddOns()->data();
	EntityType = f.getType();
//	Text = f.getText().data();
	// �������� ��������� ����������
	Redefined = f.getRedefinedText()->data();
	Redefined_Flag = f.isRedefined();
	Text = f.getInitialText()->data();
	vector <EntityVariable *> * freeVariables = f.getFreeVariables();
	for ( size_t i = 0; i < freeVariables->size(); i++)
	{
		EntityVariable * pev = freeVariables->at(i);
		CVariableItem * pvi = new CVariableItem(*pev);
		FreeVariables.Add(pvi);
	}
	// ���������� ������� ����������
	vector <EntityVariable *> * baseVariables = f.getBaseVariables();
	for ( size_t i = 0; i < baseVariables->size(); i++)
	{
		EntityVariable * pev = baseVariables->at(i);
		CVariableItem * pvi = new CVariableItem(*pev);
		BaseVariables.Add(pvi);
	}
}



#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
#else
//  ���� ������/�������� ����� ���������� ������������ ���������� ������������

//
// ������������ �������� EntityVariable, Formula � FormulaSet �� CVariableItem, CFormulaItem � CSetItem
// ���������� ��������� � ����� Formula.h
//

//
//	����������� ��� ���������� EntityVariable �� �������������� ������� CVariableItem
//
EntityVariable::EntityVariable(const CVariableItem& vi)
{
	type = vi.Variable;
	label = vi.Label;
	index = vi.Index;
	text = vi.Text;
}


//
//	����������� ��� ���������� Formula  �� �������������� ������ CFormulaItem  ��� ������������
//
Formula::Formula(const CFormulaItem& fi)
{
	label = fi.Label;
	description = fi.Description;
	addons = fi.Addons;
	text = fi.Text;
	feature = fi.Feature;
	type = fi.EntityType;
	redefined_text = fi.Redefined;
	redefined = fi.Redefined_Flag;
	trimWhitespaces(&redefined_text);
	removeDoubledWhitespaces(&redefined_text);
	for ( int i = 0; i < fi.FreeVariables.GetSize(); i++)
	{
		EntityVariable * pv = new EntityVariable(*(fi.FreeVariables[i]));
		freeVariables.push_back(pv);
	}
}


//
//	����������� ��� ���������� FormulaSet �� �������������� ������  CSetItem ��� ������������
//
FormulaSet::FormulaSet(const CSetItem& si)
{
	label = si.Label;
	description = si.Description;
	addons = si.Addons;
	text = si.Text;
	redefined_text = si.Redefined;
	trimWhitespaces(&redefined_text);
	removeDoubledWhitespaces(&redefined_text);
	redefined = si.Redefined_Flag;
	type = si.EntityType;
	for ( int i = 0; i < si.FreeVariables.GetSize(); i++)
	{
		EntityVariable * pv = new EntityVariable(*(si.FreeVariables[i]));
		freeVariables.push_back(pv);
	}
	for ( int i = 0; i < si.BaseVariables.GetSize(); i++)
	{
		EntityVariable * pv = new EntityVariable(*(si.BaseVariables[i]));
		baseVariables.push_back(pv);
	}
}
#endif