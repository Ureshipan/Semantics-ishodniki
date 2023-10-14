//  ����� ����: ���������� �.�., akizelokro@mail.ru , 2013-2014
//  Author: Tatarintsev V.V., akizelokro@mail.ru, 2013-2014
#include "StdAfx.h"
#include "Formula.h"
#include "Operation.h"


//
//   ������� ���������� ��� ����������, � ������ ��������������� �����������, ������� � ���� ���������� true, ����� false 
// 
bool EntityVariable::compare(const EntityVariable& ev)
{
	bool result = false;
	if((this->label == ev.label ) && ( this->index == ev.index ) && ( this->type == ev.type ))   // ���� ��������� ����������� ����������, ������� � ��� ����������-��������
		result = true;
	return result;
}


bool operator == (EntityVariable& ev1, EntityVariable& ev2)
{
	return ev1.compare(ev2);
}

/*
bool EntityVariable::operator==(const EntityVariable& ev)
{
	if( this->compare(ev) )
		return true;
	else
		return false;
}
*/


//
//  ����������� �� ��������� ��� ������ EntityBase
//
EntityBase::EntityBase(void)
{
	redefined = false;
}

//
//  ���������� ������ EntityBase
//
EntityBase::~EntityBase(void)
{
	// ������ ��������� ����������
	for(size_t i = 0; i < freeVariables.size(); i++)
		delete freeVariables.at(i);
}


//
//  ����������� �� ���������
//
Formula::Formula(void)
{
//	dobavka=TCHAR(' '); 
	type = NONE_FORMULA;
	feature = NONE_FEATURE;
}

/*
Formula::Formula(const Formula &f)
{
	type = f.type;
	text = f.text;
	feature = f.feature;
	description = f.description;
	addons = f.addons;
	label = f.label;
	quantifierPrefix = f.quantifierPrefix;
	parents = f.parents;
	for ( size_t i = 0; i < f.getNumberOfParents(); i++)
	{
		ParentLink * curr = f.getParentLink(i);
		tstring * ptstr1 = get<1>(*curr);
		ParentLink * elem;
		if ( ptstr1 == nullptr )
			elem = new ParentLink(get<0>(*curr), ptstr1);
		else
		{
			tstring * ptstr2 = new tstring(*ptstr1);
			elem = new ParentLink(get<0>(*curr), ptstr2);
		}
		parents.push_back(elem);
	}
	for ( size_t i = 0; i < f.freeVariables.size(); i++)
	{
		tstring * ptstr = f.freeVariables.at(i);
		tstring * ptstr1 = new tstring(*ptstr);
		freeVariables.push_back(ptstr1);
	}
}
*/

//
//  ����������
//
Formula::~Formula(void)
{
	// ������� ��������� ���������� �� ���������� ���������
	for(size_t i = 0; i < getNumberOfParents(); i++)
	{
		EntityVariable * ptstr = get<1>(*(getParentLink(i)));
		if ( ptstr != nullptr )    // ���� ���������� ������� � ������������ �����
			delete ptstr;   // �������� ����������, ��������� � ������������ �����
		delete getParentLink(i);   // �������� ������������ �����
	}
	parents.clear();
	// ������� ��������� ���������� �� ��������� ��� �������� ��������� ����������
	for ( size_t i = 0; i < possibleVariables.size(); i++)
		delete possibleVariables.at(i);    // �������� ��������� ���������� �� ���������� ��������� ���������� ��� ���������� ������
	possibleVariables.clear();
}

//
// ������� ���������� �� ����� ��������� � ���������� ������ �������� ������
//
void Formula::unsetFreeVariables(vector <EntityVariable *> * unset)
{
	// ������� ��������� ���������� �� ��������� ��� �������� ��������� ����������
	for ( size_t i = 0; i < possibleVariables.size(); i++)
		delete possibleVariables.at(i);    // �������� ��������� ���������� �� ���������� ��������� ���������� ��� ���������� ������
	possibleVariables.clear();
	// ���� �� ��������� ���������� �������, ��������� �� ��� � ��������� ���������
	for ( size_t i = 0; i < freeVariables.size(); i++)
	{
		EntityVariable * pev = new EntityVariable(*(freeVariables.at(i)));
		possibleVariables.push_back(pev);    // ��������� ��������� ���������� ���������� ��������� ��������� ����������
	}
	// ������� �� ���������� ���������� "� �������� �� ������ ���������"
	for ( size_t i = 0; i < unset->size(); i++ )
	{
		auto j = possibleVariables.begin();     // ������ ������� � ���������� ���������
		bool found = false;
		EntityVariable * pev1 = unset->at(i);    // ��������� ���������� �� ������ "� ��������"
		// ���� �� ��������� ����������
		for ( ; j != possibleVariables.end(); ++j)
		{
//			EntityVariable * pev2 = j;
			EntityVariable * pev2 = *j;
			if ( ( pev1 != nullptr ) && (*pev1 == *pev2) )    // ���� ������� ����������� ����������
			{
				found = true;
				break;
			}
		}
		if ( found )
		{
			// ������ ���������� �� ������ ���������
			delete(*j);
			possibleVariables.erase(j);
		}
	}
}

