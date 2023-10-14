//  Автор кода: Татаринцев В.В., akizelokro@mail.ru , 2013-2014
//  Author: Tatarintsev V.V., akizelokro@mail.ru, 2013-2014
// SetDialog.cpp: файл реализации
//



#include "stdafx.h"
#include "Semantics.h"
#include "SetDialog.h"
#include "afxdialogex.h"
//#include <cmath>



IMPLEMENT_DYNAMIC(SetDialog, CDialogEx)


//
// конструктор
//
SetDialog::SetDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(SetDialog::IDD, pParent)
{
	m_fIndexes = nullptr;
	m_fSymbol = nullptr;
}


//
// деструктор
//
SetDialog::~SetDialog()
{
	// если шрифты установлены (указатели на них не равны nullptr), освободить обьекты
	if ( m_fIndexes != nullptr )
		delete m_fIndexes;
	if ( m_fSymbol != nullptr )
		delete m_fSymbol;
}

void SetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SETLIST1, m_list);
	DDX_Control(pDX, IDC_SETLIST2, m_listVariables);
	DDX_Control(pDX, IDOK, m_buttonOK);
}


BEGIN_MESSAGE_MAP(SetDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &SetDialog::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_SETLIST1, &SetDialog::OnSelchangeSetlist1)
END_MESSAGE_MAP()


// обработчики сообщений SetDialog

