//  ����� ����: ���������� �.�., akizelokro@mail.ru , 2013-2014
//  Author: Tatarintsev V.V., akizelokro@mail.ru, 2013-2014
#pragma once

#include "common_functions.h"
//#include <stdint.h>



// ������������ ����� ���������
enum EntityBaseType { 
	NONE_FORMULA, // ��������������
	ATOMIC_FORMULA = 1, // ��������� �������
	NEGATIVE_OPERATION = 2, // �������, ���������� ����������� �������� ���������
	QUANTIFIER1_OPERATION = 3, // �������, ���������� ����������� �������� ��������������
	QUANTIFIER2_OPERATION = 4,
	AND_OPERATION = 5,  // �������, ���������� ����������� �������� "�"
	OR_OPERATION = 6,  // �������, ���������� ����������� �������� "���"
	FORMULA_SET = 7,  // ��������� ���������� �������
};

// ������������ ����� ������
enum Formula_Feature {
	NONE_FEATURE,  // ������������
	TRUTH_FEATURE = 1, // ����������
	FALSE_FEATURE = 2, // ��������
	PERFORMED_FEATURE = 3  // ������������
};

// ������������ ����� ���������� �� �������� "���������� ��� ���������"
enum VariableType {
	NONE_VARIABLE,
	VARIABLE = 1, // ����������
	SET = 2 //���������
};