//
//	����� ������ ��������� ������� �� ���� �����, ���������������� ������ ������ - ����������, � ������ - ���������, � ��������� � � �����
//
bool Formula::setAtomFormula(vector <EntityBase *> * entities, const EntityVariable & ev1, const EntityVariable & ev2)
{
	feature = PERFORMED_FEATURE;    // "���������"
	type = ATOMIC_FORMULA;    // ��������� ��� - ��������� �������
	// ��������� ��� ��������-����������, ������ - ������� ����������, ������ - ���������
	EntityVariable * pev1 = new EntityVariable(ev1); 
	freeVariables.push_back(pev1);
	EntityVariable * pev2 = new EntityVariable(ev2); 
	freeVariables.push_back(pev2);
	putTextOfFormula(entities);    // ������ ����� �������
	// ������ ����������� ��� ��������� �������
	label = _T(" P<Symbol=/low>0 ( ");    // ����� "P" � ������ �������� "0", � ���� ��������� ������ ����� �����������, � ����� ������� ������
	// ���� �� ��������� ���������� �������
	for ( size_t i = 0; i < freeVariables.size(); i++ )
	{
		if ( i > 0 )    // ���� ������ ��� ������ �� ����� ����������
			label += _T(" , ");    // ��������� ������� � ������������ ���������
		label += *(freeVariables.at(i)->GetText());    // ��������� ����� ������� ����������
	}
	label += _T(" ) ");    // ��������� ����������� ������ ������� �������
	bool found = false;
	for(auto curr = entities->begin(); curr != entities->end(); curr++)
	{
		EntityBase * pbase = *curr;
		if( pbase->isFormula() )
		{
			Formula * pformula = (Formula *)pbase;
			if( ( pformula->getType() == ATOMIC_FORMULA ) && ( *this == *pformula ) )
				found = true;
		}
	}
	if(!found)
		entities->push_back(this);    // ��������� ������� ������� � ��������� ��������� ����� entities
	return !found;
}


//
//	����� ������ ��������� ������� �� ���� �����, ���������������� ������ ������ - ����������, � ������ - ���������
//
void Formula::setAtomFormula(const EntityVariable & ev1, const EntityVariable & ev2)
{
	feature = PERFORMED_FEATURE;    // "���������"
	type = ATOMIC_FORMULA;    // ��������� ��� - ��������� �������
	// ��������� ��� ��������-����������, ������ - ������� ����������, ������ - ���������
	EntityVariable * pev1 = new EntityVariable(ev1); 
	freeVariables.push_back(pev1);
	EntityVariable * pev2 = new EntityVariable(ev2); 
	freeVariables.push_back(pev2);
//	putTextOfFormula(entities);    // ������ ����� �������
	// ������ ����������� ��� ��������� �������
	label = _T(" P<Symbol=/low>0 ( ");    // ����� "P" � ������ �������� "0", � ���� ��������� ������ ����� �����������, � ����� ������� ������
	// ���� �� ��������� ���������� �������
	for ( size_t i = 0; i < freeVariables.size(); i++ )
	{
		if ( i > 0 )    // ���� ������ ��� ������ �� ����� ����������
			label += _T(" , ");    // ��������� ������� � ������������ ���������
		label += *(freeVariables.at(i)->GetText());    // ��������� ����� ������� ����������
	}
	label += _T(" ) ");    // ��������� ����������� ������ ������� �������
//	entities->push_back(this);    // ��������� ������� ������� � ��������� ��������� ����� entities
}