//
// обработчик нажатия на кнопку "OK"
//
void SetDialog::OnBnClickedOk()
{

	int selected1 = m_list.GetCurSel();    // номер выбранной формулы
	if ( selected1 != LB_ERR )     // если номер выбранного элемента, возвращённый методом GetCurSel, действителен
	{
		SemanticString * dsi1 = (SemanticString *) m_list.DCU.at(selected1);    // получаем указатель на SemanticString, связанную с выбранным элементом
		Formula * formula = dsi1->formula;    // берём указатель на обьект Formula для текущего объекта SemnticString
		if ( formula != nullptr)    // проверка на то, что указатель не нулевой (избыточная проверка)
		{
			// Get the indexes of all the selected items.
			int nCount = m_listVariables.GetSelCount();    // проверяем, сколько свободных переменных выбрано
			if(nCount > 0)    // если выбрана хотя бы одна
			{
				vector <EntityVariable *> * variables = new vector <EntityVariable *>;


				CArray<int,int> aryListBoxSel;    // контейнер для выбранных элементов списка свободных переменных
				aryListBoxSel.SetSize(nCount);    // устанавливаем размер контейнера, равным числу выбранных свободных переменных
				m_listVariables.GetSelItems(nCount, aryListBoxSel.GetData());     // заполняем контейнер
				int pt;
				// перебор по выбранным свободным переменным
				for (int i = 0; i <= aryListBoxSel.GetUpperBound(); i++)
				{
					pt = aryListBoxSel.GetAt(i);    // получаю номер очередного элемента, соответствующего очередной выбранной свободной переменной
					vector <EntityVariable *> * fv = formula->getFreeVariables(); 
					variables->push_back(fv->at(pt)); // добавляем в вектор переменных, по которым строим множество истинности
				}
				// проверка, построено ли раньше такое множство истинности, на основе той же формулы и тех же наборов переменных
				bool isExist = false;
				for (size_t i = 0; i < entities->size(); i++) // перебор всех созданных сущностей
				{
					bool toProccess = false;
					EntityBase * base = entities->at(i);     // указатель на очередную сущность
					if ( base->isFormula() ) // если очередная сущность формула, то переходим к следующей сущности
						continue;
					FormulaSet * baseFS = (FormulaSet *)base;    // указатель на множество истинности
					if (formula == baseFS->getParentFormula()) // если родительская формула для очередной сущности совпадает с выбранной формулой, 
					// по которой построим множество истинности
					{
						// проверка, что совпадают наборы свободных переменных для этих двух формул ( возможно, избыточная проверка)
						vector <EntityVariable *> * fv_base = base->getFreeVariables();
						vector <EntityVariable *> * fv_formula = formula->getFreeVariables();
						if ( fv_base->size() == fv_formula->size() ) // только если размеры наборов свободных переменных совпадают
						{
							bool isFoundFVAll = false;
							for (size_t i1 = 0; i1 < fv_base->size(); i1++)    // перебор по всем свободным переменным очередной сущности
							{
								EntityVariable * pev = fv_base->at(i1);    
								bool isFoundFV = false;
								for (size_t i2 = 0; i2 < fv_formula->size(); i2++)    // вложенный цикл по свободным переменным формулы
								{
									if ( pev->compare(*fv_formula->at(i2)))     // если сопадают
									{
										isFoundFV = true;
										break;
									}
								}
								if ( isFoundFV )
								{
									isFoundFVAll = true;    // по всей видимости, сработает даже только при одной совпадающей переменной, но и вся проверка,
									// возможно, является избыточной
									break;
								}
							}
							if ( isFoundFVAll )
								toProccess = true;
						}
						// проверка, что совпадают наборы базовых переменных будущей сущности и очередной сущности, по которым строятся множества истинности
						vector <EntityVariable *> * bv_base = baseFS->getBaseVariables();
						if ( toProccess && ( bv_base->size() == variables->size() ) ) // только если размеры наборов свободных переменных совпадают
						{
							toProccess = false;
							bool isFoundFVAll = false;
							for (size_t i1 = 0; i1 < bv_base->size(); i1++)
							{
								EntityVariable * pev = bv_base->at(i1);
								bool isFoundFV = false;
								for (size_t i2 = 0; i2 < variables->size(); i2++)
								{
									if ( pev->compare(*variables->at(i2)))
									{
										isFoundFV = true;
										break;
									}
								}
								if ( isFoundFV )
								{
									isFoundFVAll = true;
									break;
								}
							}
							if ( isFoundFVAll )
								toProccess = true;
						}
						else
							toProccess = false;
					}
					if(toProccess)
					{
						isExist = toProccess;
						break;
					}
				}
				if ( !isExist )     // если не найдено подобного множества истинности
				{
					FormulaSet * pfs = new FormulaSet(formula, variables);    // создаём множество истинности
					pfs->SetLabel(entities);    // формуруем обозначение
					entities->push_back(pfs);   // добавляю созданное множество истинности в схему
				}
				else
					AfxMessageBox(_T("Выберите другой набор переменных, по этому множество истинности уже построено"));
				// пишем обозначение
				delete variables;     // освобождаю память
				if ( !isExist )       // если было создано множество истинности
					CDialogEx::OnOK();    // закрываю диалоговое окно
			}
			else
				AfxMessageBox(_T("Не выбрано ни одной переменной"));
/*
			int selected2 = m_listVariables.GetCurSel();
			if(selected2 != LB_ERR)
			{
//				SemanticString * dsi2 = (SemanticString *) m_listVariables.DCU.at(selected2);
				vector <EntityVariable *> * fv = formula->getFreeVariables();
				vector <EntityVariable *> * variables = new vector <EntityVariable *>;
				variables->push_back(fv->at(selected2));
				FormulaSet * pfs = new FormulaSet(formula, variables);
				pfs->SetLabel(entities);
				entities->push_back(pfs);
				// пишем обозначение
				delete variables;
			}
*/
		}
	}
	else
		AfxMessageBox(_T("Не выбрана формула для построения множества истинности"));

	// TODO: добавьте свой код обработчика уведомлений
}


//
// устанавливает значения и свойства диалога и его элементов при инициализации
//
BOOL SetDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString str;
	int count = 0;
	tstring res;
	tstring res_formula;
	for (size_t i = 0; i < entities->size(); i++)    // перебор по всем сущностям схемы
	{
		bool toProccess = true;     // флаг 
		EntityBase * base = entities->at(i);    // указатель на очередную сущность
		if ( !base->isFormula() )       // если сущность не формула
			continue;     // переходим к другой
		vector <EntityVariable *> * fv = base->getFreeVariables();    // указатель на контейнер свободных переменных сущности
		if ( fv->size() == 0)     // если нет свободных переменных
			continue;     // переходим к следующей сущности
		Formula * icurr = (Formula *)base;    // указатель на объект класса Formula
/*
этот блок раньше исключал из попадания в список формул-кандидатов для составления множеств истинности
но это было до того, как я начала фомрировать множества и на основе кортежей
		if ( icurr->formulasetCandidate(entities) == nullptr )
			continue;
*/
		// вставляем в листбокс строку с порядковым номером
		str.Format(_T("%d"), count);
		m_list.AddString(str);
		m_list.SetItemHeight(count, 30);
		SemanticString * dsi = new SemanticString;
		m_list.DCU.push_back(dsi);
		dsi->PrepareSubItemData(icurr->getText());
		dsi->formula = icurr;
		m_list.SetItemData(count, (DWORD_PTR)dsi);   // устанавливаем полученную SemanticString в качестве 
	}
	// TODO:  Добавить дополнительную инициализацию
