// матрица симметрий
#define IN_PROGRESS 0
#define COMPUTER_WIN 1
#define USER_WIN 2
#define PARITY 3

#define RES_WIN 1
#define RES_PARITY 0
#define RES_LOOSE 2

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