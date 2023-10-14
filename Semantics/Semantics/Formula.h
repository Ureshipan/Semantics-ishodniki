//  Автор кода: Татаринцев В.В., akizelokro@mail.ru , 2013-2014
//  Author: Tatarintsev V.V., akizelokro@mail.ru, 2013-2014
#pragma once

#include "common_functions.h"
//#include <stdint.h>



// перечисление типов сущностей
enum EntityBaseType { 
	NONE_FORMULA, // неопределенный
	ATOMIC_FORMULA = 1, // атомарная формула
	NEGATIVE_OPERATION = 2, // формула, полученная применением операции отрицания
	QUANTIFIER1_OPERATION = 3, // формула, полученная применением квантора принадлежности
	QUANTIFIER2_OPERATION = 4,
	AND_OPERATION = 5,  // формула, полученная применением операции "И"
	OR_OPERATION = 6,  // формула, полученная применением операции "ИЛИ"
	FORMULA_SET = 7,  // множество истинности формулы
};

// перечисление типов формул
enum Formula_Feature {
	NONE_FEATURE,  // неопределено
	TRUTH_FEATURE = 1, // истинность
	FALSE_FEATURE = 2, // ложность
	PERFORMED_FEATURE = 3  // выполнимость
};

// перечисление видов переменных по признаку "переменная или множество"
enum VariableType {
	NONE_VARIABLE,
	VARIABLE = 1, // переменная
	SET = 2 //множество
};


#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
// это определение используется в случае использования библиотеки libxml2 для сохранения/загрузки схемы в/из xml-файла
// работа по применению этой библиотеки не была завершнена
#else
// библиотека libxml не используется
// объявление классов, используемых для сериализации
//class CParentLinkItem;
class CVariableItem;  // предварительное объявление класса для сериализации сущностей-переменных
class CFormulaItem;  // предварительное объявление класса для сериализации формул, представляемых классом Formula
class CSetItem;    // предварительное объявление класса для сериализации множеств истинности, представляемых классом FormulaSet
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// класс EntityVariable, представляющий сущности-переменные (как переменные, так и множества
//
class EntityVariable
{
	VariableType type; // чистая переменная или переменная-множество
	tstring label;  // строка-обозначение переменной (не включая индекс)
	tstring index;  // текст индекса, пустая строка при отсутствии индекса
	tstring text;  // текст переменной (уже с индексом), конвертируется в используемый формат (с тэгами)
public:
	// конструктор по умолчанию
	EntityVariable() { type = VARIABLE; label = _T("x"); index = _T(""); SetText(); };
	// деструктор
	~EntityVariable() {};
	// конструктор, принимающий обозначение переменной, индекса и тип (переменная или множество)
	EntityVariable( const tstring & str, const tstring & index_, VariableType type_) { label = str; index = index_; type = type_; SetText(); };
	// конструктор, создающий копию уже существующей переменной
	EntityVariable( const EntityVariable & ev) { type = ev.type; label = ev.label; index = ev.index; SetText(); };
#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
	// в случае применения библиотеки libxml2
#else
	// когда библиотека libxml2 не применяется
	//	конструктор для построения Variable и Formula из соотвествущих классов CVariableItem, CFormulaItem для сериализации
	EntityVariable(const CVariableItem& vi);
#endif


	// метод, возвращающий указатель на текст переменной
	tstring * GetText() { return &text; };
	// метод формирует текст переменной в используемом TS-формате 
	void SetText() { text = label; if(index.length() > 0) text += _T("<Symbol=/low>") + index; };
	//   метод сравнивает две переменные, в случае тождественности обозначения, индекса и типа возвращает true, иначе false 
	bool compare(const EntityVariable&);
	// метод возвращет true, если сущность-переменная является множество
	bool isSet() { bool res; (type == SET) ? res=true : res=false; return res; };
	// метод возвращает true, если сущность-переменная является обычной переменной
	bool isVariable() { bool res; (type == VARIABLE) ? res=true : res=false; return res; };
	// возвращает ссылку на строку переменной (без индекса)
	tstring & GetLabel() { return label; };
	// возвращает ссылку на строку индекса
	tstring & GetIndex() { return index; };
	// возвращает тип переменной (обычная переменная или сущность-множество)
	VariableType GetType() { return type; };
	// operator ==
//	bool operator == (const EntityVariable& ev);


//	копирующий конструктор для построения EntityVariable
};