//
//	��������� �������� ����������� ���� type � ������� �������, ���� ��������� �������� ��������, ������ ����� �������,
//	������ � � ������ ������ entities � ���������� ��������� �� ����� �������, ����� NULL
//
Formula * joinFormula(vector <EntityBase *> * entities, EntityBaseType type, Formula * first, Formula * second)
{
	Formula * formula = NULL;
	int k = entities->size();
	int t, tt, f, ff;
	bool check, check2;
	if ( ( type == AND_OPERATION ) || ( type == OR_OPERATION ) )
	{
		// ��������, ���������� �� ��� �������� ������� � ����� ��������� � ������ ������������� ���������
		check=true;
		for (t = 0; t < k; t++)
		{
			EntityBase * base = entities->at(t);
			if ( !base->isFormula() )
				continue;
			Formula * curr = (Formula *)base;
			if ( curr->getNumberOfParents() >= 2)
			{
//				if ( (curr->getParentLink(0) == first ) && (curr->getParentLink(1) == second ) && (curr->getType() == type ) )
				if ( (get<0>(*curr->getParentLink(0)) == first ) && (get<0>(*curr->getParentLink(1)) == second ) && (curr->getType() == type ) )
					check=false;
			}
//			if ( (first->getType() == QUANTIFIER1_OPERATION) || (first->getType() == QUANTIFIER2_OPERATION) ||
//					(second->getType() == QUANTIFIER1_OPERATION) || (second->getType() == QUANTIFIER2_OPERATION) )
			if ( first->isQuantifier() || second->isQuantifier() )
					check = false;
		}
		if (check)	// ���� ��������� �������� ���, ��������
		{
			formula = new Formula();
			formula->setType(type);
			formula->feature = NONE_FEATURE;

			Formula * icurr = first;
			Formula * jcurr = second;
//			if ( (icurr->quantifierPrefix.compare(_T(" ")) == 0 ) & ( jcurr->quantifierPrefix.compare(_T(" ")) == 0 ) )  //do kv
			if ( !(icurr->isQuantifier() && jcurr->isQuantifier() ))  //do kv
			{
				if ( (icurr->getFeature() == FALSE_FEATURE) & (type == AND_OPERATION ) ) 
					formula->feature = FALSE_FEATURE;
				if ( (jcurr->getFeature() == FALSE_FEATURE) & ( type == AND_OPERATION ) )  
					formula->feature = FALSE_FEATURE;
				if ( (icurr->getFeature() == FALSE_FEATURE) & ( type == OR_OPERATION  ) )  
					formula->feature = TRUTH_FEATURE;
				if ( (jcurr->getFeature() == FALSE_FEATURE) & ( type == OR_OPERATION ) )  
					formula->feature = FALSE_FEATURE;
			}
			// ��������� ��������� ���������� �� ������������ ������
			t = icurr->freeVariables.size();
			for (tt = 0; tt < t; tt++)
			{
				EntityVariable * pev = new EntityVariable(*icurr->freeVariables.at(tt));
				formula->freeVariables.push_back(pev);
			}
			f = jcurr->freeVariables.size();
			for (tt = 0; tt < f; tt++)
			{
				check2 = true;
				for (ff = 0; ff < t; ff++)
				{
					if ( *formula->freeVariables.at(ff) == *jcurr->freeVariables.at(tt) )
//					if ( formula->freeVariables.at(ff)->GetText()->compare(*jcurr->freeVariables.at(tt)->GetText()) == 0 )
						check2=false;
				}
				if (check2==true)
				{
					EntityVariable * pev1 = new EntityVariable(*jcurr->freeVariables.at(tt));
					formula->freeVariables.push_back(pev1);
				}
			}
			ParentLink * elem1 = new ParentLink(icurr, nullptr);
//			formula->parents.push_back(icurr);
			formula->parents.push_back(elem1);
			ParentLink * elem2 = new ParentLink(jcurr, nullptr);
//			formula->parents.push_back(jcurr);
			formula->parents.push_back(elem2);
			formula->putTextOfFormula(entities);
			formula->SetLabel(entities);
		}
	}
	return formula;
}




//
//	����� ������������ ��� ���������� �������� ����������� ���� type ������� � �������� �� ������ entities
//
void joinAllFormulas(vector <EntityBase *> * entities, EntityBaseType type)
{
	int k = entities->size();
	int i, j;
	// ��� ��������� �����, ���� ��������� �� 0 �� (����� ������ - 2), ������ ������ �� ������� ������ �������, �� �� ������ (����� ������ - 1)
	// ������������� ���� ������ (�� �������� ����������) � ��� ��� ��������������� ������� ��������� ������� ���� ����������� ���� type
	// ���� ���������� ��������, �� � newFormula ����� ��������� ���������, ����� ���������� ������� ��������� � entities
	for (i = 0; i < (k - 1); i++)
	{
		EntityBase * first = entities->at(i);
		if ( !first->isFormula() )
			continue;
		for (j = (i + 1); j < k; j++)
		{
			EntityBase * second = entities->at(j);
			if ( !second->isFormula() )
				continue;
			Formula * newFormula = joinFormula(entities, type, (Formula *)first, (Formula *)second );
			if(newFormula)
				entities->push_back(newFormula);
		}
	}
}

//
//  ��������� �������� �������� ���� type � ������� ������� �� ��������� ���������� freeVariable, ���� ��������� �������� ��������, ������ ����� �������,
//  ������ � � ������ ������ entities � ���������� �� �� ���������, ����� NULL
//
Formula* Formula::quantifierFormula(vector <EntityBase *> * entities, EntityBaseType type, EntityVariable * freeVariable)
{
	Formula * formula = NULL;
	int l, j;
	if ( ( type == QUANTIFIER1_OPERATION ) || ( type == QUANTIFIER2_OPERATION ) )    // �������� �� ��� ��������, ������ ���� ����������� ��������
	{
		tstring str = _T(" ");
		Formula * icurr = this;
		j = icurr->freeVariables.size();
		if (j != 0)
		{
			formula = new Formula();    // ������ ����� �������
			for (l = 0; l < j; l++)     // ���� �� ��������� ���������� ������� �������
			{
				if ( !(*freeVariables.at(l) == *freeVariable) )    // ���� ��������� ��������� ���������� ����������� � ���, �� ������� ������ �������
				{
					EntityVariable * newVar = new EntityVariable(*(icurr->freeVariables.at(l)));
					formula->freeVariables.push_back(newVar);    // ��������� � � ��������� ���������� ����� �������
					continue;    // ������� � ��������� ��������� ����������
				}
				// ������ �������� �� ���������� �����������, ������������ � ���, �� ������� �������� ����� �������
				// ��������� ����������� �������� � ������
				if ( type == QUANTIFIER1_OPERATION )
					str += _T("<Symbol=/34>");
				else 
					str += _T("<Symbol=/36>");

				str += _T("(") + *(icurr->freeVariables.at(l)->GetText());    // ��������� ����� ������� ������ � ����������� ����������
				str += _T(")");    // ��������� ������ ������� ������
			}
			// ������ ������������ ����� ��� ����� �������
			EntityVariable * pev = new EntityVariable(*freeVariable);    // ���������� ��� �����
			ParentLink * plcurr = new ParentLink(this, pev);    // ����������, ���� �����
			entities->push_back(formula);    // ��������� ����� ������� � ������ �����
			formula->parents.push_back(plcurr);    // ��������� ������������ ����� � ���������� �������
			formula->quantifierPrefix = str;    // ����� �������
			formula->type = type;     // ����� ��� ��������
			formula->feature = NONE_FEATURE;    // �������������� - � ���� (�������)
			formula->putTextOfFormula(entities);     // ������ ��� ����� �������
			// �������� ����������� ��� ���� �������, ���������� ������������ ��������
			formula->SetLabel(entities);
		}
	}
	return formula;
}

