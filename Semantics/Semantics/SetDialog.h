//  јвтор кода: “атаринцев ¬.¬., akizelokro@mail.ru , 2013-2014
//  Author: Tatarintsev V.V., akizelokro@mail.ru, 2013-2014
#pragma once

#include "afxwin.h"
#include "Formula.h"
#include "ScientificListBox.h"
// диалоговое окно SetDialog

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// класс SetDialog, используетс€ дл€ диалогового окна IDD_SETDIALOG, используемого при построении множеств истинности


class SetDialog : public CDialogEx
{
	DECLARE_DYNAMIC(SetDialog)

	vector <EntityBase *> * entities;    // указатель на список сущностей, по которым будет осуществл€тьс€ выбор дл€ построени€ множества истинности
public:
	// конструктор
	SetDialog(CWnd* pParent = NULL);   
	// декструктор
	virtual ~SetDialog();

// ƒанные диалогового окна
	enum { IDD = IDD_SETDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	ScientificListBox m_list;    // окно списка формул
	CFont * m_fIndexes;          // 
	CFont * m_fSymbol;           // указатель на шрифт, используемый дл€ отображени€ математических символов (как правило, Symbol)
	ScientificListBox m_listVariables;    // окно списка свободных переменных
	CButton m_buttonOK;          // кнопка "OK"


	// обработчик нажати€ на кнопку "OK"
	afx_msg void OnBnClickedOk();
	// устанавливает значени€ и свойства диалога и его элементов при инициализации
	virtual BOOL OnInitDialog();
	// устанавливаю указатель на контейнер сущностей, из которых нужно выбрать формулы и свободные переменные, дл€ которых ещЄ не построены
	// множества истинности (тоже приведены в контейнере
	void  setEntities(vector <EntityBase *> * all_entities = NULL) {entities = all_entities; };
	// метод отображает свободные переменные в элементе m_listVariables дл€ выбранного элемента в списке формул
	void ShowFreeVariables();
	// метод удал€ет свободные переменные из элемента m_listVariables 
	void HideFreeVariables();
	// обработчик выбора формулы
	afx_msg void OnSelchangeSetlist1();
};