bool operator == (EntityVariable& ev1, EntityVariable& ev2);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// класс ЕntityBase используется для отображения формул согласно dataSubItems, является базовым для классов Formula и Set
//
class EntityBase
{
protected:
	vector <EntityVariable *> freeVariables;  // свободные переменные
	tstring description;  // описание формулы
	tstring addons;  // дополнительное описание
	tstring label; // условное обозначение
	tstring text; // тест , записывается при создании, выделен в отдельную переменную, чтобы не формировать каждый раз 
	EntityBaseType type; // тип cущности, одно из значений enum EntityBaseType::

	tstring redefined_text; // переопределенный текст формулы, значение заполняется при переопределении
	bool redefined; // флаг для определения, скрывать ли историю

public:
	// конструктор по умолчанию
	EntityBase(void);
	// деструктор
	~EntityBase(void);

	// возвращает указатель на контейнер свобоных переменных для этой сущности
	vector <EntityVariable *> * getFreeVariables() { return &freeVariables; };
	// возвращает текст сущности
	tstring & getText() { if(redefined_text.length() > 0) return redefined_text; else return text; };
	// возвращает указатель на описание сущности
	tstring * getDescription() { return &description; };
	// возвращает указатель на дополнительное описание 
	tstring * getAddOns() { return &addons; };
	// изменяет текст
	void SetText(LPCTSTR lpctstr);	
//	void SetText(LPCTSTR lpctstr) {	tstring tstr = lpctstr; if(text.compare(tstr)) redefined_text = tstr; };
	// изменяет пер текст
//	void SetIRedefinedText(LPCTSTR lpctstr) { redefined_text = lpctstr; };
	// изменяет описание
	void SetDescription(LPCTSTR lpctstr) { description = lpctstr; };
	// изменяет дополнительное описание
	void SetAddons(LPCTSTR lpctstr) { addons = lpctstr; };
	// возвращает указатель на обозначение
	tstring * getLabel() {return &label; };
	// возвращает указатель на обозначение
//	tstring * getInitialText() { if(redefined_text.length() > 0) return &text; else return &redefined_text; };
	tstring * getInitialText() { return &text; };
	// возвращает указатель на обозначение
	tstring * getRedefinedText() { return &redefined_text; };
	// возвращает тип сущности
//	EntityBaseType getRealType() { if((type != FORMULA_SET) && isRedefined()) return ATOMIC_FORMULA; else return type; };
	EntityBaseType getType() { if( isRedefined()) return ATOMIC_FORMULA; else return type; };
	// назначает тип сущности
	void setType( EntityBaseType type_) { type = type_; };
	// проверка, является ли сущность формулой
	bool isFormula() { return ( type != FORMULA_SET ); };
	// проверка, является ли сущность множеством истинности
	bool isSet() { if( type == FORMULA_SET ) return true; else return false; };
	bool isRedefined() { return redefined; };
	void setRedefined() { redefined = true; };
	void setTemporarilyUnredefined() {redefined = false;};


};


class Formula;  // предварительное объявление класса формул
class FormulaSet;  // предварительное объявление класса множеств истинности

typedef tuple<Formula*, EntityVariable *> ParentLink; // пара, указатель на родительский экземпляр класса Formula и указатель на свободную переменную в нём
// служит для занесения информации, откуда был построен текущий экземпляр класса Formula, если указатель на свободную переменную равен nullptr,
// то свободные переменные не использовались при построении текущей формулы, если же были использованы все возможные, то для каждой использованной 
// строится своя пара значений tuple



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  class Formula, представляющий формулы
//
class Formula: public EntityBase
{
//	tstring text; // тест формулы, записывается при создании, выделен в отдельную переменную, чтобы не формировать каждый раз 
	Formula_Feature feature;  // выполнимость, ложность или истинносто формулы
	tstring quantifierPrefix;       // добавка-префикс для кванторов, при слиянии кванторов используется для ПНФ
//	Formula(const Formula &); // конструктор копирования


public:
	vector <ParentLink *> parents;  // контейнер указателей на экзмепляры родителей, на основе которых сформирована формула
	vector <EntityVariable *> possibleVariables; // контейнер указателей на возможные для операции свободных переменных, практически временное значение
	vector <Formula*> possibleFormulas; // контейнер указателей на возможные формулы для бинарной операции объединения "и" или "или"

public:
	// конструктор по умолчанию
	Formula(void);
	// деструктор
	~Formula(void);
#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
#else
	// если не используется библиотека libxml2
	// конструктор построения объекта Formula из объекта, загружаемого из xml-файла
	Formula(const CFormulaItem& );
#endif