//
// ������ �� ������� ������� ����� ������������ ��������, ��� ���������� � �������� � �����
//
Formula* Formula::quantifierFormula(EntityBaseType type, EntityVariable * freeVariable)
{
	Formula * formula = nullptr;
	int l, j;
	if ( ( type == QUANTIFIER1_OPERATION ) || ( type == QUANTIFIER2_OPERATION ) )    // �������� �� ��� ��������, ������ ���� ����������� ��������
	{
		tstring str = _T(" ");
		Formula * icurr = this;
		j = icurr->freeVariables.size();
		if (j != 0)
		{
			formula = new Formula();    // ������ ����� �������
			for (l = 0; l < j; l++)     // ���� �� ��������� ���������� ������� �������
			{
				if ( !(*freeVariables.at(l) == *freeVariable) )    // ���� ��������� ��������� ���������� ����������� � ���, �� ������� ������ �������
				{
					EntityVariable * newVar = new EntityVariable(*(icurr->freeVariables.at(l)));
					formula->freeVariables.push_back(newVar);    // ��������� � � ��������� ���������� ����� �������
					continue;    // ������� � ��������� ��������� ����������
				}
				// ������ �������� �� ���������� �����������, ������������ � ���, �� ������� �������� ����� �������
				// ��������� ����������� �������� � ������
				if ( type == QUANTIFIER1_OPERATION )
					str += _T("<Symbol=/34>");
				else 
					str += _T("<Symbol=/36>");

				str += _T("(") + *(icurr->freeVariables.at(l)->GetText());    // ��������� ����� ������� ������ � ����������� ����������
				str += _T(")");    // ��������� ������ ������� ������
			}
			// ������ ������������ ����� ��� ����� �������
			EntityVariable * pev = new EntityVariable(*freeVariable);    // ���������� ��� �����
			ParentLink * plcurr = new ParentLink(this, pev);    // ����������, ���� �����
//			entities->push_back(formula);    // ��������� ����� ������� � ������ �����
			formula->parents.push_back(plcurr);    // ��������� ������������ ����� � ���������� �������
			formula->quantifierPrefix = str;    // ����� �������
			formula->type = type;     // ����� ��� ��������
			formula->feature = NONE_FEATURE;    // �������������� - � ���� (�������)
			// �������� ����������� ��� ���� �������, ���������� ������������ ��������
//    ���� ����� ��������
//			formula->SetLabel(entities);
		}
	}
	return formula;
}




//
//  ���������� ������� ���� type � ������� ������� �� ���� ��������� ���������� ���������
//
void Formula::quantifierFormula(vector <EntityBase *> * entities, EntityBaseType type)
{
	Formula * formula = nullptr;
	int l, j;
	if ( ( type == QUANTIFIER1_OPERATION ) || ( type == QUANTIFIER2_OPERATION ) )     // ��������, ��� ��� �������� ������ ����������� ��������
	{
		tstring str = _T(" ");
		Formula * icurr = this;
		// � ����� �� ��������� ���������� ������� ��������� �������� ����������� �������� �� ��������� ��������� ����������
		j = icurr->freeVariables.size();
		if (j != 0)
		{
			for (l = 0; l < j; l++)
			{
				formula = quantifierFormula(entities, type, freeVariables.at(l));	// ����� �������, �������� ����� ������� ���� ����������� ��������
//				if ( formula != nullptr )
//					formulas->push_back(formula);
			}
		}
	}
//	return formula;
}
//
//  ��������� �������� �������� ���� type � ������ ������
//
void quantifierAllFormulas(vector <EntityBase *> * entities, EntityBaseType type)
{
	tstring str;
	// � ����� ���� �������, ������� � �������� ��������� ����������� �������� �� ���� ��������� ���������� ���������
	for (size_t i = 0; i <entities->size(); i++)
	{
		EntityBase * base = entities->at(i);
		if ( !base->isFormula() )
			continue;
		Formula * icurr = (Formula *)base;
		icurr->quantifierFormula(entities, type);
	}
}

