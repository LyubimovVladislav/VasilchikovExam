
// NavalBattleDoc.h : interface of the CNavalBattleDoc class
//


#pragma once

class CGamePoint : public CObject {
private:
	DECLARE_SERIAL(CGamePoint)
	int m_hitCount;
	enum class Orientation { VERTICAL, HORIZONTAL };
	Orientation orientation;
public:
	virtual void Serialize(CArchive& ar);
	int getLength();
	void setLength(int length);
	int m_x, m_y, m_length;
public:
	CGamePoint(int x, int y) {
		m_x = x;
		m_y = y;
		m_length = m_hitCount = 0;
		orientation = Orientation::HORIZONTAL;
	}
	CGamePoint() {
		m_x = rand() % 10;
		m_y = rand() % 10;
		m_length = m_hitCount = 0;
		if (rand() % 2)
			orientation = Orientation::HORIZONTAL;
		else
			orientation = Orientation::VERTICAL;
	}
public:
	boolean isHorizontal();
	boolean isDestroyed();
	int getX();
	int getY();

};


class CNavalBattle : public CObject
{
protected:
	int** player1Field;
	int** player2Field;
	CGamePoint* player1Points[10][10];
	CGamePoint* player2Points[10][10];
	int player1Alive = 10;
	int player2Alive = 10;
	void  fillField();
	void makeShip(CGamePoint* point, int** field, int length);
	void makeFields();
	CGamePoint* getHead(int** field, int length);
	boolean checkPlacement(int** field, CGamePoint* point, int shipLength);
	void markAdjacent(int** field, CGamePoint* point);
	CGamePoint* FindHead(CGamePoint* points[10][10], int** field, CGamePoint* point);
	DECLARE_SERIAL(CNavalBattle)
public:
	CNavalBattle() {
		fillField();
	}
public:
	boolean isHit(int x, int y, int player);
	boolean gameFinished();
	boolean makeAturn();
	void SetSquare(int** field, int x, int y);
	int** getPlayer1Field();
	int** getPlayer2Field();
	virtual void Serialize(CArchive& ar);
};

class CSquare : public CObject
{
protected:
	int m_r;
	int m_g;
	int m_b;
	CSquare() {}
	DECLARE_SERIAL(CSquare)
public:
	CRect* m_rect;
	CSquare(int X1, int Y1, int X2, int Y2, int r, int g, int b)
	{
		m_rect = new CRect(X1, Y1, X2, Y2);
		m_r = r;
		m_g = g;
		m_b = b;
	}
	CSquare(CRect * rect, int r, int g, int b)
	{
		m_rect = rect;
		m_r = r;
		m_g = g;
		m_b = b;
	}
	void Draw(CDC* PDC);
	virtual void Serialize(CArchive& ar);
	boolean isGray();
};


class CNavalBattleDoc : public CDocument
{
protected:
	CTypedPtrArray <CObArray, CSquare*> m_SquareArray;
	CNavalBattle* navalBattle = NULL;
public:
	void AddSquare(CSquare* pSquare);
	CSquare* GetSquare(int Index);
	int GetSquaresAmount();
	void ReplaceSquare(int Index, CSquare* Square);
	int Turn(int x, int y);
	int NbInit();
	boolean enemyTurn();
	boolean Check(int x, int y);
	CNavalBattle* getNavalBattle();

protected: // create from serialization only
	CNavalBattleDoc() noexcept;
	DECLARE_DYNCREATE(CNavalBattleDoc)

	// Attributes
public:

	// Operations
public:

	// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// Implementation
public:
	virtual ~CNavalBattleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual void DeleteContents();
	afx_msg void OnEditClearAll();
};