	// убирает переменные из числа свободных в экземпляре класса согласно списку
	void unsetFreeVariables(vector <EntityVariable *> * unset); 

//  методы для работы с private и protected данными класса
	Formula_Feature getFeature() { return feature; };
	// возвращает префикс, если операция построена путём навешивания любого из двух квантора 
	tstring * getQuantifierPrefix() { return &quantifierPrefix; };


// функции - методы для создания новых формул


	//	метод используется для применения всех возможных операций объединения типа type к списку формул
	friend void joinAllFormulas(vector <EntityBase *> * entities, EntityBaseType type);
	//	применяет операцию объединения типа type к текущей формуле, если применить операцию возможно, создаёт новую формулу,
	//	вносит её в список формул entities и возвращает указатель на новую формулу, иначе NULL
	friend Formula * joinFormula(vector <EntityBase *> * entities, EntityBaseType type, Formula * first, Formula * second);
	//  навешивает квантор типа type к текущей формуле по всем свободным переменным поочерёдно
	void quantifierFormula(vector <EntityBase *> * entities, EntityBaseType type);
	//	метод создаёт атомарную формулу из двух строк, предположительно первая строка - переменная, а вторая - множество
	void setAtomFormula(const EntityVariable & ev1, const EntityVariable & ev2);
	//	метод создаёт атомарную формулу из двух строк, предположительно первая строка - переменная, а вторая - множество, и добавляет её в схему
	//  возвращает true, если формула или равносильная ей не встречается в схеме и тогда формула добавлена в схему, иначе false
	bool setAtomFormula(vector <EntityBase *> * entities, const EntityVariable & ev1, const EntityVariable & ev2);
	//  применяет операцию квантора типа type к текущей формуле по свободной переменной freeVariable, если применить операцию возможно, создаёт новую формулу,
	//  вносит её в список формул entities и возвращает на неё указатель, иначе NULL
	Formula* quantifierFormula(vector <EntityBase *> * entities, EntityBaseType type, EntityVariable * freeVariable);
	// создаёт из текущей формулы новую навешиванием квантора, без добавления и проверок в схеме
	Formula* quantifierFormula(EntityBaseType type, EntityVariable * freeVariable);
	//  метод используется для применения операций объединения типа type попарно к формулам из списку entities
	friend void quantifierAllFormulas(vector <EntityBase *> * entities, EntityBaseType type);
	//	применение операции отрицания ко всем формулам из указанного списка.
	friend void negativeAllFormulas(vector <EntityBase *> * entities);
	// создание формулы, используя операцию отрицания применительно к текущей формуле c полной проверкой
	Formula*  negativeFormula(vector <EntityBase *> * entities);
	// создание формулы, используя операцию отрицания применительно к текущей формуле с полной проверкой при i равном нулю
	Formula*  negativeFormula(vector <EntityBase *> * entities, int i);

// функции - вспомогательные функции, реализующие логику

	// определяет, является ли формула атомарной
	bool isAtom() { return (type == ATOMIC_FORMULA); };
	// определяет, построена ли формула применением операции отрицания
//	bool isNegative() { return (type == NEGATIVE_OPERATION); };
	bool isNegative() { if( isRedefined()) return false; else return (type == NEGATIVE_OPERATION); };
	// определяет, построена ли формула навешиванием квантора любого из двух типов
//	bool isQuantifier() { if((type == QUANTIFIER1_OPERATION) || (type == QUANTIFIER2_OPERATION)) return true; return false; };
	bool isQuantifier() { if( isRedefined()) return false; else if((type == QUANTIFIER1_OPERATION) || (type == QUANTIFIER2_OPERATION)) return true; return false; };
	// определяет, построена ли формула применением операции либо "И" либо "ИЛИ"
//	bool isUnion() { if((type == AND_OPERATION) || (type == OR_OPERATION)) return true; return false; };
	bool isUnion() { if( isRedefined()) return false; else if((type == AND_OPERATION) || (type == OR_OPERATION)) return true; return false; };

//	friend void proccessOneDetail(vector <Formula *> * formulas, tstring * formulaText, Formula * formula);