Formula* Formula::negativeFormula(vector <EntityBase *> * entities)
{
	return this->negativeFormula(entities, 0);
}
//
// �������� �������, ��������� �������� ��������� ������������� � ������� �������, ��� i ������ ���� ��� ������ ��������
//
Formula* Formula::negativeFormula(vector <EntityBase *> * entities, int i)
{
	Formula * formula = nullptr;
	Formula * icurr = this;
//	if(icurr->type != NEGATIVE_OPERATION)     // ��������, ��� �������� ������ �������� ���������
//	{
		formula = new Formula();    // ����� �������
		ParentLink * elem = new ParentLink(icurr, nullptr);    // ������ ������������ �����
		formula->parents.push_back(elem);    // ��������� ��� ����� �������
//		formula->parents.push_back(icurr);
		formula->type = NEGATIVE_OPERATION;    // ��� ����� ������� - �������� ���������
		formula->feature = NONE_FEATURE;    // �������������� �������� ��������������
			   
		// ��������� �������������� ����� ������� � ����������� �� �������-��������
		if (icurr->feature == TRUTH_FEATURE)    
			formula->feature = FALSE_FEATURE;
		if (icurr->feature == FALSE_FEATURE) 
			formula->feature = TRUTH_FEATURE;
		if (icurr->feature == PERFORMED_FEATURE) 
			formula->feature = PERFORMED_FEATURE;

		// ���� �� ��������� ���������� �������-��������, ��������� �� ��� � ������ ��������� ���������� ����� �������
		for (size_t t = 0; t < icurr->freeVariables.size(); t++)
		{
			EntityVariable * str = new EntityVariable(*icurr->freeVariables.at(t));
			formula->freeVariables.push_back(str);  
		}
		formula->putTextOfFormula(entities);    // ������ ����� �������
		formula->SetLabel(entities);    // �������� ����������� ��� ���� �������, ���������� ���������� �������� ���������
	bool found = false;
	for(auto curr = entities->begin(); curr != entities->end(); curr++)
	{
		EntityBase * pbase = *curr;
		if(i == 0)
		{
			if( pbase->isFormula() )
			{
				Formula * pformula = (Formula *)pbase;
				if( *formula == *pformula )
				{
					found = true;
					break;
				}
			}
		}
		else
		{
			if(pbase == (EntityBase *)formula)
			{
				found = true;
				break;
			}
		}
	}
	if(!found)
		entities->push_back(formula);    // ��������� ��������� ������� � ������ ���������
	else
	{
		delete formula;
		formula = nullptr;
	}
	return formula;
}

//
//	���������� �������� ��������� �� ���� �������� �� ���������� ������.
//
void negativeAllFormulas(vector <EntityBase *> * entities)
{
	int k = entities->size();
	for (int i = 0; i < k; i++)
	{
		EntityBase * base = entities->at(i);
		if ( !base->isFormula() )
			continue;
		Formula * icurr = (Formula *)base;
		if (icurr->isNegative()) 
			continue;
		icurr->negativeFormula(entities);
	}
}


//
//	���������� �������� ��������� �� ���� �������� �� ���������� ������.
//
void proccessOneDetail(vector <EntityBase *> * all_entities, tstring * formulaText, ParentLink* parent)
{

	Formula * ncurr = get<0>(*parent);
	
	if ( ncurr->isNegative() )
	{
		*formulaText += _T("<Symbol=/216>");
//		*formulaText += _T(" not ");
//		Formula * parent = ncurr->getParentLink(0);
		Formula * parent = get<0>(*ncurr->getParentLink(0));
		tstring prom(parent->getText());
		if ( !parent->isQuantifier() )
		{
			if(parent->getRedefinedText()->length() == 0)
				prom = prom.substr(3, prom.length() - 5);
		}
		*formulaText += prom;
	}
	else
	{
		if(ncurr->getRedefinedText()->length() > 0)
		{
			*formulaText += ncurr->getText();
		}
		else
		{
			*formulaText += _T(" ( ");
			if( ncurr->type == ATOMIC_FORMULA )
				*formulaText += *(ncurr->freeVariables.at(0)->GetText());
			else 
//				proccessOneDetail(all_formulas,formulaText, ncurr->getParentLink(0));
				proccessOneDetail(all_entities,formulaText, ncurr->getParentLink(0));
    
//			if ( ncurr->svjazka == TCHAR('0')) 
			if ( ncurr->getType() == ATOMIC_FORMULA) 
				*formulaText += _T("<Symbol=/0xCE>");
//			*formula += _T(" (- ");
//			else if ( ncurr->svjazka == TCHAR('1')) *formula += _T("<Symbol=/and>");
			else if ( ncurr->getType() == AND_OPERATION) 
				*formulaText += _T("<Symbol=/and>");
//			else if ( ncurr->svjazka == TCHAR('2')) *formula += _T("<Symbol=/or>");
			else if ( ncurr->getType() == OR_OPERATION ) 
				*formulaText += _T("<Symbol=/or>");
//			else if ( ncurr->svjazka == TCHAR('2')) *formula += _T(" v ");

			if( ncurr->getType() == ATOMIC_FORMULA )
				*formulaText += *(ncurr->freeVariables.at(1)->GetText());
			else 
//				proccessOneDetail(all_formulas,formulaText, ncurr->getParentLink(1));
				proccessOneDetail(all_entities,formulaText, ncurr->getParentLink(1));
			*formulaText += _T(" ) ");
		}
	} 
}



