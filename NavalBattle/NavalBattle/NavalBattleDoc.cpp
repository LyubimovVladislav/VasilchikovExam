
// NavalBattleDoc.cpp : implementation of the CNavalBattleDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "NavalBattle.h"
#endif

#include "NavalBattleDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNavalBattleDoc

IMPLEMENT_DYNCREATE(CNavalBattleDoc, CDocument)

BEGIN_MESSAGE_MAP(CNavalBattleDoc, CDocument)
	ON_COMMAND(ID_EDIT_CLEAR_ALL, &CNavalBattleDoc::OnEditClearAll)
END_MESSAGE_MAP()


// CNavalBattleDoc construction/destruction

CNavalBattleDoc::CNavalBattleDoc() noexcept
{
	// TODO: add one-time construction code here
	srand(static_cast<unsigned int>(time(0)));
}

CNavalBattleDoc::~CNavalBattleDoc()
{
}

BOOL CNavalBattleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CNavalBattleDoc serialization

void CNavalBattleDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		m_SquareArray.Serialize(ar);
		navalBattle->Serialize(ar);
	}
	else
	{
		m_SquareArray.Serialize(ar);
		navalBattle->Serialize(ar);
	}
	UpdateAllViews(0);
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CNavalBattleDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CNavalBattleDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CNavalBattleDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CNavalBattleDoc diagnostics

#ifdef _DEBUG
void CNavalBattleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNavalBattleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNavalBattleDoc commands

IMPLEMENT_SERIAL(CSquare, CObject, 1) // Класс, баз.класс, версия
IMPLEMENT_SERIAL(CNavalBattle, CObject, 1) // Класс, баз.класс, версия
IMPLEMENT_SERIAL(CGamePoint, CObject, 1) // Класс, баз.класс, версия

void CSquare::Draw(CDC* pDC)
{
	pDC->FillRect(m_rect, new CBrush(RGB(m_r, m_g, m_b)));
}

void CNavalBattleDoc::AddSquare(CSquare* pSquare)
{
	m_SquareArray.Add(pSquare);
}

int CNavalBattleDoc::Turn(int x, int y) {

	return navalBattle->isHit(x, y, 1);
}

boolean CNavalBattleDoc::Check(int x, int y) {
	int** field = navalBattle->getPlayer2Field();
	bool res = field[x][y] == 2 || field[x][y] == 3;
	navalBattle->SetSquare(field, x, y);
	return res;
}

int CNavalBattleDoc::NbInit() {
	if (navalBattle == NULL) {
		navalBattle = new CNavalBattle();
		int** field = navalBattle->getPlayer1Field();
		CSquare* square;
		int x, y;
		for (int i = 0; i < 10; i++)
			for (int j = 0; j < 10; j++) {
				if (field[i][j] == 1) {
					x = 100 + 50 * i;
					y = 100 + 50 * j;
					square = new CSquare(x + 1, y + 1, x + 50 - 1, y + 50 - 1, 200, 200, 200);
					m_SquareArray.Add(square);
				}
			}


		return 0;
	}
	else
		return 1;
}


CSquare* CNavalBattleDoc::GetSquare(int Index)
{
	if (Index < 0 || Index > m_SquareArray.GetUpperBound())
		return 0;
	return m_SquareArray.GetAt(Index);
}

int CNavalBattleDoc::GetSquaresAmount()
{
	return m_SquareArray.GetSize();
}


void CNavalBattleDoc::DeleteContents()
{
	int Index = m_SquareArray.GetSize();
	while (Index--)
		delete m_SquareArray.GetAt(Index);
	m_SquareArray.RemoveAll();
	delete navalBattle;
	navalBattle = NULL;
	NbInit();
	CDocument::DeleteContents();
}


void CNavalBattleDoc::OnEditClearAll()
{
	DeleteContents();
	/*UpdateAllViews(0);*/
}

void CGamePoint::setLength(int length) {
	m_hitCount = length;
	m_length = length;
}

boolean CGamePoint::isHorizontal() {
	return orientation == Orientation::HORIZONTAL;
}

boolean CGamePoint::isDestroyed() {
	m_hitCount--;
	return m_hitCount <= 0;
}

int CGamePoint::getLength() {
	return m_length;
}

int CGamePoint::getX() {
	return m_x;
}

int CGamePoint::getY() {
	return m_y;
}