	// вспомогательный метод, используемый при построении формул разных типов
	friend void proccessOneDetail(vector <EntityBase *> * entities, tstring * formulaText, ParentLink * parent);
	//	функция формирует текст формулы и помещает в EntityBase::text
	void putTextOfFormula(vector <EntityBase *> * entities);
	//  метод в зависимости от значения типа возможной операции operation определяет, есть ли возможные для создания варианты на основе текущей формулы,
	//  если есть, то возвращает указатель на текущую формулу и заполняет контейнеры possibleVariables (для навешивания кванторов) и possibleFormulas
	//  (для объединения "и" или "или"). Если вакатных операций больше нет (всё, что возможно, на основе формулы уже построено) возвращается nullptr
	Formula * operationCandidate(vector <EntityBase *> * entities, EntityBaseType operation);
	// сейчас просматривал, нигде не используется.. шоб було?
	void SetLabel();
	// строим обозначение формулы
	void SetLabel(vector <EntityBase *> * entities);
	//  метод строит множество истинности для текущей формулы и возвращает указатель на него, в случае невозможности построить такое множество 
	//  возвращает nullptr
	FormulaSet * makeFormulaSet(vector <EntityBase *> * entities, vector <EntityVariable *> * variables);
	//	метод определяет,  является ли текущий класс потомком другого экземпляра класса Formula и свободной переменной freeVariable
	//  в случае если freeVariable равна nullptr означает, что поиск только по формуле
	bool inChildOf(Formula * formula, EntityVariable * freeVariable = nullptr);
	//  метод определяет, возможно ли для текущей формулы построить множество истинности. В случае возможно возвращает указатель на текущую формулу,
	// если невозможно (уже построена) - возвращает nullptr
	Formula * formulasetCandidate(vector <EntityBase *> * entities);
	//	копирующий конструктор для построения Formula 
//	Formula::Formula(const Formula& fi);

	int getNumberOfParents();
	ParentLink * getParentLink(int n) { if(!isRedefined()) return parents.at(n);  else return nullptr; };

//	friend bool operator == (const Formula &);
};



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  class FormulaSet, представляющий множества истинности для формул
//
class FormulaSet : public EntityBase
{
	// указатель на формулу-родителя
	Formula * formula;
	// контейнер базовых, по которым строится множество истинности, свободных переменных
	vector <EntityVariable * > baseVariables;
//	vector <ParentLink *> parents;  // контейнер указателей на экзмепляры родителей и свободные переменные, на основе которых сформирована множество


public:
	// конструктор по умолчанию
	FormulaSet(void);
	// деструктор
	~FormulaSet(void);
	// конструктор для построения множества истинности по указателю на родительскую формулу и контейнер используемых свободных переменных (базовых)
	FormulaSet(Formula * parent, vector <EntityVariable *> * variables);
#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
#else
	// если не используется библиотека libxml2
	// конструктор построения объекта Formula из объекта, загружаемого из xml-файла
	FormulaSet(const CSetItem& );  
//	копирующий конструктор для построения FormulaSet
//	FormulaSet::FormulaSet(const FormulaSet& si);
#endif


	//  метод формирует текст множества истинности
	void setText();
	// возвращает указатель на обьект, представляющий формулу-родителя
	Formula * getParentFormula() { return formula; };
	// устанавливает формулу-родителя
	void setParentFormula(Formula * parent) { formula = parent; };
	//  метод строит обозначение для множества истинности
	void SetLabel(vector <EntityBase *> * entities);
	//  показывает что множество истинности построено при помощи только свободных переменных - немножеств
	bool isVariableBased();  
	// показывает что множество истинности построено при помощи свободных переменных - множеств
	bool isSetBased();
	// возвращает указатель на контейнер базовых, по которым строится множество истинности, свободных переменных
	vector <EntityVariable *> * getBaseVariables() { return &baseVariables; };
//	bool isMixed(); // показывает что множество истинности построено при помощи смешанного состава переменных: переменных и множеств
};