//
//	������� ����������,  �������� �� ������� ����� �������� ������� ���������� ������ Formula � ��������� ���������� freeVariable
//  � ������ ���� freeVariable ����� nullptr ��������, ��� ����� ������ �� �������
//
bool Formula::inChildOf(Formula * formula, EntityVariable * freeVariable)
{
	bool res = false;     
	if(!this->isRedefined())
	{
	for(size_t i = 0; i < getNumberOfParents(); i++)    // ������� ���� ������������ ������
	{
		ParentLink * plcurr = getParentLink(i);     // ��������� ������������ �����
		if ( get<0>(*plcurr) == formula )     // ���� ������� � ������������ ����� ��������� � ����������� ��������
		{
			if ( freeVariable == nullptr )     // ���� freeVariable ����� nullptr, �� ����������� ������������ ������� ��� ��� ����������� 
			// �� ��������� ���������� ������������ �����, �� ���� ���������� �������� ������ �� �������
			{
				res = true;    // ��������
				break;
			}
			else    // ���� �������� ��� � �� �������, � �� ����������� ��������� ����������
			{
				EntityVariable * ptstr = get<1>(*plcurr);    // ��������� ���������� �� ��������� ������������ ����� (nullptr ��� ���������� ����������
//				if ( ( ptstr != nullptr ) && ( ptstr->compare(*freeVariable) == 0) )
				if ( ( ptstr != nullptr ) && ( *ptstr == *freeVariable ) )    // ���� ��������� ���������� ���������
				{
					res = true;    // ��������
					break;
				}
			}
		}
	}
	}
	return res;
}

//
//	������� ��������� ����� ������� � �������� � EntityBase::text
//
void Formula::putTextOfFormula(vector <EntityBase *> * entities)
{
	text.clear();
	text = *(getQuantifierPrefix());

//		text += _T("<Symbol=/123>");
	
	EntityBaseType ft = this->getType();
	if ( ( ft == QUANTIFIER1_OPERATION ) || ( ft == QUANTIFIER2_OPERATION ) )
	{
//		Formula * f = this->getParentLink(0);
		ParentLink * ppl = this->getParentLink(0);
		Formula * f = get<0>(*ppl);
		text += f->getText();
	}
	else
	{
		text += _T(" [ ");

//		proccessOneDetail(formulas, &text, this);
		ParentLink pl(this, nullptr);
		proccessOneDetail(entities, &text, &pl);
 
//		res_formula += _T("<Symbol=/125>");
		text += _T(" ]");
	}
}

//
//  ������� � ����������� �� �������� ���� ��������� �������� operation ����������, ���� �� ��������� ��� �������� �������� �� ������ ������� �������,
//  ���� ����, �� ���������� ��������� �� ������� ������� � ��������� ���������� possibleVariables (��� ����������� ���������) � possibleFormulas
//  (��� ����������� "�" ��� "���"). ���� �������� �������� ������ ��� (��, ��� ��������, �� ������ ������� ��� ���������) ������������ nullptr
//
Formula * Formula::operationCandidate(vector <EntityBase *> * entities, EntityBaseType operation)
{
	Formula * result = nullptr;
	bool toProccess = true;
	switch ( operation )
	{
	case NEGATIVE_OPERATION:
		if( ( (type != NEGATIVE_OPERATION) || this->isRedefined() ) && ( freeVariables.size() > 0 ) )
		{
			for( size_t i = 0; i < entities->size(); i++)
			{
				EntityBase * base = entities->at(i);
				if ( !base->isFormula() )
					continue;
				Formula * curr = (Formula *)base;
				if ( curr->inChildOf(this) && curr->isNegative() )
				{
					toProccess = false;
					break;
				}
			}
			if(toProccess)
			{
				result = this;
			}
		}
		break;
	case QUANTIFIER1_OPERATION:
	case QUANTIFIER2_OPERATION:
		if ( freeVariables.size() > 0 )
		{
			// ���� � ������� ���� ��������� ����������, �� �������� ��, �� ������� ��� ��� ���� ����� �������
			vector <EntityVariable *> unset;
			if(!this->isRedefined())
// ����� ���� �� ����� ������� �������������� �����������
#ifdef EQUITYTEST

			for( size_t i = 0; i < entities->size(); i++) // ������� �� ���� ��������, ����� ����� ������ �������, ��� ������� ��������.
			{
				EntityBase * base = entities->at(i);
				if ( !base->isFormula() )
					continue;
				Formula * curr = (Formula *)base;
//				if ( curr->inChildOf(this) && (curr->getType() == operation ) )
				for ( size_t j = 0; j < curr->getNumberOfParents(); j++) // ������� �� ������������ ������ ParentLink ��� ��������� �������
				{
					ParentLink * ppl = curr->getParentLink(j);
					// ���� ������������ ������� ��� ��������� ������� � �������� - ����������� ��������
					if ( ( get<0>(*ppl) == this ) && (curr->getType() == operation ) ) 
					{
						unset.push_back(get<1>(*ppl)); // �� ��������� � unset ����������, �� ������� �������� �������
					}
				}
			}
#else
			for ( size_t j = 0; j < freeVariables.size(); j++) // ������� �� ��������� ����������
			{
				EntityVariable * pev = freeVariables.at(j);
				Formula * pf = quantifierFormula(operation, pev);
				if( pf != nullptr)
				{
					for( size_t i = 0; i < entities->size(); i++) // ������� �� ���� ��������, ����� ����� ������ �������, ��� ������� ��������.
					{
						EntityBase * pbase = entities->at(i);
						if ( !pbase->isFormula() )
							continue;
						Formula * pformula = (Formula *)pbase;
						if( *pformula == *pf )    // ���� ������� ������� ��������� ���� � ����� ������������ � �����
						{
							unset.push_back(pev); // �� ��������� � unset ����������, �� ������� �������� �������
							break;
						}
					}
					delete pf;
				}
			}
#endif

			unsetFreeVariables(&unset);
			if( possibleVariables.size() > 0 ) // ���� ����� �������������� ��� ������� �������� ��������� ���������� ������ ����� �� �����
			{
				result = this; // ������ ���������� ��������� ��� ����������� ��������
			}
			unset.clear();
		}
		break;
	case AND_OPERATION:
	case OR_OPERATION:
		possibleFormulas.clear();
		for ( size_t i = 0; i < entities->size(); i++)
		{
			EntityBase * base = entities->at(i);
			if ( !base->isFormula() )
				continue;
			Formula * f = (Formula *)base;
			if( f != this )
			{
				bool found = false;
				if(!this->isRedefined())
				{
				for ( size_t j = 0; j < entities->size(); j++ )
				{
					EntityBase * base1 = entities->at(j);
					if ( !base1->isFormula() )
						continue;
					Formula * f1 = (Formula *)base1;
					if ( f1->type == operation )
					{
						if ( f1->inChildOf(f) && f1->inChildOf(this) )
						{
							found = true;
							break;
						}
					}
				}
				}
				if ( !found )
					possibleFormulas.push_back(f);
			}
		}
		if ( possibleFormulas.size() > 0 )
			result = this;
		break;
	default:
		break;
	}

	return result;
}


