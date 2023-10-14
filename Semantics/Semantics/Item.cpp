//  Автор кода: Татаринцев В.В., akizelokro@mail.ru , 2013-2014
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
// CItem - класс описания xml-файла для сериализации схемы (.fms)

IMPLEMENT_XMLSERIAL(CItem, CObject)

//
// конструктор по умолчанию
//
CItem::CItem()
{
	static int sequentialNo = 0;    // осталось от примера 

	DateOfCreation = CTime::GetCurrentTime();    // текущее время
	Version = _T("1.0.1");    // текущая версия файла


// оставил как примеры
//	m_intAttribute = sequentialNo++;
//	m_stringAttribute.Format(_T("1.1"), m_intAttribute);
//	m_boolAtribute = false;
//	m_doubleAttribute = m_intAttribute;
//	m_stringArrayAttribute.Add(_T("Ciao"));
//	m_stringArrayAttribute.Add(_T("Hello"));
//	m_stringArrayAttribute.Add(_T("Goodbye"));
}


//
//  деструктор
//
CItem::~CItem()
{
}


//
// функция сериализации (записи в файл/загрузки из файла)
//
void CItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE

	// Do not call CObject::Serialize() , it's not XML aware !
	
	XMLDATA(DateOfCreation);    // сериализация даты создания
	XMLDATA(Version);    // сериализация версии файла

//	XMLDATA(m_boolAtribute);
//	XMLDATA(m_doubleAttribute);
//	XMLDATA(m_stringArrayAttribute);
	
	XMLENDNODE
	version_of_file = Version;
}

////////////////////////////////////////////////////////////////////////////
// CDerivedItem производный класс от CItem

IMPLEMENT_XMLSERIAL(CDerivedItem, CItem)

//
// конструктор по умолчанию
//
CDerivedItem::CDerivedItem()
{
	static int sequentialNo = 0;

	m_enumAttribute = eFIRST;

}

//
// деструктор
//
CDerivedItem::~CDerivedItem()
{
}


//
// функция сериализации (записи в файл/загрузки из файла)
//
void CDerivedItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE

	CItem::Serialize(ar);    // сериализация базового класса

	XMLINTDATA(m_enumAttribute);
	
	XMLENDNODE

}


////////////////////////////////////////////////////////////////////////////
// CVariableItem - класс для сериализации класса EntityVariable, описывающего переменные

IMPLEMENT_XMLSERIAL(CVariableItem, CObject)

//
// конструктор
// 
CVariableItem::CVariableItem()
{
	Variable = NONE_VARIABLE;    // "никакая" переменная
}

//
// деструктор
//
CVariableItem::~CVariableItem()
{
}

//
// функция сериализации (записи в файл/загрузки из файла)
//
void CVariableItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE
//	XMLINTDATA(m_enumAttribute);
	XMLINTDATA(Variable);     // сериализация типа переменной
	XMLDATA(Label);           // сериализация переменной
	XMLDATA(Index);           // сериализация индекса
	XMLDATA(Text);            // сериализация текста переменной, текст в формате с тэгами
	
	XMLENDNODE
}

//
// конструктор, принимающий в качестве параметра ссылку на объект EntityVariable
//
CVariableItem::CVariableItem(EntityVariable& ev)
{
	Variable = ev.GetType();    // тип переменной
	Label = ev.GetLabel().data();   // символ переменной
	Index = ev.GetIndex().data();   // символ индекса
	Text = ev.GetText()->data();   // текст переменной, текст в формате с тэгами
}


////////////////////////////////////////////////////////////////////////////
// CParentLinkItem - класс для сериализации родительских связей ParentLink

IMPLEMENT_XMLSERIAL(CParentLinkItem, CObject)

//
// конструктор по умолчанию
//
CParentLinkItem::CParentLinkItem()
{
	FormulaNumber = -1;
	VariableNumber = -1;
}


//
// деструктор
//
CParentLinkItem::~CParentLinkItem()
{
}


//
// функция сериализации (записи в файл/загрузки из файла)
//
void CParentLinkItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE
	// сериализуем порядковые номера родительской формулы и родительской переменной
	XMLINTDATA(FormulaNumber);
	XMLINTDATA(VariableNumber);
	
	XMLENDNODE
}

//
// конструктор, принимающий в качестве параметров порядковый номер формулы и порядковый номер переменной для указания родительской связи
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
// конструктор по умолчанию
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
// деструктор
//
CFormulaItem::~CFormulaItem()
{
	// освобождаю свободные переменные
	for ( int i = 0; i < FreeVariables.GetSize(); i++)
	{
		delete FreeVariables[i];
	}
	FreeVariables.RemoveAll();
}


//
// функция сериализации (записи в файл/загрузки из файла)
//
void CFormulaItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE
	
	// сериализуем аттрибуты класса
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
//  конструктор, принимающий в качестве аргумента ссылку на объект Formula
//
CFormulaItem::CFormulaItem(Formula& f)
{
	// инициализация аттрибутов значениями из объекта Formula
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
// конструктор по умолчанию
//
CSetItem::CSetItem()
{
	EntityType = FORMULA_SET;
	FormulaNumber = -1;
	Number = -1;
	Redefined = "";
	Redefined_Flag = false;}


//
// деструктор
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
// функция сериализации (записи в файл/загрузки из файла)
//
void CSetItem::Serialize(CArchive& ar)
{
	XMLCLASSNODE

	// инициализация аттрибутов значениями из объекта FormulaSet
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
//  конструктор, принимающий в качестве аргумента ссылку на объект FormulaSet
//
CSetItem::CSetItem(FormulaSet& f)
{
	// инициализация аттрибутов значениями из объекта FormulaSet
	Description = f.getDescription()->data();
	Label = f.getLabel()->data();
	Addons = f.getAddOns()->data();
	EntityType = f.getType();
//	Text = f.getText().data();
	// указываю свободные переменные
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
	// присваиваю базовые переменные
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
//  если запись/загрузка схемы проводится стандартными средствами сериализации

//
// конструкторы объектов EntityVariable, Formula и FormulaSet из CVariableItem, CFormulaItem и CSetItem
// объявления приведены в файле Formula.h
//

//
//	конструктор для построения EntityVariable из соотвествущего классов CVariableItem
//
EntityVariable::EntityVariable(const CVariableItem& vi)
{
	type = vi.Variable;
	label = vi.Label;
	index = vi.Index;
	text = vi.Text;
}


//
//	конструктор для построения Formula  из соотвествущего класса CFormulaItem  для сериализации
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
//	конструктор для построения FormulaSet из соотвествущего класса  CSetItem для сериализации
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