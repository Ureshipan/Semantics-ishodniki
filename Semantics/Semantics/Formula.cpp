//  Автор кода: Татаринцев В.В., akizelokro@mail.ru , 2013-2014
//  Author: Tatarintsev V.V., akizelokro@mail.ru, 2013-2014
#include "StdAfx.h"
#include "Formula.h"
#include "Operation.h"


//
//   функция сравнивает две переменные, в случае тождественности обозначения, индекса и типа возвращает true, иначе false 
// 
bool EntityVariable::compare(const EntityVariable& ev)
{
	bool result = false;
	if((this->label == ev.label ) && ( this->index == ev.index ) && ( this->type == ev.type ))   // если совпадают обозначения переменной, индексы и тип переменной-сущности
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
//  конструктор по умолчания для класса EntityBase
//
EntityBase::EntityBase(void)
{
	redefined = false;
}

//
//  деструктор класса EntityBase
//
EntityBase::~EntityBase(void)
{
	// удаляю свободные переменные
	for(size_t i = 0; i < freeVariables.size(); i++)
		delete freeVariables.at(i);
}


//
//  конструктор по умолчанию
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
//  деструктор
//
Formula::~Formula(void)
{
	// очищает контейнер указателей на экзмепляры родителей
	for(size_t i = 0; i < getNumberOfParents(); i++)
	{
		EntityVariable * ptstr = get<1>(*(getParentLink(i)));
		if ( ptstr != nullptr )    // если переменная указана в родительской связи
			delete ptstr;   // удаление переменной, указанной в родительской связи
		delete getParentLink(i);   // удаление родительской связи
	}
	parents.clear();
	// очищает контейнер указателей на возможные для операции свободных переменных
	for ( size_t i = 0; i < possibleVariables.size(); i++)
		delete possibleVariables.at(i);    // удаление очередной переменной из контейнера возможных переменных для построения формул
	possibleVariables.clear();
}

//
// убирает переменные из числа свободных в экземпляре класса согласно списку
//
void Formula::unsetFreeVariables(vector <EntityVariable *> * unset)
{
	// очищает контейнер указателей на возможные для операции свободных переменных
	for ( size_t i = 0; i < possibleVariables.size(); i++)
		delete possibleVariables.at(i);    // удаление очередной переменной из контейнера возможных переменных для построения формул
	possibleVariables.clear();
	// цикл по свободным переменным формулы, добавляем их все в контейнер возможных
	for ( size_t i = 0; i < freeVariables.size(); i++)
	{
		EntityVariable * pev = new EntityVariable(*(freeVariables.at(i)));
		possibleVariables.push_back(pev);    // очередную свободную переменную добавляемв контейнер возможных переменных
	}
	// перебор по контейнеру переменных "к удалению из списка возможных"
	for ( size_t i = 0; i < unset->size(); i++ )
	{
		auto j = possibleVariables.begin();     // первый элемент в контейнере возможных
		bool found = false;
		EntityVariable * pev1 = unset->at(i);    // очередная переменная из списка "к удалению"
		// цикл по возможным переменным
		for ( ; j != possibleVariables.end(); ++j)
		{
//			EntityVariable * pev2 = j;
			EntityVariable * pev2 = *j;
			if ( ( pev1 != nullptr ) && (*pev1 == *pev2) )    // если найдена совпадающая переменная
			{
				found = true;
				break;
			}
		}
		if ( found )
		{
			// удалем переменную из списка возможных
			delete(*j);
			possibleVariables.erase(j);
		}
	}
}

//
//	метод создаёт атомарную формулу из двух строк, предположительно первая строка - переменная, а вторая - множество, и добавляет её в схему
//
bool Formula::setAtomFormula(vector <EntityBase *> * entities, const EntityVariable & ev1, const EntityVariable & ev2)
{
	feature = PERFORMED_FEATURE;    // "выполнима"
	type = ATOMIC_FORMULA;    // назначаем тип - атомарная формула
	// добавляем две сущности-переменные, первая - простая переменная, вторая - множество
	EntityVariable * pev1 = new EntityVariable(ev1); 
	freeVariables.push_back(pev1);
	EntityVariable * pev2 = new EntityVariable(ev2); 
	freeVariables.push_back(pev2);
	putTextOfFormula(entities);    // строим текст формулы
	// строим обозначение для атомарной формулы
	label = _T(" P<Symbol=/low>0 ( ");    // пишем "P" с нижним индексом "0", у всех атомарных формул такое обозначение, и левую круглую скобку
	// цикл по свободным переменным формулы
	for ( size_t i = 0; i < freeVariables.size(); i++ )
	{
		if ( i > 0 )    // если вторая или больше по счёту переменная
			label += _T(" , ");    // добавляем запятую с окаймляющими пробелами
		label += *(freeVariables.at(i)->GetText());    // добавляем текст очедной переменной
	}
	label += _T(" ) ");    // закрываем обозначение правой круглой скобкой
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
		entities->push_back(this);    // добавляем текущую формулу в контейнер сущностей схемы entities
	return !found;
}


//
//	метод создаёт атомарную формулу из двух строк, предположительно первая строка - переменная, а вторая - множество
//
void Formula::setAtomFormula(const EntityVariable & ev1, const EntityVariable & ev2)
{
	feature = PERFORMED_FEATURE;    // "выполнима"
	type = ATOMIC_FORMULA;    // назначаем тип - атомарная формула
	// добавляем две сущности-переменные, первая - простая переменная, вторая - множество
	EntityVariable * pev1 = new EntityVariable(ev1); 
	freeVariables.push_back(pev1);
	EntityVariable * pev2 = new EntityVariable(ev2); 
	freeVariables.push_back(pev2);
//	putTextOfFormula(entities);    // строим текст формулы
	// строим обозначение для атомарной формулы
	label = _T(" P<Symbol=/low>0 ( ");    // пишем "P" с нижним индексом "0", у всех атомарных формул такое обозначение, и левую круглую скобку
	// цикл по свободным переменным формулы
	for ( size_t i = 0; i < freeVariables.size(); i++ )
	{
		if ( i > 0 )    // если вторая или больше по счёту переменная
			label += _T(" , ");    // добавляем запятую с окаймляющими пробелами
		label += *(freeVariables.at(i)->GetText());    // добавляем текст очедной переменной
	}
	label += _T(" ) ");    // закрываем обозначение правой круглой скобкой
//	entities->push_back(this);    // добавляем текущую формулу в контейнер сущностей схемы entities
}


//
//	применяет операцию объединения типа type к текущей формуле, если применить операцию возможно, создаёт новую формулу,
//	вносит её в список формул entities и возвращает указатель на новую формулу, иначе NULL
//
Formula * joinFormula(vector <EntityBase *> * entities, EntityBaseType type, Formula * first, Formula * second)
{
	Formula * formula = NULL;
	int k = entities->size();
	int t, tt, f, ff;
	bool check, check2;
	if ( ( type == AND_OPERATION ) || ( type == OR_OPERATION ) )
	{
		// проверка, существует ли уже заданный элемент с такой операцией и такими родительскими формулами
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
		if (check)	// если подобного элемента нет, работаем
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
			// добавляем свободные переменные из родительских формул
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
//	метод используется для применения операций объединения типа type попарно к формулам из списку entities
//
void joinAllFormulas(vector <EntityBase *> * entities, EntityBaseType type)
{
	int k = entities->size();
	int i, j;
	// два связанных цикла, один пробегает от 0 до (всего формул - 2), второй всегда на единицу больше первого, но не больше (всего формул - 1)
	// разыскиваются пары формул (не множеств истинности) и для них предпринимается попытка построить формулу путём объединения типа type
	// если построение возможно, то в newFormula будет ненулевой указатель, тогда полученная формула заносится в entities
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
//  применяет операцию квантора типа type к текущей формуле по свободной переменной freeVariable, если применить операцию возможно, создаёт новую формулу,
//  вносит её в список формул entities и возвращает на неё указатель, иначе NULL
//
Formula* Formula::quantifierFormula(vector <EntityBase *> * entities, EntityBaseType type, EntityVariable * freeVariable)
{
	Formula * formula = NULL;
	int l, j;
	if ( ( type == QUANTIFIER1_OPERATION ) || ( type == QUANTIFIER2_OPERATION ) )    // проверка на тип операции, должно быть навешивание квантора
	{
		tstring str = _T(" ");
		Formula * icurr = this;
		j = icurr->freeVariables.size();
		if (j != 0)
		{
			formula = new Formula();    // создаём новую формулу
			for (l = 0; l < j; l++)     // цикл по свободным переменным текущей формулы
			{
				if ( !(*freeVariables.at(l) == *freeVariable) )    // если очередная свободная переменная различается с той, по которой строим формулу
				{
					EntityVariable * newVar = new EntityVariable(*(icurr->freeVariables.at(l)));
					formula->freeVariables.push_back(newVar);    // добавляем её в свободные переменные новой формулы
					continue;    // переход к следующей свободной переменной
				}
				// дальше работаем со свободными переменными, совпадающими с той, по которой строится новая формула
				// добавляем обозначение квантора в строку
				if ( type == QUANTIFIER1_OPERATION )
					str += _T("<Symbol=/34>");
				else 
					str += _T("<Symbol=/36>");

				str += _T("(") + *(icurr->freeVariables.at(l)->GetText());    // добавляем левую круглую скобку и обозначение переменной
				str += _T(")");    // Добавляем правую круглую скобку
			}
			// строим родительскую связь для новой формулы
			EntityVariable * pev = new EntityVariable(*freeVariable);    // переменная для связи
			ParentLink * plcurr = new ParentLink(this, pev);    // собственно, сама связь
			entities->push_back(formula);    // добавляем новую формулу в список схемы
			formula->parents.push_back(plcurr);    // добавляем родительскую связь в полученную формулу
			formula->quantifierPrefix = str;    // пишем префикс
			formula->type = type;     // пишем тип сущности
			formula->feature = NONE_FEATURE;    // характеристика - в нуль (никакая)
			formula->putTextOfFormula(entities);     // строим сам текст формулы
			// построим обозначение для этой формулы, полученной навешиванием квантора
			formula->SetLabel(entities);
		}
	}
	return formula;
}

//
// создаёт из текущей формулы новую навешиванием квантора, без добавления и проверок в схеме
//
Formula* Formula::quantifierFormula(EntityBaseType type, EntityVariable * freeVariable)
{
	Formula * formula = nullptr;
	int l, j;
	if ( ( type == QUANTIFIER1_OPERATION ) || ( type == QUANTIFIER2_OPERATION ) )    // проверка на тип операции, должно быть навешивание квантора
	{
		tstring str = _T(" ");
		Formula * icurr = this;
		j = icurr->freeVariables.size();
		if (j != 0)
		{
			formula = new Formula();    // создаём новую формулу
			for (l = 0; l < j; l++)     // цикл по свободным переменным текущей формулы
			{
				if ( !(*freeVariables.at(l) == *freeVariable) )    // если очередная свободная переменная различается с той, по которой строим формулу
				{
					EntityVariable * newVar = new EntityVariable(*(icurr->freeVariables.at(l)));
					formula->freeVariables.push_back(newVar);    // добавляем её в свободные переменные новой формулы
					continue;    // переход к следующей свободной переменной
				}
				// дальше работаем со свободными переменными, совпадающими с той, по которой строится новая формула
				// добавляем обозначение квантора в строку
				if ( type == QUANTIFIER1_OPERATION )
					str += _T("<Symbol=/34>");
				else 
					str += _T("<Symbol=/36>");

				str += _T("(") + *(icurr->freeVariables.at(l)->GetText());    // добавляем левую круглую скобку и обозначение переменной
				str += _T(")");    // Добавляем правую круглую скобку
			}
			// строим родительскую связь для новой формулы
			EntityVariable * pev = new EntityVariable(*freeVariable);    // переменная для связи
			ParentLink * plcurr = new ParentLink(this, pev);    // собственно, сама связь
//			entities->push_back(formula);    // добавляем новую формулу в список схемы
			formula->parents.push_back(plcurr);    // добавляем родительскую связь в полученную формулу
			formula->quantifierPrefix = str;    // пишем префикс
			formula->type = type;     // пишем тип сущности
			formula->feature = NONE_FEATURE;    // характеристика - в нуль (никакая)
			// построим обозначение для этой формулы, полученной навешиванием квантора
//    надо будет заменить
//			formula->SetLabel(entities);
		}
	}
	return formula;
}




//
//  навешивает квантор типа type к текущей формуле по всем свободным переменным поочерёдно
//
void Formula::quantifierFormula(vector <EntityBase *> * entities, EntityBaseType type)
{
	Formula * formula = nullptr;
	int l, j;
	if ( ( type == QUANTIFIER1_OPERATION ) || ( type == QUANTIFIER2_OPERATION ) )     // проверка, что тип операции именно навешивание квантора
	{
		tstring str = _T(" ");
		Formula * icurr = this;
		// в цикле по свободным переменным пробуем применить операцию навешивания квантора на очередную свободную переменную
		j = icurr->freeVariables.size();
		if (j != 0)
		{
			for (l = 0; l < j; l++)
			{
				formula = quantifierFormula(entities, type, freeVariables.at(l));	// вызов функции, строящей новую формулу путём навешивания квантора
//				if ( formula != nullptr )
//					formulas->push_back(formula);
			}
		}
	}
//	return formula;
}
//
//  применяет операцию квантора типа type к списку формул
//
void quantifierAllFormulas(vector <EntityBase *> * entities, EntityBaseType type)
{
	tstring str;
	// в цикле ищем формулы, пробуем к формулам применить навешивание квантора по всем свободным переменным поочерёдно
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
// создание формулы, используя операцию отрицания применительно к текущей формуле, при i равном нулю идёт точная проверка
//
Formula* Formula::negativeFormula(vector <EntityBase *> * entities, int i)
{
	Formula * formula = nullptr;
	Formula * icurr = this;
//	if(icurr->type != NEGATIVE_OPERATION)     // проверка, что операция именно операция отрицания
//	{
		formula = new Formula();    // новая формула
		ParentLink * elem = new ParentLink(icurr, nullptr);    // строим родительскую связь
		formula->parents.push_back(elem);    // добавляем для новой формулы
//		formula->parents.push_back(icurr);
		formula->type = NEGATIVE_OPERATION;    // тип новой формулы - опреация отрицания
		formula->feature = NONE_FEATURE;    // предварительно обнуляем характеристику
			   
		// указываем характеристику новой формулы в зависимости от формулы-родителя
		if (icurr->feature == TRUTH_FEATURE)    
			formula->feature = FALSE_FEATURE;
		if (icurr->feature == FALSE_FEATURE) 
			formula->feature = TRUTH_FEATURE;
		if (icurr->feature == PERFORMED_FEATURE) 
			formula->feature = PERFORMED_FEATURE;

		// цикл по свобожным переменным формулы-родителя, дублируем их все в список свободных переменных новой формулы
		for (size_t t = 0; t < icurr->freeVariables.size(); t++)
		{
			EntityVariable * str = new EntityVariable(*icurr->freeVariables.at(t));
			formula->freeVariables.push_back(str);  
		}
		formula->putTextOfFormula(entities);    // строим текст формулы
		formula->SetLabel(entities);    // построим обозначение для этой формулы, полученной применений операции отрицания
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
		entities->push_back(formula);    // Добавляем созданную формулу в список сущностей
	else
	{
		delete formula;
		formula = nullptr;
	}
	return formula;
}

//
//	применение операции отрицания ко всем формулам из указанного списка.
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
//	применение операции отрицания ко всем формулам из указанного списка.
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
//	функция определяет,  является ли текущий класс потомком другого экземпляра класса Formula и свободной переменной freeVariable
//  в случае если freeVariable равна nullptr означает, что поиск только по формуле
//
bool Formula::inChildOf(Formula * formula, EntityVariable * freeVariable)
{
	bool res = false;     
	if(!this->isRedefined())
	{
	for(size_t i = 0; i < getNumberOfParents(); i++)    // перебор всех родительских связей
	{
		ParentLink * plcurr = getParentLink(i);     // очередная родительская связь
		if ( get<0>(*plcurr) == formula )     // если формула в родительской связи совпадает с тестируемой формулой
		{
			if ( freeVariable == nullptr )     // если freeVariable равна nullptr, то определения родительской формулы идёт вне зависимости 
			// от свободных переменных родительской связи, то есть обращается внимание только на формулу
			{
				res = true;    // явялется
				break;
			}
			else    // если проверка идёт и по формуле, и по определённой свободной переменной
			{
				EntityVariable * ptstr = get<1>(*plcurr);    // свободная переменная из очередной родительской связи (nullptr или конкретная переменная
//				if ( ( ptstr != nullptr ) && ( ptstr->compare(*freeVariable) == 0) )
				if ( ( ptstr != nullptr ) && ( *ptstr == *freeVariable ) )    // если свободные переменные совпадают
				{
					res = true;    // является
					break;
				}
			}
		}
	}
	}
	return res;
}

//
//	функция формирует текст формулы и помещает в EntityBase::text
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
//  функция в зависимости от значения типа возможной операции operation определяет, есть ли возможные для создания варианты на основе текущей формулы,
//  если есть, то возвращает указатель на текущую формулу и заполняет контейнеры possibleVariables (для навешивания кванторов) и possibleFormulas
//  (для объединения "и" или "или"). Если вакатных операций больше нет (всё, что возможно, на основе формулы уже построено) возвращается nullptr
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
			// если в формуле есть свободные переменные, то отсеиваю те, по которым уже мог быть взять квантор
			vector <EntityVariable *> unset;
			if(!this->isRedefined())
// здесь пока на время отладки раносильностей комментарим
#ifdef EQUITYTEST

			for( size_t i = 0; i < entities->size(); i++) // перебор по всем формулам, чтобы найти другие формулы, где текущая родитель.
			{
				EntityBase * base = entities->at(i);
				if ( !base->isFormula() )
					continue;
				Formula * curr = (Formula *)base;
//				if ( curr->inChildOf(this) && (curr->getType() == operation ) )
				for ( size_t j = 0; j < curr->getNumberOfParents(); j++) // перебор по родительским связям ParentLink для очередной формулы
				{
					ParentLink * ppl = curr->getParentLink(j);
					// если родительская формула для очередной искомая и операция - навешивание квантора
					if ( ( get<0>(*ppl) == this ) && (curr->getType() == operation ) ) 
					{
						unset.push_back(get<1>(*ppl)); // то добавляем в unset переменную, по которой навесили квантор
					}
				}
			}
#else
			for ( size_t j = 0; j < freeVariables.size(); j++) // перебор по свободным переменным
			{
				EntityVariable * pev = freeVariables.at(j);
				Formula * pf = quantifierFormula(operation, pev);
				if( pf != nullptr)
				{
					for( size_t i = 0; i < entities->size(); i++) // перебор по всем формулам, чтобы найти другие формулы, где текущая родитель.
					{
						EntityBase * pbase = entities->at(i);
						if ( !pbase->isFormula() )
							continue;
						Formula * pformula = (Formula *)pbase;
						if( *pformula == *pf )    // если пробная формула совпадает хоть с одной существующей в схеме
						{
							unset.push_back(pev); // то добавляем в unset переменную, по которой навесили квантор
							break;
						}
					}
					delete pf;
				}
			}
#endif

			unsetFreeVariables(&unset);
			if( possibleVariables.size() > 0 ) // если число использованный для текущий операции свободных переменных меньше всего их числа
			{
				result = this; // создаём возможного кандидата для навешивания квантора
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
сейчас просматривал, нигде не используется.. шоб було?
void Formula::SetLabel()
{
//	type = FORMULA_SET;
}
*/

//
// строим обозначение формулы
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
//  конструктор по умолчанию
//
FormulaSet::FormulaSet(void)
{
	type = FORMULA_SET;
}

//
// деструктор
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
	// освобождает базовые переменные
	for ( size_t i = 0; i < baseVariables.size(); i++)
		delete(baseVariables.at(i));
	baseVariables.clear();

}

//
// конструктор для построения множества истинности по указателю на родительскую формулу и контейнер используемых свободных переменных
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
//  метод формирует текст множества истинности
//
void FormulaSet::setText()
{
	text = _T(" { ");  // левая фигурная скобка
//	for ( size_t i = 0; i < getNumberOfParents() ; i++ )
	size_t size = baseVariables.size();
	if ( size > 1) // если существует хотя бы одна базовая переменная, избыточное условие
		text += _T("<Symbol=/left_tag> "); // обозначение левого тэга
	// цикл по базовым переменным
	for ( size_t i = 0; i < size ; i++ )
	{
		if (i > 0) // если это вторая базовая переменная
			text += _T(" , "); // добаляем запятую между переменными, окаймлённую пробелами
		text += *baseVariables.at(i)->GetText(); // добавляем собственно текст очередной базовой переменной
	}
	if ( size > 1) // если существует хотя бы одна базовая переменная, избыточное условие
		text += _T("<Symbol=/right_tag> "); // обозначение правого тэга
	tstring label = *formula->getRedefinedText();
	trimWhitespaces(&label);
	if(label.length() == 0)
	{
		label = *formula->getLabel(); 
		trimWhitespaces(&label);
	}
	text += _T(" | ") + label; // добавляем разделитель и следующее за ним обозначение формулы-родителя
	text += _T(" }"); // добавляем правую фигурную скобку
}

//
//  метод строит множество истинности для текущей формулы и возвращает указатель на него, в случае невозможности построить такое множество 
//  возвращает nullptr
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
//  метод определяет, возможно ли для текущей формулы построить множество истинности. В случае возможно возвращает указатель на текущую формулу,
// если невозможно (уже построена) - возвращает nullptr
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
//  показывает что множество истинности построено при помощи только свободных переменных - немножеств
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
// показывает что множество истинности построено при помощи свободных переменных - множеств
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
//  метод строит обозначение для множества истинности
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
//	копирующий конструктор для построения Formula 
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
//	копирующий конструктор для построения FormulaSet
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