/*
������ ������������, ����� �� ������������.. ��� ����?
void Formula::SetLabel()
{
//	type = FORMULA_SET;
}
*/

//
// ������ ����������� �������
//
void Formula::SetLabel(vector <EntityBase *> * entities)
{
	int count = 0;
	for( size_t i = 0; i < entities->size(); i++ )
	{
		EntityBase * base = entities->at(i);
		if ( base->isFormula() )
		{
			if(this == base)
				break;
			Formula * f = (Formula *)base;
			if( !f->isAtom() || ( i == 0 ) )
				count++;
		}
	}
	label = _T(" P");
	tstring tstr = to_tstring((long long)count);
	for( size_t i = 0; i < tstr.length(); i++)
	{
		label += _T("<Symbol=/low>");
		label += tstr[i];
	}
	label += _T(" ( ");
	for ( size_t i = 0; i < freeVariables.size(); i++ )
	{
		if ( i > 0 )
			label += _T(" , ");
		label += *(freeVariables.at(i)->GetText());
	}
	label += _T(" ) ");
}

//
//  ����������� �� ���������
//
FormulaSet::FormulaSet(void)
{
	type = FORMULA_SET;
}

//
// ����������
//
FormulaSet::~FormulaSet(void)
{
/*
	for(size_t i = 0; i < getNumberOfParents(); i++)
	{
		tstring * ptstr = get<1>(*(getParentLink(i)));
		if ( ptstr != nullptr )
			delete ptstr;
		delete getParentLink(i);
	}
*/
	// ����������� ������� ����������
	for ( size_t i = 0; i < baseVariables.size(); i++)
		delete(baseVariables.at(i));
	baseVariables.clear();

}

//
// ����������� ��� ���������� ��������� ���������� �� ��������� �� ������������ ������� � ��������� ������������ ��������� ����������
//
FormulaSet::FormulaSet(Formula * parent, vector <EntityVariable *> * variables)
{
	type = FORMULA_SET;
	vector <EntityVariable *> *fv = parent->getFreeVariables();
	for ( size_t i = 0; i < fv->size(); i++)
	{
		EntityVariable * pev = new EntityVariable(*(fv->at(i)));
		freeVariables.push_back(pev);
	}
	for ( size_t i = 0; i < variables->size(); i++)
	{
		EntityVariable * pev = new EntityVariable(*(variables->at(i)));
		baseVariables.push_back(pev);
//		ParentLink * pl = new ParentLink(parent, ptstr);
	}
	formula = parent;
	setText();
}


//
//  ����� ��������� ����� ��������� ����������
//
void FormulaSet::setText()
{
	text = _T(" { ");  // ����� �������� ������
//	for ( size_t i = 0; i < getNumberOfParents() ; i++ )
	size_t size = baseVariables.size();
	if ( size > 1) // ���� ���������� ���� �� ���� ������� ����������, ���������� �������
		text += _T("<Symbol=/left_tag> "); // ����������� ������ ����
	// ���� �� ������� ����������
	for ( size_t i = 0; i < size ; i++ )
	{
		if (i > 0) // ���� ��� ������ ������� ����������
			text += _T(" , "); // �������� ������� ����� �����������, ���������� ���������
		text += *baseVariables.at(i)->GetText(); // ��������� ���������� ����� ��������� ������� ����������
	}
	if ( size > 1) // ���� ���������� ���� �� ���� ������� ����������, ���������� �������
		text += _T("<Symbol=/right_tag> "); // ����������� ������� ����
	tstring label = *formula->getRedefinedText();
	trimWhitespaces(&label);
	if(label.length() == 0)
	{
		label = *formula->getLabel(); 
		trimWhitespaces(&label);
	}
	text += _T(" | ") + label; // ��������� ����������� � ��������� �� ��� ����������� �������-��������
	text += _T(" }"); // ��������� ������ �������� ������
}

