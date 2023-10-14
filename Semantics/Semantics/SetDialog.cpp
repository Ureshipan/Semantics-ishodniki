//  ����� ����: ���������� �.�., akizelokro@mail.ru , 2013-2014
//  Author: Tatarintsev V.V., akizelokro@mail.ru, 2013-2014
// SetDialog.cpp: ���� ����������
//



#include "stdafx.h"
#include "Semantics.h"
#include "SetDialog.h"
#include "afxdialogex.h"
//#include <cmath>



IMPLEMENT_DYNAMIC(SetDialog, CDialogEx)


//
// �����������
//
SetDialog::SetDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(SetDialog::IDD, pParent)
{
	m_fIndexes = nullptr;
	m_fSymbol = nullptr;
}


//
// ����������
//
SetDialog::~SetDialog()
{
	// ���� ������ ����������� (��������� �� ��� �� ����� nullptr), ���������� �������
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


// ����������� ��������� SetDialog

//
// ���������� ������� �� ������ "OK"
//
void SetDialog::OnBnClickedOk()
{

	int selected1 = m_list.GetCurSel();    // ����� ��������� �������
	if ( selected1 != LB_ERR )     // ���� ����� ���������� ��������, ������������ ������� GetCurSel, ������������
	{
		SemanticString * dsi1 = (SemanticString *) m_list.DCU.at(selected1);    // �������� ��������� �� SemanticString, ��������� � ��������� ���������
		Formula * formula = dsi1->formula;    // ���� ��������� �� ������ Formula ��� �������� ������� SemnticString
		if ( formula != nullptr)    // �������� �� ��, ��� ��������� �� ������� (���������� ��������)
		{
			// Get the indexes of all the selected items.
			int nCount = m_listVariables.GetSelCount();    // ���������, ������� ��������� ���������� �������
			if(nCount > 0)    // ���� ������� ���� �� ����
			{
				vector <EntityVariable *> * variables = new vector <EntityVariable *>;


				CArray<int,int> aryListBoxSel;    // ��������� ��� ��������� ��������� ������ ��������� ����������
				aryListBoxSel.SetSize(nCount);    // ������������� ������ ����������, ������ ����� ��������� ��������� ����������
				m_listVariables.GetSelItems(nCount, aryListBoxSel.GetData());     // ��������� ���������
				int pt;
				// ������� �� ��������� ��������� ����������
				for (int i = 0; i <= aryListBoxSel.GetUpperBound(); i++)
				{
					pt = aryListBoxSel.GetAt(i);    // ������� ����� ���������� ��������, ���������������� ��������� ��������� ��������� ����������
					vector <EntityVariable *> * fv = formula->getFreeVariables(); 
					variables->push_back(fv->at(pt)); // ��������� � ������ ����������, �� ������� ������ ��������� ����������
				}
				// ��������, ��������� �� ������ ����� �������� ����������, �� ������ ��� �� ������� � ��� �� ������� ����������
				bool isExist = false;
				for (size_t i = 0; i < entities->size(); i++) // ������� ���� ��������� ���������
				{
					bool toProccess = false;
					EntityBase * base = entities->at(i);     // ��������� �� ��������� ��������
					if ( base->isFormula() ) // ���� ��������� �������� �������, �� ��������� � ��������� ��������
						continue;
					FormulaSet * baseFS = (FormulaSet *)base;    // ��������� �� ��������� ����������
					if (formula == baseFS->getParentFormula()) // ���� ������������ ������� ��� ��������� �������� ��������� � ��������� ��������, 
					// �� ������� �������� ��������� ����������
					{
						// ��������, ��� ��������� ������ ��������� ���������� ��� ���� ���� ������ ( ��������, ���������� ��������)
						vector <EntityVariable *> * fv_base = base->getFreeVariables();
						vector <EntityVariable *> * fv_formula = formula->getFreeVariables();
						if ( fv_base->size() == fv_formula->size() ) // ������ ���� ������� ������� ��������� ���������� ���������
						{
							bool isFoundFVAll = false;
							for (size_t i1 = 0; i1 < fv_base->size(); i1++)    // ������� �� ���� ��������� ���������� ��������� ��������
							{
								EntityVariable * pev = fv_base->at(i1);    
								bool isFoundFV = false;
								for (size_t i2 = 0; i2 < fv_formula->size(); i2++)    // ��������� ���� �� ��������� ���������� �������
								{
									if ( pev->compare(*fv_formula->at(i2)))     // ���� ��������
									{
										isFoundFV = true;
										break;
									}
								}
								if ( isFoundFV )
								{
									isFoundFVAll = true;    // �� ���� ���������, ��������� ���� ������ ��� ����� ����������� ����������, �� � ��� ��������,
									// ��������, �������� ����������
									break;
								}
							}
							if ( isFoundFVAll )
								toProccess = true;
						}
						// ��������, ��� ��������� ������ ������� ���������� ������� �������� � ��������� ��������, �� ������� �������� ��������� ����������
						vector <EntityVariable *> * bv_base = baseFS->getBaseVariables();
						if ( toProccess && ( bv_base->size() == variables->size() ) ) // ������ ���� ������� ������� ��������� ���������� ���������
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
				if ( !isExist )     // ���� �� ������� ��������� ��������� ����������
				{
					FormulaSet * pfs = new FormulaSet(formula, variables);    // ������ ��������� ����������
					pfs->SetLabel(entities);    // ��������� �����������
					entities->push_back(pfs);   // �������� ��������� ��������� ���������� � �����
				}
				else
					AfxMessageBox(_T("�������� ������ ����� ����������, �� ����� ��������� ���������� ��� ���������"));
				// ����� �����������
				delete variables;     // ���������� ������
				if ( !isExist )       // ���� ���� ������� ��������� ����������
					CDialogEx::OnOK();    // �������� ���������� ����
			}
			else
				AfxMessageBox(_T("�� ������� �� ����� ����������"));
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
				// ����� �����������
				delete variables;
			}
*/
		}
	}
	else
		AfxMessageBox(_T("�� ������� ������� ��� ���������� ��������� ����������"));

	// TODO: �������� ���� ��� ����������� �����������
}


//
// ������������� �������� � �������� ������� � ��� ��������� ��� �������������
//
BOOL SetDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString str;
	int count = 0;
	tstring res;
	tstring res_formula;
	for (size_t i = 0; i < entities->size(); i++)    // ������� �� ���� ��������� �����
	{
		bool toProccess = true;     // ���� 
		EntityBase * base = entities->at(i);    // ��������� �� ��������� ��������
		if ( !base->isFormula() )       // ���� �������� �� �������
			continue;     // ��������� � ������
		vector <EntityVariable *> * fv = base->getFreeVariables();    // ��������� �� ��������� ��������� ���������� ��������
		if ( fv->size() == 0)     // ���� ��� ��������� ����������
			continue;     // ��������� � ��������� ��������
		Formula * icurr = (Formula *)base;    // ��������� �� ������ ������ Formula
/*
���� ���� ������ �������� �� ��������� � ������ ������-���������� ��� ����������� �������� ����������
�� ��� ���� �� ����, ��� � ������ ����������� ��������� � �� ������ ��������
		if ( icurr->formulasetCandidate(entities) == nullptr )
			continue;
*/
		// ��������� � �������� ������ � ���������� �������
		str.Format(_T("%d"), count);
		m_list.AddString(str);
		m_list.SetItemHeight(count, 30);
		SemanticString * dsi = new SemanticString;
		m_list.DCU.push_back(dsi);
		dsi->PrepareSubItemData(icurr->getText());
		dsi->formula = icurr;
		m_list.SetItemData(count, (DWORD_PTR)dsi);   // ������������� ���������� SemanticString � �������� 
	}
	// TODO:  �������� �������������� �������������
/*
	m_fIndexes = new CFont;
	m_fIndexes->CreatePointFont(70, L"Microsoft San Serif");
	m_StaticDescription.m_fIndexes = m_fIndexes;
	m_fSymbol = new CFont;
	m_fSymbol->CreatePointFont(140, L"Symbol");
	m_StaticDescription.m_fSymbol = m_fSymbol;
*/
	// TODO:  �������� �������������� �������������

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����������: �������� ������� OCX ������ ���������� �������� FALSE
}

//
// ����� ���������� ��������� ���������� � �������� m_listVariables ��� ���������� �������� � ������ ������
//
void SetDialog::ShowFreeVariables()
{
	int selected = m_list.GetCurSel();     // ������� ����� ���������� ������� (���������� � �������� ��������)
	if( selected >= 0 )     //���� �������� 
	{
		SemanticString * dsi = m_list.DCU.at(selected);    // ������� ��������� �� SemanticString, ��������� � ��������� ��������
		Formula * f = dsi->formula;     // �� SemanticString ������� ��������� �� ���� �������
		vector <EntityVariable *> * freeVariables = f->getFreeVariables();   // ��������� �� ��������� ���������� �������
		for (size_t i = 0; i < freeVariables->size(); i++)     // ������� �� ��������� ����������
		{
			CString str;
			// ��������� � �������� ������ � ���������� �������
			str.Format(_T("%d"), i);
			m_listVariables.InsertString(i,str);
			m_listVariables.SetItemHeight(i, 30);
			// ������� ������ SemanticString ��� ��������� ��������� ���������� �� ������ �������-������
			SemanticString * dsi1 = new SemanticString;
			m_listVariables.DCU.push_back(dsi1);
			dsi1->PrepareSubItemData(*(freeVariables->at(i)->GetText()));
			m_listVariables.SetItemData(i, (DWORD_PTR)dsi1);     // �������� 
			if(i == 0)     // ���� ������ (�� ���� ���� �� ���� ��������� �������)
			{
				m_buttonOK.EnableWindow(true);    // ����� ��������� ������ "OK"
				m_listVariables.EnableWindow(true);    // ����� ��������� ����
			}
		}
		m_listVariables.SetCurSel(0);    // ������������ ��������� ������ �������
	}
}

//
// ����� ������� ��������� ���������� �� �������� m_listVariables
//
void SetDialog::HideFreeVariables()
{
	int item;
	while( (m_listVariables.GetCount( ) ) > 0 )   // ���� � ������ ��������� ���������� ������� ���� �� ���� �������
	{
		item = m_listVariables.GetTopIndex( );    // �������� ������ �������� ��������
		SemanticString * dsi = (SemanticString *) m_listVariables.GetItemData(item);    // �������� ������
		CString cs;
		m_listVariables.GetText(item, cs);	   // �������� ������, ��������������� �������� ��������, �������, ���� �������� ������
		m_listVariables.DeleteString(item);    //  ������� �� ����-������ ������ ��� �������� � �������� item
		DELETEITEMSTRUCT dis;                  // ��������� ��� �������� ������ �������� 
		dis.itemID = item;                     
		m_listVariables.DeleteItem(&dis);      // ������� ���� ������� ���������
	}
	m_listVariables.DCU.clear();               // ������� ��������� ������������ SemanticString
	m_buttonOK.EnableWindow(false);            // ����� ���� ����������
}

//
// ���������� ������ �������
//
void SetDialog::OnSelchangeSetlist1()
{
	HideFreeVariables();    // ������� ����������, ���� ����, �� ����-������ ��������� ����������
	ShowFreeVariables();    // ��������� ����-������ ��������� ���������� � ����������� �� ��������� �������
}