void CNavalBattle::fillField() {
	makeFields();
	int shipsAvailable[] = { 1,2,3,4 };
	int i = 0;
	while (i <= 3) {
		CGamePoint* point1 = getHead(player1Field, 4 - i);
		CGamePoint* point2 = getHead(player2Field, 4 - i);

		player1Points[point1->getX()][point1->getY()] = point1;
		player2Points[point2->getX()][point2->getY()] = point2;
		point1->setLength(4 - i);
		point2->setLength(4 - i);
		makeShip(point1, player1Field, 4 - i);
		makeShip(point2, player2Field, 4 - i);
		shipsAvailable[i] -= 1;
		if (shipsAvailable[i] <= 0)
			i++;
	}
}

void CNavalBattle::SetSquare(int** field,int x,int y) {
	field[x][y] = 3;
}

void CNavalBattle::makeShip(CGamePoint* point, int** field, int length) {
	if (point->isHorizontal()) {
		for (int i = 0; i < length; i++) {
			field[point->getX() + i][point->getY()] = 1;
		}
	}
	else {
		for (int i = 0; i < length; i++) {
			field[point->getX()][point->getY() + i] = 1;
		}
	}
}
void CNavalBattle::makeFields() {
	player1Field = new int* [10];
	player2Field = new int* [10];

	for (int i = 0; i < 10; i++) {
		player1Field[i] = new int[10];
		player2Field[i] = new int[10];
	}
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			player1Points[i][j] = NULL;
			player2Points[i][j] = NULL;
			player1Field[i][j] = 0;
			player2Field[i][j] = 0;
		}
	}
}
CGamePoint* CNavalBattle::getHead(int** field, int length) {
	CGamePoint* point = new CGamePoint();
	while (!checkPlacement(field, point, length))
		point = new CGamePoint();
	return point;
}
boolean CNavalBattle::checkPlacement(int** field, CGamePoint* point, int shipLength) {
	if (field[point->getX()][point->getY()] == 1)
		return false;
	if (point->isHorizontal()) {
		if (point->getX() + shipLength - 1 > 9)
			return false;
		for (int i = point->getX() - 1; i < point->getX() + shipLength + 1; i++) {
			if (i < 0)
				continue;
			if (i > 9)
				continue;
			if (field[i][point->getY()] == 1)
				return false;
			if (point->getY() - 1 > -1)
				if (field[i][point->getY() - 1] == 1)
					return false;
			if (point->getY() + 1 < 10)
				if (field[i][point->getY() + 1] == 1)
					return false;

		}

	}
	else {
		if (point->getY() + shipLength - 1 > 9)
			return false;
		for (int i = point->getY() - 1; i < point->getY() + shipLength + 1; i++) {
			if (i < 0)
				continue;
			if (i > 9)
				continue;
			if (field[point->getX()][i] == 1)
				return false;
			if (point->getX() - 1 > -1)
				if (field[point->getX() - 1][i] == 1)
					return false;
			if (point->getX() + 1 < 10)
				if (field[point->getX() + 1][i] == 1)
					return false;
		}
	}
	return true;

}
//void CNavalBattle::markAdjacent(int** field, CGamePoint* point) {
//	if (point->isHorizontal()) {
//		if (point->getX() - 1 > -1) {
//			field[point->getX() - 1][point->getY()] = 2;
//		}
//		if (point->getX() + point->getLength() < 10)
//			field[point->getX() + point->getLength()][point->getY()] = 2;
//		for (int i = point->getX() - 1; i < point->getX() + point->getLength() + 1; i++) {
//			if (i < 0)
//				continue;
//			if (i > 9)
//				continue;
//			if (point->getY() - 1 > -1)
//				field[i][point->getY() - 1] = 2;
//			if (point->getY() + 1 < 10)
//				field[i][point->getY() + 1] = 2;
//		}
//
//	}
//	else {
//		if (point->getY() - 1 > -1) {
//			field[point->getX()][point->getY() - 1] = 2;
//		}
//		if (point->getY() + point->getLength() < 10)
//			field[point->getX()][point->getY() + point->getLength()] = 2;
//		for (int i = point->getY() - 1; i < point->getY() + point->getLength() + 1; i++) {
//			if (i < 0)
//				continue;
//			if (i > 9)
//				continue;
//			if (point->getX() - 1 > -1)
//				field[point->getX() - 1][i] = 2;
//			if (point->getX() + 1 < 10)
//				field[point->getX() + 1][i] = 2;
//		}
//	}
//}