//
//  ����� ������ ��������� ���������� ��� ������� ������� � ���������� ��������� �� ����, � ������ ������������� ��������� ����� ��������� 
//  ���������� nullptr
//
FormulaSet * Formula::makeFormulaSet(vector <EntityBase *> * entities, vector <EntityVariable *> * variables)
{
	FormulaSet * set = nullptr;
	bool found = false;
	for ( size_t i = 0; i < entities->size(); i++ )
	{
		EntityBase * base = entities->at(i);
		if ( !base->isSet() )
			continue;
		FormulaSet * set = (FormulaSet *)base;
		if ( set->getParentFormula() == this )
		{
			found = true;
			break;
		}
	}
	if ( !found )
	{
		set = new FormulaSet(this, variables);
	}
	return set;
}


//
//  ����� ����������, �������� �� ��� ������� ������� ��������� ��������� ����������. � ������ �������� ���������� ��������� �� ������� �������,
// ���� ���������� (��� ���������) - ���������� nullptr
//
Formula * Formula::formulasetCandidate(vector <EntityBase *> * entities)
{
	Formula * set = nullptr;
	bool found = false;
	for ( size_t i = 0; i < entities->size(); i++ )
	{
		EntityBase * base = entities->at(i);
		if ( !base->isSet() )
			continue;
		FormulaSet * set = (FormulaSet *)base;
		if ( set->getParentFormula() == this )
		{
			found = true;
			break;
		}
	}
	if ( !found )
		set = this;
	return set;
}


//
//  ���������� ��� ��������� ���������� ��������� ��� ������ ������ ��������� ���������� - ����������
//
bool FormulaSet::isVariableBased()
{
	bool res = true;
	for(size_t i = 0; i < baseVariables.size(); i++)
	{
		EntityVariable * tev = baseVariables.at(i);
		if(!tev->isVariable() )
		{
			res = false;
			break;
		}
	}
	return res;
}

//
// ���������� ��� ��������� ���������� ��������� ��� ������ ��������� ���������� - ��������
//
bool FormulaSet::isSetBased()
{
	bool res = true;
	for(size_t i = 0; i < baseVariables.size(); i++)
	{
		EntityVariable * tev = baseVariables.at(i);
		if(!tev->isSet() )
		{
			res = false;
			break;
		}
	}
	return res;
}



//
//  ����� ������ ����������� ��� ��������� ����������
//
void FormulaSet::SetLabel(vector <EntityBase *> * entities)
{
	int count = 0;
	bool isMixed = false;
	for (size_t i = 0; i < entities->size(); i++)
	{
		EntityBase * base = entities->at(i);
		if( base->getType() == FORMULA_SET )
		{
			FormulaSet * pfsbase = (FormulaSet *)base;
			if( this->isSetBased() && pfsbase->isSetBased() )
				count++;
			if( this->isVariableBased() && pfsbase->isVariableBased() )
				count++;
			
		}
	}
	if(isSetBased())
		label = _T("R");
	else if(isVariableBased())
		label = _T("M");
	else
		isMixed = true;
	if(!isMixed)
	{
		tstring tstr = to_tstring((long long)count);
		for( size_t i = 0; i < tstr.length(); i++)
		{
			label += _T("<Symbol=/low>");
			label += tstr[i];
		}
		label += _T(" ( ");
		int count1 = 0;
		for ( size_t i = 0; i < freeVariables.size(); i++ )
		{
			EntityVariable * pfv = freeVariables.at(i);
			bool isFound = false;
			for( size_t i1 = 0; i1 < baseVariables.size(); i1++)
			{
				if( *pfv == *(baseVariables.at(i1) ) )
				{
					isFound = true;
					break;
				}
			}
			if( !isFound )
			{
				if ( count1 > 0 )
					label += _T(" , ");
				count1++;
				label += *(pfv->GetText());
			}
/*
		if( isSetBased() == pfv->isSet() )
		{
			if ( count1 > 0 )
			{
				label += _T(" , ");
				count1++;
			}
			label += *(pfv->GetText());
		}
*/
		}
		label += _T(" ) ");
	}
	else
		label = text;
}




/*
//
//	���������� ����������� ��� ���������� Formula 
//
Formula::Formula(const Formula& fi)
{
	label = fi.Label;
	description = fi.Description;
	addons = fi.Addons;
	text = fi.Text;
	feature = fi.Feature;
	type = fi.EntityType;
	for ( int i = 0; i < fi.freeVariables.size(); i++)
	{
		EntityVariable * pv = new EntityVariable(*(fi.FreeVariables[i]));
		freeVariables.push_back(pv);
	}
}


//
//	���������� ����������� ��� ���������� FormulaSet
//
FormulaSet::FormulaSet(const FormulaSet& si)
{
	label = si.Label;
	description = si.Description;
	addons = si.Addons;
	text = si.Text;
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
*/

void EntityBase::SetText(LPCTSTR lpctstr) 
{	
	tstring tstr = lpctstr; 
	trimWhitespaces(&tstr);
//	removeDoubledWhitespaces(&tstr);
	removeAllWhitespaces(&tstr);
	tstring text1;
		text1 = text;
	trimWhitespaces(&text1);
//	removeDoubledWhitespaces(&text1);
	removeAllWhitespaces(&text1);
	if(text1.compare(tstr))
		redefined_text = lpctstr; 
};

int Formula::getNumberOfParents() 
{ 
	if(isRedefined()) 
		return 0; 
	else 
		return parents.size();
}