#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
// ��� ����������� ������������ � ������ ������������� ���������� libxml2 ��� ����������/�������� ����� �/�� xml-�����
// ������ �� ���������� ���� ���������� �� ���� ����������
#else
// ���������� libxml �� ������������
// ���������� �������, ������������ ��� ������������
//class CParentLinkItem;
class CVariableItem;  // ��������������� ���������� ������ ��� ������������ ���������-����������
class CFormulaItem;  // ��������������� ���������� ������ ��� ������������ ������, �������������� ������� Formula
class CSetItem;    // ��������������� ���������� ������ ��� ������������ �������� ����������, �������������� ������� FormulaSet
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����� EntityVariable, �������������� ��������-���������� (��� ����������, ��� � ���������
//
class EntityVariable
{
	VariableType type; // ������ ���������� ��� ����������-���������
	tstring label;  // ������-����������� ���������� (�� ������� ������)
	tstring index;  // ����� �������, ������ ������ ��� ���������� �������
	tstring text;  // ����� ���������� (��� � ��������), �������������� � ������������ ������ (� ������)
public:
	// ����������� �� ���������
	EntityVariable() { type = VARIABLE; label = _T("x"); index = _T(""); SetText(); };
	// ����������
	~EntityVariable() {};
	// �����������, ����������� ����������� ����������, ������� � ��� (���������� ��� ���������)
	EntityVariable( const tstring & str, const tstring & index_, VariableType type_) { label = str; index = index_; type = type_; SetText(); };
	// �����������, ��������� ����� ��� ������������ ����������
	EntityVariable( const EntityVariable & ev) { type = ev.type; label = ev.label; index = ev.index; SetText(); };
#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
	// � ������ ���������� ���������� libxml2
#else
	// ����� ���������� libxml2 �� �����������
	//	����������� ��� ���������� Variable � Formula �� ������������� ������� CVariableItem, CFormulaItem ��� ������������
	EntityVariable(const CVariableItem& vi);
#endif


	// �����, ������������ ��������� �� ����� ����������
	tstring * GetText() { return &text; };
	// ����� ��������� ����� ���������� � ������������ TS-������� 
	void SetText() { text = label; if(index.length() > 0) text += _T("<Symbol=/low>") + index; };
	//   ����� ���������� ��� ����������, � ������ ��������������� �����������, ������� � ���� ���������� true, ����� false 
	bool compare(const EntityVariable&);
	// ����� ��������� true, ���� ��������-���������� �������� ���������
	bool isSet() { bool res; (type == SET) ? res=true : res=false; return res; };
	// ����� ���������� true, ���� ��������-���������� �������� ������� ����������
	bool isVariable() { bool res; (type == VARIABLE) ? res=true : res=false; return res; };
	// ���������� ������ �� ������ ���������� (��� �������)
	tstring & GetLabel() { return label; };
	// ���������� ������ �� ������ �������
	tstring & GetIndex() { return index; };
	// ���������� ��� ���������� (������� ���������� ��� ��������-���������)
	VariableType GetType() { return type; };
	// operator ==
//	bool operator == (const EntityVariable& ev);


//	���������� ����������� ��� ���������� EntityVariable
};

bool operator == (EntityVariable& ev1, EntityVariable& ev2);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����� �ntityBase ������������ ��� ����������� ������ �������� dataSubItems, �������� ������� ��� ������� Formula � Set
//
class EntityBase
{
protected:
	vector <EntityVariable *> freeVariables;  // ��������� ����������
	tstring description;  // �������� �������
	tstring addons;  // �������������� ��������
	tstring label; // �������� �����������
	tstring text; // ���� , ������������ ��� ��������, ������� � ��������� ����������, ����� �� ����������� ������ ��� 
	EntityBaseType type; // ��� c�������, ���� �� �������� enum EntityBaseType::

	tstring redefined_text; // ���������������� ����� �������, �������� ����������� ��� ���������������
	bool redefined; // ���� ��� �����������, �������� �� �������

public:
	// ����������� �� ���������
	EntityBase(void);
	// ����������
	~EntityBase(void);

	// ���������� ��������� �� ��������� �������� ���������� ��� ���� ��������
	vector <EntityVariable *> * getFreeVariables() { return &freeVariables; };
	// ���������� ����� ��������
	tstring & getText() { if(redefined_text.length() > 0) return redefined_text; else return text; };
	// ���������� ��������� �� �������� ��������
	tstring * getDescription() { return &description; };
	// ���������� ��������� �� �������������� �������� 
	tstring * getAddOns() { return &addons; };
	// �������� �����
	void SetText(LPCTSTR lpctstr);	
//	void SetText(LPCTSTR lpctstr) {	tstring tstr = lpctstr; if(text.compare(tstr)) redefined_text = tstr; };
	// �������� ��� �����
//	void SetIRedefinedText(LPCTSTR lpctstr) { redefined_text = lpctstr; };
	// �������� ��������
	void SetDescription(LPCTSTR lpctstr) { description = lpctstr; };
	// �������� �������������� ��������
	void SetAddons(LPCTSTR lpctstr) { addons = lpctstr; };
	// ���������� ��������� �� �����������
	tstring * getLabel() {return &label; };
	// ���������� ��������� �� �����������
//	tstring * getInitialText() { if(redefined_text.length() > 0) return &text; else return &redefined_text; };
	tstring * getInitialText() { return &text; };
	// ���������� ��������� �� �����������
	tstring * getRedefinedText() { return &redefined_text; };
	// ���������� ��� ��������
//	EntityBaseType getRealType() { if((type != FORMULA_SET) && isRedefined()) return ATOMIC_FORMULA; else return type; };
	EntityBaseType getType() { if( isRedefined()) return ATOMIC_FORMULA; else return type; };
	// ��������� ��� ��������
	void setType( EntityBaseType type_) { type = type_; };
	// ��������, �������� �� �������� ��������
	bool isFormula() { return ( type != FORMULA_SET ); };
	// ��������, �������� �� �������� ���������� ����������
	bool isSet() { if( type == FORMULA_SET ) return true; else return false; };
	bool isRedefined() { return redefined; };
	void setRedefined() { redefined = true; };
	void setTemporarilyUnredefined() {redefined = false;};


};


class Formula;  // ��������������� ���������� ������ ������
class FormulaSet;  // ��������������� ���������� ������ �������� ����������

typedef tuple<Formula*, EntityVariable *> ParentLink; // ����, ��������� �� ������������ ��������� ������ Formula � ��������� �� ��������� ���������� � ��
// ������ ��� ��������� ����������, ������ ��� �������� ������� ��������� ������ Formula, ���� ��������� �� ��������� ���������� ����� nullptr,
// �� ��������� ���������� �� �������������� ��� ���������� ������� �������, ���� �� ���� ������������ ��� ���������, �� ��� ������ �������������� 
// �������� ���� ���� �������� tuple



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  class Formula, �������������� �������
//
class Formula: public EntityBase
{
//	tstring text; // ���� �������, ������������ ��� ��������, ������� � ��������� ����������, ����� �� ����������� ������ ��� 
	Formula_Feature feature;  // ������������, �������� ��� ���������� �������
	tstring quantifierPrefix;       // �������-������� ��� ���������, ��� ������� ��������� ������������ ��� ���
//	Formula(const Formula &); // ����������� �����������


public:
	vector <ParentLink *> parents;  // ��������� ���������� �� ���������� ���������, �� ������ ������� ������������ �������
	vector <EntityVariable *> possibleVariables; // ��������� ���������� �� ��������� ��� �������� ��������� ����������, ����������� ��������� ��������
	vector <Formula*> possibleFormulas; // ��������� ���������� �� ��������� ������� ��� �������� �������� ����������� "�" ��� "���"

public:
	// ����������� �� ���������
	Formula(void);
	// ����������
	~Formula(void);
#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
#else
	// ���� �� ������������ ���������� libxml2
	// ����������� ���������� ������� Formula �� �������, ������������ �� xml-�����
	Formula(const CFormulaItem& );
#endif

	// ������� ���������� �� ����� ��������� � ���������� ������ �������� ������
	void unsetFreeVariables(vector <EntityVariable *> * unset); 

//  ������ ��� ������ � private � protected ������� ������
	Formula_Feature getFeature() { return feature; };
	// ���������� �������, ���� �������� ��������� ���� ����������� ������ �� ���� �������� 
	tstring * getQuantifierPrefix() { return &quantifierPrefix; };


// ������� - ������ ��� �������� ����� ������


	//	����� ������������ ��� ���������� ���� ��������� �������� ����������� ���� type � ������ ������
	friend void joinAllFormulas(vector <EntityBase *> * entities, EntityBaseType type);
	//	��������� �������� ����������� ���� type � ������� �������, ���� ��������� �������� ��������, ������ ����� �������,
	//	������ � � ������ ������ entities � ���������� ��������� �� ����� �������, ����� NULL
	friend Formula * joinFormula(vector <EntityBase *> * entities, EntityBaseType type, Formula * first, Formula * second);
	//  ���������� ������� ���� type � ������� ������� �� ���� ��������� ���������� ���������
	void quantifierFormula(vector <EntityBase *> * entities, EntityBaseType type);
	//	����� ������ ��������� ������� �� ���� �����, ���������������� ������ ������ - ����������, � ������ - ���������
	void setAtomFormula(const EntityVariable & ev1, const EntityVariable & ev2);
	//	����� ������ ��������� ������� �� ���� �����, ���������������� ������ ������ - ����������, � ������ - ���������, � ��������� � � �����
	//  ���������� true, ���� ������� ��� ������������ �� �� ����������� � ����� � ����� ������� ��������� � �����, ����� false
	bool setAtomFormula(vector <EntityBase *> * entities, const EntityVariable & ev1, const EntityVariable & ev2);
	//  ��������� �������� �������� ���� type � ������� ������� �� ��������� ���������� freeVariable, ���� ��������� �������� ��������, ������ ����� �������,
	//  ������ � � ������ ������ entities � ���������� �� �� ���������, ����� NULL
	Formula* quantifierFormula(vector <EntityBase *> * entities, EntityBaseType type, EntityVariable * freeVariable);
	// ������ �� ������� ������� ����� ������������ ��������, ��� ���������� � �������� � �����
	Formula* quantifierFormula(EntityBaseType type, EntityVariable * freeVariable);
	//  ����� ������������ ��� ���������� �������� ����������� ���� type ������� � �������� �� ������ entities
	friend void quantifierAllFormulas(vector <EntityBase *> * entities, EntityBaseType type);
	//	���������� �������� ��������� �� ���� �������� �� ���������� ������.
	friend void negativeAllFormulas(vector <EntityBase *> * entities);
	// �������� �������, ��������� �������� ��������� ������������� � ������� ������� c ������ ���������
	Formula*  negativeFormula(vector <EntityBase *> * entities);
	// �������� �������, ��������� �������� ��������� ������������� � ������� ������� � ������ ��������� ��� i ������ ����
	Formula*  negativeFormula(vector <EntityBase *> * entities, int i);

// ������� - ��������������� �������, ����������� ������

	// ����������, �������� �� ������� ���������
	bool isAtom() { return (type == ATOMIC_FORMULA); };
	// ����������, ��������� �� ������� ����������� �������� ���������
//	bool isNegative() { return (type == NEGATIVE_OPERATION); };
	bool isNegative() { if( isRedefined()) return false; else return (type == NEGATIVE_OPERATION); };
	// ����������, ��������� �� ������� ������������ �������� ������ �� ���� �����
//	bool isQuantifier() { if((type == QUANTIFIER1_OPERATION) || (type == QUANTIFIER2_OPERATION)) return true; return false; };
	bool isQuantifier() { if( isRedefined()) return false; else if((type == QUANTIFIER1_OPERATION) || (type == QUANTIFIER2_OPERATION)) return true; return false; };
	// ����������, ��������� �� ������� ����������� �������� ���� "�" ���� "���"
//	bool isUnion() { if((type == AND_OPERATION) || (type == OR_OPERATION)) return true; return false; };
	bool isUnion() { if( isRedefined()) return false; else if((type == AND_OPERATION) || (type == OR_OPERATION)) return true; return false; };

//	friend void proccessOneDetail(vector <Formula *> * formulas, tstring * formulaText, Formula * formula);

	// ��������������� �����, ������������ ��� ���������� ������ ������ �����
	friend void proccessOneDetail(vector <EntityBase *> * entities, tstring * formulaText, ParentLink * parent);
	//	������� ��������� ����� ������� � �������� � EntityBase::text
	void putTextOfFormula(vector <EntityBase *> * entities);
	//  ����� � ����������� �� �������� ���� ��������� �������� operation ����������, ���� �� ��������� ��� �������� �������� �� ������ ������� �������,
	//  ���� ����, �� ���������� ��������� �� ������� ������� � ��������� ���������� possibleVariables (��� ����������� ���������) � possibleFormulas
	//  (��� ����������� "�" ��� "���"). ���� �������� �������� ������ ��� (��, ��� ��������, �� ������ ������� ��� ���������) ������������ nullptr
	Formula * operationCandidate(vector <EntityBase *> * entities, EntityBaseType operation);
	// ������ ������������, ����� �� ������������.. ��� ����?
	void SetLabel();
	// ������ ����������� �������
	void SetLabel(vector <EntityBase *> * entities);
	//  ����� ������ ��������� ���������� ��� ������� ������� � ���������� ��������� �� ����, � ������ ������������� ��������� ����� ��������� 
	//  ���������� nullptr
	FormulaSet * makeFormulaSet(vector <EntityBase *> * entities, vector <EntityVariable *> * variables);
	//	����� ����������,  �������� �� ������� ����� �������� ������� ���������� ������ Formula � ��������� ���������� freeVariable
	//  � ������ ���� freeVariable ����� nullptr ��������, ��� ����� ������ �� �������
	bool inChildOf(Formula * formula, EntityVariable * freeVariable = nullptr);
	//  ����� ����������, �������� �� ��� ������� ������� ��������� ��������� ����������. � ������ �������� ���������� ��������� �� ������� �������,
	// ���� ���������� (��� ���������) - ���������� nullptr
	Formula * formulasetCandidate(vector <EntityBase *> * entities);
	//	���������� ����������� ��� ���������� Formula 
//	Formula::Formula(const Formula& fi);

	int getNumberOfParents();
	ParentLink * getParentLink(int n) { if(!isRedefined()) return parents.at(n);  else return nullptr; };

//	friend bool operator == (const Formula &);
};



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  class FormulaSet, �������������� ��������� ���������� ��� ������
//
class FormulaSet : public EntityBase
{
	// ��������� �� �������-��������
	Formula * formula;
	// ��������� �������, �� ������� �������� ��������� ����������, ��������� ����������
	vector <EntityVariable * > baseVariables;
//	vector <ParentLink *> parents;  // ��������� ���������� �� ���������� ��������� � ��������� ����������, �� ������ ������� ������������ ���������


public:
	// ����������� �� ���������
	FormulaSet(void);
	// ����������
	~FormulaSet(void);
	// ����������� ��� ���������� ��������� ���������� �� ��������� �� ������������ ������� � ��������� ������������ ��������� ���������� (�������)
	FormulaSet(Formula * parent, vector <EntityVariable *> * variables);
#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
#else
	// ���� �� ������������ ���������� libxml2
	// ����������� ���������� ������� Formula �� �������, ������������ �� xml-�����
	FormulaSet(const CSetItem& );  
//	���������� ����������� ��� ���������� FormulaSet
//	FormulaSet::FormulaSet(const FormulaSet& si);
#endif


	//  ����� ��������� ����� ��������� ����������
	void setText();
	// ���������� ��������� �� ������, �������������� �������-��������
	Formula * getParentFormula() { return formula; };
	// ������������� �������-��������
	void setParentFormula(Formula * parent) { formula = parent; };
	//  ����� ������ ����������� ��� ��������� ����������
	void SetLabel(vector <EntityBase *> * entities);
	//  ���������� ��� ��������� ���������� ��������� ��� ������ ������ ��������� ���������� - ����������
	bool isVariableBased();  
	// ���������� ��� ��������� ���������� ��������� ��� ������ ��������� ���������� - ��������
	bool isSetBased();
	// ���������� ��������� �� ��������� �������, �� ������� �������� ��������� ����������, ��������� ����������
	vector <EntityVariable *> * getBaseVariables() { return &baseVariables; };
//	bool isMixed(); // ���������� ��� ��������� ���������� ��������� ��� ������ ���������� ������� ����������: ���������� � ��������
};
