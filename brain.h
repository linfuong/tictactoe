class Brain 
{
	struct NERV_CELL{
		BYTE cnt; // количество ходов
		BYTE first;
		BYTE result;// результат по окончанию игры
		BYTE field[9];// лог ходов игры
	};
	struct NERV_INPUT{
		BYTE cnt;		
		BYTE field[9];	// игровое поле
	};
	
	int Cmp(int *, int *, std::vector<int> &);
	int FindPattern();
	
	TCHAR fname[256];
	std::vector<NERV_CELL> mind_win;
	std::vector<NERV_CELL> mind_par;
	int *m_field;
	bool m_first;
	bool m_inProgress;
	bool m_who;
	bool m_needLearn;
	NERV_INPUT m_nerv;

public:
	Brain(TCHAR *);
	~Brain();
	void InitGame(bool, INT *);
	void Move();
	void MoveUser(int);
	bool CanMove();
	void Learn(int);	
	int AnalyseGame();
};