/*
	m_fIndexes = new CFont;
	m_fIndexes->CreatePointFont(70, L"Microsoft San Serif");
	m_StaticDescription.m_fIndexes = m_fIndexes;
	m_fSymbol = new CFont;
	m_fSymbol->CreatePointFont(140, L"Symbol");
	m_StaticDescription.m_fSymbol = m_fSymbol;
*/
	// TODO:  Добавить дополнительную инициализацию

	return TRUE;  // return TRUE unless you set the focus to a control
	// Исключение: страница свойств OCX должна возвращать значение FALSE
}

//
// метод отображает свободные переменные в элементе m_listVariables для выбранного элемента в списке формул
//
void SetDialog::ShowFreeVariables()
{
	int selected = m_list.GetCurSel();     // получаю номер выделенной формулы (связанного с формулой элемента)
	if( selected >= 0 )     //если выделена 
	{
		SemanticString * dsi = m_list.DCU.at(selected);    // получаю указатель на SemanticString, связанную с указанной формулой
		Formula * f = dsi->formula;     // из SemanticString получаю указатель на саму формулу
		vector <EntityVariable *> * freeVariables = f->getFreeVariables();   // указатель на свободные переменные формулы
		for (size_t i = 0; i < freeVariables->size(); i++)     // перебор по свободным переменным
		{
			CString str;
			// вставляем в листбокс строку с порядковым номером
			str.Format(_T("%d"), i);
			m_listVariables.InsertString(i,str);
			m_listVariables.SetItemHeight(i, 30);
			// готовлю обьект SemanticString для очередной свободной переменной во второй элемент-список
			SemanticString * dsi1 = new SemanticString;
			m_listVariables.DCU.push_back(dsi1);
			dsi1->PrepareSubItemData(*(freeVariables->at(i)->GetText()));
			m_listVariables.SetItemData(i, (DWORD_PTR)dsi1);     // добавляю 
			if(i == 0)     // если первая (то есть хотя бы одна свободная формула)
			{
				m_buttonOK.EnableWindow(true);    // делаю доступным кнопку "OK"
				m_listVariables.EnableWindow(true);    // делаю доступным окно
			}
		}
		m_listVariables.SetCurSel(0);    // устанавливаю выбранным первый элемент
	}
}

//
// метод удаляет свободные переменные из элемента m_listVariables
//
void SetDialog::HideFreeVariables()
{
	int item;
	while( (m_listVariables.GetCount( ) ) > 0 )   // пока в списке свободных переменных остаётся хотя бы один элемент
	{
		item = m_listVariables.GetTopIndex( );    // получаем индекс верхнего элемента
		SemanticString * dsi = (SemanticString *) m_listVariables.GetItemData(item);    // излишняя строка
		CString cs;
		m_listVariables.GetText(item, cs);	   // получаем строку, соответствующюю верхнему элементу, кажется, тоже излишняя строка
		m_listVariables.DeleteString(item);    //  удаляем из окна-списка строку для элемента с индексом item
		DELETEITEMSTRUCT dis;                  // структура для удаления самого элемента 
		dis.itemID = item;                     
		m_listVariables.DeleteItem(&dis);      // удаляем весь элемент полностью
	}
	m_listVariables.DCU.clear();               // очищаем контейнер отображаемых SemanticString
	m_buttonOK.EnableWindow(false);            // делаю окно неактивным
}

//
// обработчик выбора формулы
//
void SetDialog::OnSelchangeSetlist1()
{
	HideFreeVariables();    // удаляет переменные, если есть, из окна-списка свободных переменных
	ShowFreeVariables();    // заполняет окно-список свободных переменных в зависимости от выбранной формулы
}