CGamePoint* CNavalBattle::FindHead(CGamePoint* points[10][10], int** field, CGamePoint* point) {
	if (points[point->getX()][point->getY()] != NULL)
		return points[point->getX()][point->getY()];
	int delta = 0;
	if (point->getX() - 1 > -1 && (field[point->getX() - 1][point->getY()] == 1 || field[point->getX() - 1][point->getY()] == 3)) {
		while (points[point->getX() - delta][point->getY()] == NULL)
			delta++;
		return points[point->getX() - delta][point->getY()];
	}
	else {
		while (points[point->getX()][point->getY() - delta] == NULL)
			delta++;
		return points[point->getX()][point->getY() - delta];
	}
}

boolean  CNavalBattle::isHit(int x, int y, int player) {
	CGamePoint* point = new CGamePoint(x, y);
	if (player) {
		if (player2Field[x][y] == 1) {
			player2Field[x][y] = 3;
			CGamePoint* head = FindHead(player2Points, player2Field, point);
			if (head->isDestroyed()) {
				player2Alive -= 1;
				/*markAdjacent(player2Field, head);*/
			}
			return true;
		}
		else {
			player2Field[x][y] = 2;
			return false;
		}
	}
	else {
		if (player1Field[x][y] == 1) {
			player1Field[x][y] = 3;
			CGamePoint* head = FindHead(player1Points, player1Field, point);
			if (head->isDestroyed()) {
				player1Alive -= 1;
				/*markAdjacent(player1Field, head);*/
			}
			return true;
		}
		else {
			player1Field[x][y] = 2;
			return false;
		}
	}

}

boolean  CNavalBattle::gameFinished() {
	return player1Alive == 0 || player2Alive == 0;
}
boolean CNavalBattle::makeAturn() {
	if (gameFinished())
		return false;
	CGamePoint* point = new CGamePoint();
	while (player1Field[point->getX()][point->getY()] > 1)
		point = new CGamePoint();

	return isHit(point->getX(), point->getY(), 0);

}

boolean CNavalBattleDoc::enemyTurn() {
	return navalBattle->makeAturn();
}

int** CNavalBattle::getPlayer1Field() {
	return player1Field;
}

CNavalBattle* CNavalBattleDoc::getNavalBattle() {
	return navalBattle;
}

int** CNavalBattle::getPlayer2Field() {
	return player2Field;
}

void CSquare::Serialize(CArchive& ar)
{

	if (ar.IsStoring()) {

		ar << m_rect->TopLeft().x << m_rect->TopLeft().y << m_rect->BottomRight().x << m_rect->BottomRight().y << m_r << m_g << m_b;
	}
	else {
		int x1, y1, x2, y2;
		ar >> x1 >> y1 >> x2 >> y2 >> m_r >> m_g >> m_b;
		m_rect = new CRect(x1, y1, x2, y2);
	}
}

void CNavalBattle::Serialize(CArchive& ar)
{

	if (ar.IsStoring()) {

		for (int i = 0; i < 10; i++)
			for (int j = 0; j < 10; j++) {
				ar << player1Field[i][j] << player2Field[i][j] << player1Points[i][j] << player2Points[i][j];
			}
		ar << player1Alive << player2Alive;
	}
	else {
		for (int i = 0; i < 10; i++)
			for (int j = 0; j < 10; j++) {
				ar >> player1Field[i][j] >> player2Field[i][j] >> player1Points[i][j] >> player2Points[i][j];
			}
		ar >> player1Alive >> player2Alive;
	}
}

void CGamePoint::Serialize(CArchive& ar)
{

	if (ar.IsStoring()) {
		int ot;
		if (orientation == Orientation::HORIZONTAL)
			ot = 1;
		else
			ot = 0;

		ar << m_x << m_y << m_length << m_hitCount << ot;
	}
	else {
		int ot;
		ar >> m_x >> m_y >> m_length >> m_hitCount >> ot;
		if (ot == 1)
			orientation = Orientation::HORIZONTAL;
		else
			orientation = Orientation::VERTICAL;

	}
}


boolean CSquare::isGray() {
	return RGB(m_r, m_g, m_b) == RGB(200, 200, 200);
}

void CNavalBattleDoc::ReplaceSquare(int Index, CSquare* Square) {
	m_SquareArray.SetAt(Index, Square);
}